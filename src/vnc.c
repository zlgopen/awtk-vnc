/**
 * File:   vnc.c
 * Author: AWTK Develop Team
 * Brief:  vnc thread
 *
 * Copyright (c) 2018 - 2021  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * license file for more details.
 *
 */

/**
 * history:
 * ================================================================
 * 2021-04-23 li xianjing <xianjimli@hotmail.com> created
 *
 */

#include "vnc.h"
#include "tkc/mutex.h"
#include "tkc/thread.h"
#include "base/window_manager.h"

#include <rfb/rfb.h>
#include <rfb/keysym.h>

static int32_t map_key(uint16_t code) {
  if (code < 0x255) {
    return code;
  }

  switch(code) {
    case XK_F1: return TK_KEY_F1;
    case XK_F2: return TK_KEY_F2;
    case XK_F3: return TK_KEY_F3;
    case XK_F4: return TK_KEY_F4;
    case XK_F5: return TK_KEY_F5;
    case XK_F6: return TK_KEY_F6;
    case XK_F7: return TK_KEY_F7;
    case XK_F8: return TK_KEY_F8;
    case XK_F9: return TK_KEY_F9;
    case XK_F10: return TK_KEY_F10;
    case XK_F11: return TK_KEY_F11;
    case XK_F12: return TK_KEY_F12;
    case XK_Escape: return TK_KEY_ESCAPE;
    case XK_Tab: return TK_KEY_TAB;
    case XK_Left: return TK_KEY_LEFT;
    case XK_Right: return TK_KEY_RIGHT;
    case XK_Up: return TK_KEY_UP;
    case XK_Down: return TK_KEY_DOWN;
    case XK_Return: return TK_KEY_RETURN;
    case XK_BackSpace: return TK_KEY_BACKSPACE;
    default: break;
  }

  return TK_KEY_0;
}

typedef struct _vnc_server_t {
  int w;
  int h;
  int bpp;
  int client_nr;
  main_loop_t* loop;

  bool_t running;
  tk_thread_t* thread;

  rfbClientPtr client;
  rfbScreenInfoPtr rfbScreen;
} vnc_server_t;

static vnc_server_t* s_vnc_server;

static void clientgone(rfbClientPtr cl) {
  vnc_server_t* vnc = (vnc_server_t*)(cl->clientData);

  vnc->client = NULL;
  cl->clientData = NULL;
  log_debug("client disconnected: %p\n", cl);
}

static ret_t idle_request_redraw(const idle_info_t* info) {
  widget_invalidate_force(window_manager(), NULL);
  return RET_REMOVE;
}

static enum rfbNewClientAction newclient(rfbClientPtr cl) {
  vnc_server_t* vnc = s_vnc_server;

  vnc->client = cl;
  cl->clientData = vnc;
  cl->clientGoneHook = clientgone;
  log_debug("client connected: %p\n", cl);

  idle_add(idle_request_redraw, NULL);

  return RFB_CLIENT_ACCEPT;
}

static void on_pointer_event(int buttonMask, int x, int y, rfbClientPtr cl) {
  vnc_server_t* vnc = (vnc_server_t*)(cl->clientData);

  if (x >= 0 && y >= 0 && x < vnc->w && y < vnc->h) {
    main_loop_post_pointer_event(vnc->loop, buttonMask, x, y);
  }

  rfbDefaultPtrAddEvent(buttonMask, x, y, cl);
}

static void on_key_event(rfbBool down, rfbKeySym key, rfbClientPtr cl) {
  vnc_server_t* vnc = (vnc_server_t*)(cl->clientData);

  main_loop_post_key_event(vnc->loop, down, map_key(key));
}

