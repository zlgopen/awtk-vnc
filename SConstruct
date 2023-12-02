import os
import platform
import shutil
import atexit
import awtk_config as awtk

APP_CCFLAGS = ' '

LCD = ARGUMENTS.get('LCD', '')
if len(LCD) > 0:
  wh = LCD.split('_')
  if len(wh) >= 2:
    APP_CCFLAGS = ' -DLCD_WIDTH=' + wh[0] + ' -DLCD_HEIGHT=' + wh[1] + ' '
  else:
    print('LCD param input format error, e.g. LCD=800_480')
    exit(0)

env = DefaultEnvironment(CCFLAGS = awtk.CCFLAGS + APP_CCFLAGS, 
  CFLAGS = awtk.CFLAGS,
  CC=awtk.CC,
  CXX=awtk.CXX,
  LD=awtk.LD,
  AR=awtk.AR,
  RANLIB=awtk.RANLIB,
  STRIP=awtk.STRIP,
  LIBS = awtk.LIBS,
  LIBPATH = awtk.LIBPATH,
  CPPPATH = awtk.CPPPATH + [awtk.joinPath(awtk.TK_ROOT, 'res')],
  LINKFLAGS = awtk.LINKFLAGS,
  OS_SUBSYSTEM_CONSOLE=awtk.OS_SUBSYSTEM_CONSOLE,
  OS_SUBSYSTEM_WINDOWS=awtk.OS_SUBSYSTEM_WINDOWS
)

TK_ROOT_VAR = awtk.joinPath(awtk.VAR_DIR, 'awtk')
VariantDir(TK_ROOT_VAR, awtk.TK_ROOT)

VNC_PROP_VAR = awtk.joinPath(awtk.VAR_DIR, 'awtk-vnc') 
VariantDir(VNC_PROP_VAR, os.getcwd())

APP_PROJS = [awtk.joinPath(TK_ROOT_VAR, 'demos/SConscript'),
  awtk.joinPath(VNC_PROP_VAR, 'src/SConscript')]

def add_var_path(iter):
  return awtk.joinPath(TK_ROOT_VAR, iter)

VGCANVAS_PROJS = []
for i in awtk.VGCANVAS_PROJS:
  VGCANVAS_PROJS.append(awtk.joinPath(TK_ROOT_VAR, i))
  
print(VGCANVAS_PROJS)
SConscriptFiles=[
  awtk.joinPath(TK_ROOT_VAR, '3rd/mbedtls/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/cjson/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/fribidi/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/gpinyin/SConscript'), 
  awtk.joinPath(TK_ROOT_VAR, '3rd/svgtiny/SConscript'), 
  awtk.joinPath(TK_ROOT_VAR, '3rd/libunibreak/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/miniz/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/xml/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/romfs/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/charset/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/streams/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/csv/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/conf_io/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/hal/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/ubjson/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/fscript_ext/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/debugger/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/compressors/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'tools/common/SConscript'), 
  awtk.joinPath(TK_ROOT_VAR, 'tools/ui_gen/xml_to_ui/SConscript'),
  ] + APP_PROJS + VGCANVAS_PROJS


print(APP_PROJS)
SConscript(SConscriptFiles)
