import os
import platform
import shutil
import scons_argv
complie_helper = scons_argv.set_compile_config()

from awtk_config_common import TKC_STATIC_LIBS
from awtk_config_common import joinPath, toWholeArchive, genIdlAndDefEx, setEnvSpawn, genDllLinkFlags, copySharedLib, cleanSharedLib, scons_db_check_and_remove


OS_NAME = platform.system()

CWD=os.path.normpath(os.path.abspath(os.path.dirname(__file__)));

AWTK_VNC_ROOT = CWD
TK_ROOT          = joinPath(AWTK_VNC_ROOT, '../awtk')
TK_SRC           = joinPath(TK_ROOT, 'src')
TK_3RD_ROOT      = joinPath(TK_ROOT, '3rd')
GTEST_ROOT       = joinPath(TK_ROOT, '3rd/gtest/googletest')

BUILD_DIR        = joinPath(AWTK_VNC_ROOT, 'build')
BIN_DIR          = joinPath(BUILD_DIR, 'bin')
LIB_DIR          = joinPath(BUILD_DIR, 'lib')
VAR_DIR          = joinPath(BUILD_DIR, 'var')
TK_DEMO_ROOT     = joinPath(TK_ROOT, 'demos')

LCD_DIR        = joinPath(AWTK_VNC_ROOT, 'awtk-port/lcd_linux')
INPUT_DIR      = joinPath(AWTK_VNC_ROOT, 'awtk-port/input_thread')

# lcd devices
LCD_DEICES='fb'
LCD='LINUX_FB'
#VGCANVAS='CAIRO'
VGCANVAS='NANOVG'

INPUT_ENGINE='pinyin'

COMMON_CCFLAGS=' -DHAS_STD_MALLOC -DHAS_STDIO -DHAS_FAST_MEMCPY -DWITH_VGCANVAS -DWITH_UNICODE_BREAK -DTK_IS_PC '
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DLOAD_ASSET_WITH_MMAP=1 '
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_ASSET_LOADER -DWITH_FS_RES -DHAS_GET_TIME_US64=1 ' 
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DSTBTT_STATIC -DSTB_IMAGE_STATIC -DWITH_STB_IMAGE -DWITH_STB_FONT -DWITH_TEXT_BIDI=1 '

if VGCANVAS == 'CAIRO':
  NANOVG_BACKEND=''
  NANOVG_BACKEND_LIBS=['cairo', 'pixman'];
  VGCANVAS_PROJS=['3rd/cairo/SConscript', '3rd/pixman/SConscript'];
  COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_VGCANVAS_CAIRO -DHAVE_CONFIG_H -DCAIRO_WIN32_STATIC_BUILD '
else:
  NANOVG_BACKEND='AGGE'
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_NANOVG_AGGE '
  NANOVG_BACKEND_LIBS=['nanovg-agge', 'nanovg', 'agge'];
  VGCANVAS_PROJS=['3rd/nanovg/SConscript', '3rd/agge/SConscript'];

if INPUT_ENGINE == 't9':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_T9 '
elif INPUT_ENGINE == 't9ext' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_T9EXT'
elif INPUT_ENGINE == 'pinyin' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_PINYIN '
elif INPUT_ENGINE == 'spinyin' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_SPINYIN '
elif INPUT_ENGINE == 'null' :
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_NULL '

GRAPHIC_BUFFER='default'

#only for c compiler flags
COMMON_CFLAGS=''
COMMON_CFLAGS=COMMON_CFLAGS+' -std=gnu99 '

OS_LIBS=[]
OS_LIBPATH=[]
OS_CPPPATH=[]
OS_LINKFLAGS=''
OS_SUBSYSTEM_CONSOLE=''
OS_SUBSYSTEM_WINDOWS=''
OS_FLAGS='-Wall -g '

TOOLS_PREFIX=''
TSLIB_LIB_DIR=''
TARGET_ARCH = platform.architecture();

CC=TOOLS_PREFIX+'gcc',
CXX=TOOLS_PREFIX+'g++',
LD=TOOLS_PREFIX+'g++',
AR=TOOLS_PREFIX+'ar',
RANLIB=TOOLS_PREFIX+'ranlib',
STRIP=TOOLS_PREFIX+'strip',
OS_NAME = platform.system();

if OS_NAME == 'Darwin':
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DMACOS '
  OS_LIBS = ['stdc++', 'pthread', 'iconv', 'm', 'dl']
elif OS_NAME == 'Linux':
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DLINUX '
  OS_LIBS = ['stdc++', 'pthread', 'rt', 'm', 'dl']
elif OS_NAME == 'Windows':
  COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWIN32 '
else:
  print('not support ' + OS_NAME)


COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DHAS_PTHREAD -DENABLE_CURSOR -fPIC '
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_DATA_READER_WRITER=1 '
COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_EVENT_RECORDER_PLAYER=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_WIDGET_TYPE_CHECK=1 '

if TSLIB_LIB_DIR != '':
  COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DHAS_TSLIB '

