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
APP_PROJ_VAR = [awtk.joinPath(TK_ROOT_VAR, 'demos/SConscript')]

SConscriptFiles=[
  awtk.joinPath(TK_ROOT_VAR, '3rd/mbedtls/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/nanovg/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/cjson/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/agg/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/agge/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/fribidi/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/gpinyin/SConscript'), 
  awtk.joinPath(TK_ROOT_VAR, '3rd/libunibreak/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, '3rd/miniz/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/streams/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/csv/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/conf_io/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/hal/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/ubjson/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'src/compressors/SConscript'),
  awtk.joinPath(TK_ROOT_VAR, 'tools/common/SConscript'), 
  awtk.joinPath(TK_ROOT_VAR, 'tools/ui_gen/xml_to_ui/SConscript'),
  'src/SConscript',
  ] + APP_PROJ_VAR;

SConscript(SConscriptFiles)
