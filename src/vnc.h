#ifndef VNC_H
#define VNC_H

#include "tkc/mem.h"
#include "tkc/rect.h"
#include "tkc/event.h"
#include "base/main_loop.h"
#include "main_loop/main_loop_simple.h"

ret_t vnc_start(int w, int h, int bpp, main_loop_t* loop);

ret_t vnc_update_screen(uint8_t* fb, int w, int h, int bpp, rect_t* dirty_rect);

ret_t vnc_stop(void);

#endif /*VNC_H*/