CFLAGS=COMMON_CFLAGS
LINKFLAGS=OS_LINKFLAGS;
LIBPATH=[LIB_DIR, BIN_DIR] + OS_LIBPATH
CCFLAGS=OS_FLAGS + COMMON_CCFLAGS 

STATIC_LIBS =['awtk_global', 'fscript_ext_widgets', 'extwidgets', 'widgets', 'awtk_vnc', 'vncserver', 'base', 'gpinyin', 'linebreak', 'fribidi', 'svgtiny']
if TSLIB_LIB_DIR != '':
  SHARED_LIBS=['awtk', 'ts'] + OS_LIBS;
else:
  SHARED_LIBS=['awtk'] + OS_LIBS;

STATIC_LIBS = STATIC_LIBS + NANOVG_BACKEND_LIBS + TKC_STATIC_LIBS + OS_LIBS

LIBS=STATIC_LIBS

CPPPATH=[TK_ROOT, 
  TK_SRC, 
  TK_3RD_ROOT, 
  LCD_DIR, 
  INPUT_DIR, 
  joinPath(TK_SRC, 'ext_widgets'), 
  joinPath(TK_SRC, 'custom_widgets'), 
  joinPath(TK_ROOT, 'tools'), 
  joinPath(TK_3RD_ROOT, 'agge'), 
  joinPath(TK_3RD_ROOT, 'agg/include'), 
  joinPath(TK_3RD_ROOT, 'pixman'), 
  joinPath(TK_3RD_ROOT, 'cairo'),
  joinPath(TK_3RD_ROOT, 'nanovg'), 
  joinPath(TK_3RD_ROOT, 'nanovg/gl'), 
  joinPath(TK_3RD_ROOT, 'nanovg/base'), 
  joinPath(TK_3RD_ROOT, 'mbedtls/include'), 
  joinPath(TK_3RD_ROOT, 'mbedtls/3rdparty/everest/include'), 
  joinPath(TK_3RD_ROOT, 'fribidi'), 
  joinPath(TK_3RD_ROOT, 'nanovg/base'), 
  joinPath(TK_3RD_ROOT, 'libunibreak'), 
  joinPath(TK_3RD_ROOT, 'gpinyin/include'), 
  joinPath(TK_3RD_ROOT, 'gtest/googletest'), 
  joinPath(TK_3RD_ROOT, 'gtest/googletest/include'), 
  ] + OS_CPPPATH

if TSLIB_LIB_DIR != '':
  LIBS = ['ts'] + LIBS
  LIBPATH = [TSLIB_LIB_DIR] + LIBPATH;
  CPPPATH = [TSLIB_INC_DIR] + CPPPATH;

AWTK_STATIC_LIBS=LIBS
AWTK_DLL_DEPS_LIBS = NANOVG_BACKEND_LIBS + OS_LIBS
OS_WHOLE_ARCHIVE =toWholeArchive(LIBS)

os.environ['LCD'] = LCD
os.environ['LCD_DEICES'] = LCD_DEICES
os.environ['TARGET_ARCH'] = 'arm'
os.environ['BIN_DIR'] = BIN_DIR;
os.environ['LIB_DIR'] = LIB_DIR;
os.environ['TK_ROOT'] = TK_ROOT;
os.environ['CCFLAGS'] = CCFLAGS;
os.environ['VGCANVAS'] = VGCANVAS
os.environ['INPUT_ENGINE'] = INPUT_ENGINE;
os.environ['TSLIB_LIB_DIR'] = TSLIB_LIB_DIR;
os.environ['NANOVG_BACKEND'] = NANOVG_BACKEND;
os.environ['TK_3RD_ROOT'] = TK_3RD_ROOT;
os.environ['GTEST_ROOT'] = GTEST_ROOT;
os.environ['TOOLS_NAME'] = '';
os.environ['GRAPHIC_BUFFER'] = GRAPHIC_BUFFER;
os.environ['WITH_AWTK_SO'] = 'true'
os.environ['NATIVE_WINDOW'] = 'raw';
os.environ['OS_WHOLE_ARCHIVE'] = OS_WHOLE_ARCHIVE;

os.environ['AWTK_DLL_DEPS_LIBS'] = ';'.join(AWTK_DLL_DEPS_LIBS)
os.environ['STATIC_LIBS'] = ';'.join(STATIC_LIBS)

def has_custom_cc():
    return True

def copySharedLib(src, dst, name):
  if name == 'awtk':
    src = os.path.join(src, 'build');

  if OS_NAME == 'Darwin':
    src = os.path.join(src, 'bin/lib'+name+'.dylib')
  elif OS_NAME == 'Linux':
    src = os.path.join(src, 'bin/lib'+name+'.so')
  else:
    print('not support ' + OS_NAME)
    return
	
  src = os.path.normpath(src);
  dst = os.path.normpath(dst);

  if os.path.dirname(src) == dst:
      return

  if not os.path.exists(src):
    print('Can\'t find ' + src + '. Please build '+name+' before!')
  else:
    if not os.path.exists(dst):
        os.makedirs(dst)
    shutil.copy(src, dst)
    print(src + '==>' + dst);
