/**
 * File:   vnc.h
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

#ifndef VNC_H
#define VNC_H

#include "tkc/mem.h"
#include "tkc/rect.h"
#include "tkc/event.h"
#include "base/main_loop.h"
#include "main_loop/main_loop_simple.h"

/**
 * @class vnc_t
 * @annotation ["fake"]
 * 对VNC的包装。
 */

/**
 * @method vnc_start
 * 启动vnc线程。
 * 
 * @param {int} w LCD宽度。
 * @param {int} h LCD高度。
 * @param {int} bpp 每像素的字节数。
 * @param {main_loop_t*} loop 主循环。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t vnc_start(int w, int h, int bpp, main_loop_t* loop);

/**
 * @method vnc_update_screen 
 * 更新屏幕。
 * > w/h/bpp必须和vnc_start中指定的一致。
 *  
 * @param {uint8_t*} fb 缓冲区。
 * @param {int} w LCD宽度。
 * @param {int} h LCD高度。
 * @param {int} bpp 每像素的字节数。
 * @param {rect_t*} dirty_rect 脏矩形。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t vnc_update_screen(uint8_t* fb, int w, int h, int bpp, rect_t* dirty_rect);

/**
 * @method vnc_stop
 * 停止vnc线程。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t vnc_stop(void);

#endif /*VNC_H*/