static void* vnc_thread_func(void* args) {
  int argc = 1;
  static char* argv[] = {"vnc", NULL};
  vnc_server_t* vnc = (vnc_server_t*)args;
  uint32_t fb_size = vnc->w * vnc->h * vnc->bpp;
  rfbScreenInfoPtr rfbScreen = rfbGetScreen(&argc, (char**)argv, vnc->w, vnc->h, 8, 3, vnc->bpp);

  return_value_if_fail(rfbScreen != NULL, NULL);

  vnc->rfbScreen = rfbScreen;
  rfbScreen->desktopName = "AWTK";
  rfbScreen->frameBuffer = (char*)TKMEM_ALLOC(fb_size);
  rfbScreen->ptrAddEvent = on_pointer_event;
  rfbScreen->kbdAddEvent = on_key_event;
  rfbScreen->newClientHook = newclient;
  rfbScreen->alwaysShared = TRUE;

  rfbInitServer(rfbScreen);

  while (vnc->running) {
    int i;
    for (i = 0; rfbIsActive(rfbScreen); i++) {
      rfbProcessEvents(rfbScreen, 1000);
      if (!vnc->running) {
        break;
      }
    }
  }

  TKMEM_FREE(rfbScreen->frameBuffer);
  rfbScreenCleanup(rfbScreen);

  return NULL;
}

ret_t vnc_start(int w, int h, int bpp, main_loop_t* loop) {
  return_value_if_fail(s_vnc_server == NULL && loop != NULL && w > 1 && h > 1, RET_BAD_PARAMS);
  s_vnc_server = TKMEM_ZALLOC(vnc_server_t);
  return_value_if_fail(s_vnc_server != NULL, RET_BAD_PARAMS);

  s_vnc_server->w = w;
  s_vnc_server->h = h;
  s_vnc_server->bpp = bpp;
  s_vnc_server->loop = loop;
  s_vnc_server->running = TRUE;
  s_vnc_server->thread = tk_thread_create(vnc_thread_func, s_vnc_server);
  goto_error_if_fail(s_vnc_server->thread != NULL);
  tk_thread_start(s_vnc_server->thread);

  return RET_OK;
error:
  TKMEM_FREE(s_vnc_server);
  return RET_FAIL;
}

ret_t vnc_update_screen(uint8_t* fb, int w, int h, int bpp, rect_t* dirty_rect) {
  uint32_t i = 0;
  rect_t* r = dirty_rect;
  uint32_t* dst_fb = NULL;
  uint32_t* src_fb = (uint32_t*)fb;
  vnc_server_t* vnc = s_vnc_server;

  if (vnc == NULL || vnc->client == NULL) {
    return RET_OK;
  }

  assert(w == vnc->w && h == vnc->h && bpp == vnc->bpp);

  dst_fb = (uint32_t*)(vnc->rfbScreen->frameBuffer);
  return_value_if_fail(fb != NULL && s_vnc_server != NULL && r != NULL, RET_BAD_PARAMS);

  if (r->w == w && r->h == h && r->x == 0 && r->y == 0) {
    uint32_t size = w * h * bpp;
    memcpy(dst_fb, src_fb, size);
    log_debug("dirty1 %d %d %d %d\n", r->x, r->y, r->w, r->h);
  } else if (r->w == w && r->x == 0) {
    uint32_t offset = r->y * w;
    uint32_t* src = src_fb + offset;
    uint32_t* dst = dst_fb + offset;
    uint32_t size = w * r->h * bpp;

    memcpy(dst, src, size);
    log_debug("dirty2 %d %d %d %d\n", r->x, r->y, r->w, r->h);
  } else {
    uint32_t row_size = r->w * bpp;
    for (i = 0; i < r->h; i++) {
      uint32_t offset = (r->y + i) * w + r->x;
      uint32_t* src = src_fb + offset;
      uint32_t* dst = dst_fb + offset;

      memcpy(dst, src, row_size);
    }
    log_debug("dirty3 %d %d %d %d\n", r->x, r->y, r->w, r->h);
  }

  rfbMarkRectAsModified(vnc->rfbScreen, r->x, r->y, r->x + r->w, r->y + r->h);

  return RET_OK;
}

ret_t vnc_stop(void) {
  return_value_if_fail(s_vnc_server != NULL, RET_BAD_PARAMS);
  s_vnc_server->running = FALSE;
  tk_thread_join(s_vnc_server->thread);
  tk_thread_destroy(s_vnc_server->thread);
  TKMEM_FREE(s_vnc_server);

  return RET_OK;
}
