/**
 * File:   vnc.c
 * Author: AWTK Develop Team
 * Brief:  main loop for vnc
 *
 * Copyright (c) 2018 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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

static const int32_t s_key_map[0xffff] = {
    [XK_1] = TK_KEY_1,
    [XK_2] = TK_KEY_2,
    [XK_3] = TK_KEY_3,
    [XK_4] = TK_KEY_4,
    [XK_5] = TK_KEY_5,
    [XK_6] = TK_KEY_6,
    [XK_7] = TK_KEY_7,
    [XK_8] = TK_KEY_8,
    [XK_9] = TK_KEY_9,
    [XK_0] = TK_KEY_0,
    [XK_A] = TK_KEY_a,
    [XK_B] = TK_KEY_b,
    [XK_C] = TK_KEY_c,
    [XK_D] = TK_KEY_d,
    [XK_E] = TK_KEY_e,
    [XK_F] = TK_KEY_f,
    [XK_G] = TK_KEY_g,
    [XK_H] = TK_KEY_h,
    [XK_I] = TK_KEY_i,
    [XK_J] = TK_KEY_j,
    [XK_K] = TK_KEY_k,
    [XK_L] = TK_KEY_l,
    [XK_M] = TK_KEY_m,
    [XK_N] = TK_KEY_n,
    [XK_O] = TK_KEY_o,
    [XK_P] = TK_KEY_p,
    [XK_Q] = TK_KEY_q,
    [XK_R] = TK_KEY_r,
    [XK_S] = TK_KEY_s,
    [XK_T] = TK_KEY_t,
    [XK_U] = TK_KEY_u,
    [XK_V] = TK_KEY_v,
    [XK_W] = TK_KEY_w,
    [XK_X] = TK_KEY_x,
    [XK_Y] = TK_KEY_y,
    [XK_Z] = TK_KEY_z,
    [XK_F1] = TK_KEY_F1,
    [XK_F2] = TK_KEY_F2,
    [XK_F3] = TK_KEY_F3,
    [XK_F4] = TK_KEY_F4,
    [XK_F5] = TK_KEY_F5,
    [XK_F6] = TK_KEY_F6,
    [XK_F7] = TK_KEY_F7,
    [XK_F8] = TK_KEY_F8,
    [XK_F9] = TK_KEY_F9,
    [XK_Escape] = TK_KEY_ESCAPE,
    [XK_Tab] = TK_KEY_TAB,
    [XK_Left] = TK_KEY_LEFT,
    [XK_Right] = TK_KEY_RIGHT,
    [XK_Up] = TK_KEY_UP,
    [XK_Down] = TK_KEY_DOWN,
    [XK_Return] = TK_KEY_RETURN,
    [XK_BackSpace] = TK_KEY_BACKSPACE,
};

static int32_t map_key(uint16_t code) {
  return s_key_map[code];
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

static enum rfbNewClientAction newclient(rfbClientPtr cl) {
  vnc_server_t* vnc = s_vnc_server;

  vnc->client = cl;
  cl->clientData = vnc;
  cl->clientGoneHook = clientgone;
  log_debug("client connected: %p\n", cl);
  /*repaint*/
  widget_invalidate_force(window_manager(), NULL);

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
  uint32_t row_size = r->w * vnc->bpp;

  if (vnc == NULL || vnc->client == NULL) {
    return RET_OK;
  }

  assert(w == vnc->w && h == vnc->h && bpp == vnc->bpp);

  dst_fb = (uint32_t*)(vnc->rfbScreen->frameBuffer);
  return_value_if_fail(fb != NULL && s_vnc_server != NULL && r != NULL, RET_BAD_PARAMS);

  for (i = 0; i < r->h; i++) {
    uint32_t offset = (r->y + i) * vnc->w + r->x;
    uint32_t* src = src_fb + offset;
    uint32_t* dst = dst_fb + offset;

    memcpy(dst, src, row_size);
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
