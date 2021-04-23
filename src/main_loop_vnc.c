/**
 * File:   main_loop_vnc.c
 * Author: AWTK Develop Team
 * Brief:  vnc implemented main_loop interface
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
#include "tkc/mem.h"
#include "base/idle.h"
#include "base/timer.h"
#include "base/font_manager.h"
#include "base/window_manager.h"
#include "lcd/lcd_mem_rgba8888.h"
#include "main_loop/main_loop_simple.h"
#include "native_window/native_window_raw.h"

static ret_t main_loop_vnc_destroy(main_loop_t* l) {
  main_loop_simple_t* loop = (main_loop_simple_t*)l;

  vnc_stop();
  TKMEM_FREE(loop->user1);
  main_loop_simple_reset(loop);
  native_window_raw_deinit();

  return RET_OK;
}

#define BPP 4

static ret_t lcd_on_flush_vnc(lcd_t* lcd) {
  rect_t* r = &(lcd->dirty_rect);
  lcd_mem_t* mem = (lcd_mem_t*)lcd;

  vnc_update_screen(mem->offline_fb, lcd->w, lcd->h, BPP, r);

  return RET_OK;
}

main_loop_t* main_loop_init(int w, int h) {
  lcd_t* lcd = NULL;
  main_loop_simple_t* loop = NULL;
  uint32_t fb_size = w * h * BPP;
  uint8_t* buff = (uint8_t*)TKMEM_ALLOC(fb_size);
  return_value_if_fail(buff != NULL, NULL);

  lcd = lcd_mem_rgba8888_create_single_fb(w, h, buff);
  goto_error_if_fail(lcd != NULL);

  native_window_raw_init(lcd);
  lcd->flush = lcd_on_flush_vnc;
  loop = main_loop_simple_init(lcd->w, lcd->h, NULL, NULL);
  loop->base.destroy = main_loop_vnc_destroy;
  loop->user1 = buff;

  vnc_start(w, h, BPP, (main_loop_t*)loop);

  return (main_loop_t*)loop;
error:
  TKMEM_FREE(buff);
  lcd_destroy(lcd);

  return NULL;
}
