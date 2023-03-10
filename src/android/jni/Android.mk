# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

#
# LIB FREETYPE
#
include $(CLEAR_VARS)
LOCAL_MODULE    := gamefreetype
LOCAL_CFLAGS    := -Werror -DFT2_BUILD_LIBRARY -O3
LOCAL_SRC_FILES := \
../../lib/freetype-2.5.2/src/base/ftbase.c \
../../lib/freetype-2.5.2/src/base/ftbitmap.c \
../../lib/freetype-2.5.2/src/base/ftdebug.c \
../../lib/freetype-2.5.2/src/base/ftglyph.c \
../../lib/freetype-2.5.2/src/base/ftinit.c \
../../lib/freetype-2.5.2/src/base/ftsystem.c \
../../lib/freetype-2.5.2/src/gzip/ftgzip.c \
../../lib/freetype-2.5.2/src/psnames/psnames.c \
../../lib/freetype-2.5.2/src/sfnt/sfnt.c \
../../lib/freetype-2.5.2/src/smooth/smooth.c \
../../lib/freetype-2.5.2/src/truetype/truetype.c \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../lib/freetype-2.5.2/include
include $(BUILD_STATIC_LIBRARY)

#
# LIB LUA
#
include $(CLEAR_VARS)
LOCAL_MODULE    := gamelua
LOCAL_CFLAGS    := -Werror -O3
LOCAL_SRC_FILES := \
../../lib\lua-5.2.3\src\lapi.c \
../../lib\lua-5.2.3\src\lauxlib.c \
../../lib\lua-5.2.3\src\lbaselib.c \
../../lib\lua-5.2.3\src\lbitlib.c \
../../lib\lua-5.2.3\src\lcode.c \
../../lib\lua-5.2.3\src\lcorolib.c \
../../lib\lua-5.2.3\src\lctype.c \
../../lib\lua-5.2.3\src\ldblib.c \
../../lib\lua-5.2.3\src\ldebug.c \
../../lib\lua-5.2.3\src\ldo.c \
../../lib\lua-5.2.3\src\ldump.c \
../../lib\lua-5.2.3\src\lfunc.c \
../../lib\lua-5.2.3\src\lgc.c \
../../lib\lua-5.2.3\src\linit.c \
../../lib\lua-5.2.3\src\liolib.c \
../../lib\lua-5.2.3\src\llex.c \
../../lib\lua-5.2.3\src\lmathlib.c \
../../lib\lua-5.2.3\src\lmem.c \
../../lib\lua-5.2.3\src\loadlib.c \
../../lib\lua-5.2.3\src\lobject.c \
../../lib\lua-5.2.3\src\lopcodes.c \
../../lib\lua-5.2.3\src\loslib.c \
../../lib\lua-5.2.3\src\lparser.c \
../../lib\lua-5.2.3\src\lstate.c \
../../lib\lua-5.2.3\src\lstring.c \
../../lib\lua-5.2.3\src\lstrlib.c \
../../lib\lua-5.2.3\src\ltable.c \
../../lib\lua-5.2.3\src\ltablib.c \
../../lib\lua-5.2.3\src\ltm.c \
../../lib\lua-5.2.3\src\lundump.c \
../../lib\lua-5.2.3\src\lvm.c \
../../lib\lua-5.2.3\src\lzio.c \

include $(BUILD_STATIC_LIBRARY)

#
# LIB PNG
#
include $(CLEAR_VARS)
LOCAL_MODULE    := gamepng
LOCAL_CFLAGS    := -Werror -O3
LOCAL_SRC_FILES := \
../../lib\libpng-1.6.9\png.c \
../../lib\libpng-1.6.9\pngerror.c \
../../lib\libpng-1.6.9\pngget.c \
../../lib\libpng-1.6.9\pngmem.c \
../../lib\libpng-1.6.9\pngpread.c \
../../lib\libpng-1.6.9\pngread.c \
../../lib\libpng-1.6.9\pngrio.c \
../../lib\libpng-1.6.9\pngrtran.c \
../../lib\libpng-1.6.9\pngrutil.c \
../../lib\libpng-1.6.9\pngset.c \
../../lib\libpng-1.6.9\pngtrans.c \
../../lib\libpng-1.6.9\pngwio.c \
../../lib\libpng-1.6.9\pngwrite.c \
../../lib\libpng-1.6.9\pngwtran.c \
../../lib\libpng-1.6.9\pngwutil.c \

include $(BUILD_STATIC_LIBRARY)

#
# LIB TIF
#
include $(CLEAR_VARS)
LOCAL_MODULE    := gametif
LOCAL_CFLAGS    := -Werror -Wno-error=format -O3
LOCAL_SRC_FILES := \
../../lib\tiff-4.0.3\libtiff\tif_aux.c \
../../lib\tiff-4.0.3\libtiff\tif_color.c \
../../lib\tiff-4.0.3\libtiff\tif_codec.c \
../../lib\tiff-4.0.3\libtiff\tif_close.c \
../../lib\tiff-4.0.3\libtiff\tif_compress.c \
../../lib\tiff-4.0.3\libtiff\tif_dir.c \
../../lib\tiff-4.0.3\libtiff\tif_dirinfo.c \
../../lib\tiff-4.0.3\libtiff\tif_dirread.c \
../../lib\tiff-4.0.3\libtiff\tif_dirwrite.c \
../../lib\tiff-4.0.3\libtiff\tif_dumpmode.c \
../../lib\tiff-4.0.3\libtiff\tif_error.c \
../../lib\tiff-4.0.3\libtiff\tif_extension.c \
../../lib\tiff-4.0.3\libtiff\tif_fax3.c \
../../lib\tiff-4.0.3\libtiff\tif_fax3sm.c \
../../lib\tiff-4.0.3\libtiff\tif_flush.c \
../../lib\tiff-4.0.3\libtiff\tif_getimage.c \
../../lib\tiff-4.0.3\libtiff\tif_jbig.c \
../../lib\tiff-4.0.3\libtiff\tif_jpeg.c \
../../lib\tiff-4.0.3\libtiff\tif_jpeg_12.c \
../../lib\tiff-4.0.3\libtiff\tif_luv.c \
../../lib\tiff-4.0.3\libtiff\tif_lzma.c \
../../lib\tiff-4.0.3\libtiff\tif_lzw.c \
../../lib\tiff-4.0.3\libtiff\tif_next.c \
../../lib\tiff-4.0.3\libtiff\tif_ojpeg.c \
../../lib\tiff-4.0.3\libtiff\tif_open.c \
../../lib\tiff-4.0.3\libtiff\tif_packbits.c \
../../lib\tiff-4.0.3\libtiff\tif_pixarlog.c \
../../lib\tiff-4.0.3\libtiff\tif_predict.c \
../../lib\tiff-4.0.3\libtiff\tif_print.c \
../../lib\tiff-4.0.3\libtiff\tif_read.c \
../../lib\tiff-4.0.3\libtiff\tif_stream.cxx \
../../lib\tiff-4.0.3\libtiff\tif_strip.c \
../../lib\tiff-4.0.3\libtiff\tif_swab.c \
../../lib\tiff-4.0.3\libtiff\tif_thunder.c \
../../lib\tiff-4.0.3\libtiff\tif_tile.c \
../../lib\tiff-4.0.3\libtiff\tif_unix.c \
../../lib\tiff-4.0.3\libtiff\tif_version.c \
../../lib\tiff-4.0.3\libtiff\tif_warning.c \
../../lib\tiff-4.0.3\libtiff\tif_write.c \
../../lib\tiff-4.0.3\libtiff\tif_zip.c \

include $(BUILD_STATIC_LIBRARY)

#
# LIB Z
#
include $(CLEAR_VARS)
LOCAL_MODULE    := gamez
LOCAL_CFLAGS    := -Werror -Wno-error=format -O3
LOCAL_SRC_FILES := \
../..\lib\zlib-1.2.8\adler32.c \
../..\lib\zlib-1.2.8\compress.c \
../..\lib\zlib-1.2.8\crc32.c \
../..\lib\zlib-1.2.8\deflate.c \
../..\lib\zlib-1.2.8\gzclose.c \
../..\lib\zlib-1.2.8\gzlib.c \
../..\lib\zlib-1.2.8\gzread.c \
../..\lib\zlib-1.2.8\gzwrite.c \
../..\lib\zlib-1.2.8\infback.c \
../..\lib\zlib-1.2.8\inffast.c \
../..\lib\zlib-1.2.8\inflate.c \
../..\lib\zlib-1.2.8\inftrees.c \
../..\lib\zlib-1.2.8\trees.c \
../..\lib\zlib-1.2.8\uncompr.c \
../..\lib\zlib-1.2.8\zutil.c \

include $(BUILD_STATIC_LIBRARY)

#
# LIB LIBZIP
#
include $(CLEAR_VARS)
LOCAL_MODULE    := gamelibzip
LOCAL_CFLAGS    := -Werror -Wno-error=format -O3
LOCAL_SRC_FILES := \
../../lib/libzip-0.11.2\lib\zip_add.c \
../../lib/libzip-0.11.2\lib\zip_add_dir.c \
../../lib/libzip-0.11.2\lib\zip_add_entry.c \
../../lib/libzip-0.11.2\lib\zip_close.c \
../../lib/libzip-0.11.2\lib\zip_delete.c \
../../lib/libzip-0.11.2\lib\zip_dir_add.c \
../../lib/libzip-0.11.2\lib\zip_dirent.c \
../../lib/libzip-0.11.2\lib\zip_discard.c \
../../lib/libzip-0.11.2\lib\zip_entry.c \
../../lib/libzip-0.11.2\lib\zip_err_str.c \
../../lib/libzip-0.11.2\lib\zip_error.c \
../../lib/libzip-0.11.2\lib\zip_error_clear.c \
../../lib/libzip-0.11.2\lib\zip_error_get.c \
../../lib/libzip-0.11.2\lib\zip_error_get_sys_type.c \
../../lib/libzip-0.11.2\lib\zip_error_strerror.c \
../../lib/libzip-0.11.2\lib\zip_error_to_str.c \
../../lib/libzip-0.11.2\lib\zip_extra_field.c \
../../lib/libzip-0.11.2\lib\zip_extra_field_api.c \
../../lib/libzip-0.11.2\lib\zip_fclose.c \
../../lib/libzip-0.11.2\lib\zip_fdopen.c \
../../lib/libzip-0.11.2\lib\zip_file_add.c \
../../lib/libzip-0.11.2\lib\zip_file_error_clear.c \
../../lib/libzip-0.11.2\lib\zip_file_error_get.c \
../../lib/libzip-0.11.2\lib\zip_file_get_comment.c \
../../lib/libzip-0.11.2\lib\zip_file_get_external_attributes.c \
../../lib/libzip-0.11.2\lib\zip_file_get_offset.c \
../../lib/libzip-0.11.2\lib\zip_file_rename.c \
../../lib/libzip-0.11.2\lib\zip_file_replace.c \
../../lib/libzip-0.11.2\lib\zip_file_set_comment.c \
../../lib/libzip-0.11.2\lib\zip_file_set_external_attributes.c \
../../lib/libzip-0.11.2\lib\zip_file_strerror.c \
../../lib/libzip-0.11.2\lib\zip_filerange_crc.c \
../../lib/libzip-0.11.2\lib\zip_fopen.c \
../../lib/libzip-0.11.2\lib\zip_fopen_encrypted.c \
../../lib/libzip-0.11.2\lib\zip_fopen_index.c \
../../lib/libzip-0.11.2\lib\zip_fopen_index_encrypted.c \
../../lib/libzip-0.11.2\lib\zip_fread.c \
../../lib/libzip-0.11.2\lib\zip_get_archive_comment.c \
../../lib/libzip-0.11.2\lib\zip_get_archive_flag.c \
../../lib/libzip-0.11.2\lib\zip_get_compression_implementation.c \
../../lib/libzip-0.11.2\lib\zip_get_encryption_implementation.c \
../../lib/libzip-0.11.2\lib\zip_get_file_comment.c \
../../lib/libzip-0.11.2\lib\zip_get_num_entries.c \
../../lib/libzip-0.11.2\lib\zip_get_num_files.c \
../../lib/libzip-0.11.2\lib\zip_get_name.c \
../../lib/libzip-0.11.2\lib\zip_memdup.c \
../../lib/libzip-0.11.2\lib\zip_name_locate.c \
../../lib/libzip-0.11.2\lib\zip_new.c \
../../lib/libzip-0.11.2\lib\zip_open.c \
../../lib/libzip-0.11.2\lib\zip_rename.c \
../../lib/libzip-0.11.2\lib\zip_replace.c \
../../lib/libzip-0.11.2\lib\zip_set_archive_comment.c \
../../lib/libzip-0.11.2\lib\zip_set_archive_flag.c \
../../lib/libzip-0.11.2\lib\zip_set_default_password.c \
../../lib/libzip-0.11.2\lib\zip_set_file_comment.c \
../../lib/libzip-0.11.2\lib\zip_set_file_compression.c \
../../lib/libzip-0.11.2\lib\zip_set_name.c \
../../lib/libzip-0.11.2\lib\zip_source_buffer.c \
../../lib/libzip-0.11.2\lib\zip_source_close.c \
../../lib/libzip-0.11.2\lib\zip_source_crc.c \
../../lib/libzip-0.11.2\lib\zip_source_deflate.c \
../../lib/libzip-0.11.2\lib\zip_source_error.c \
../../lib/libzip-0.11.2\lib\zip_source_file.c \
../../lib/libzip-0.11.2\lib\zip_source_filep.c \
../../lib/libzip-0.11.2\lib\zip_source_free.c \
../../lib/libzip-0.11.2\lib\zip_source_function.c \
../../lib/libzip-0.11.2\lib\zip_source_layered.c \
../../lib/libzip-0.11.2\lib\zip_source_open.c \
../../lib/libzip-0.11.2\lib\zip_source_pkware.c \
../../lib/libzip-0.11.2\lib\zip_source_pop.c \
../../lib/libzip-0.11.2\lib\zip_source_read.c \
../../lib/libzip-0.11.2\lib\zip_source_stat.c \
../../lib/libzip-0.11.2\lib\zip_source_window.c \
../../lib/libzip-0.11.2\lib\zip_source_zip.c \
../../lib/libzip-0.11.2\lib\zip_source_zip_new.c \
../../lib/libzip-0.11.2\lib\zip_stat.c \
../../lib/libzip-0.11.2\lib\zip_stat_index.c \
../../lib/libzip-0.11.2\lib\zip_stat_init.c \
../../lib/libzip-0.11.2\lib\zip_strerror.c \
../../lib/libzip-0.11.2\lib\zip_string.c \
../../lib/libzip-0.11.2\lib\zip_unchange.c \
../../lib/libzip-0.11.2\lib\zip_unchange_all.c \
../../lib/libzip-0.11.2\lib\zip_unchange_archive.c \
../../lib/libzip-0.11.2\lib\zip_unchange_data.c \
../../lib/libzip-0.11.2\lib\zip_utf-8.c \

include $(BUILD_STATIC_LIBRARY)

#
# FMOD
# 
include $(CLEAR_VARS)
LOCAL_MODULE := gamefmod
LOCAL_SRC_FILES := ../../lib/fmod-1.03.08/android/lowlevel/lib/$(TARGET_ARCH_ABI)/libfmod.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := gamefmodstudio
LOCAL_SRC_FILES := ../../lib/fmod-1.03.08/android/studio/lib/$(TARGET_ARCH_ABI)/libfmodstudio.so
include $(PREBUILT_SHARED_LIBRARY)

#
# GAME
#
include $(CLEAR_VARS)

LOCAL_MODULE    := game
LOCAL_CFLAGS    := -Werror -include "precomp.h" -O3

FILE_LIST := \
$(wildcard $(LOCAL_PATH)/../../engine/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/2d/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/3d/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/core/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/file/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/input/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/log/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/math/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/params/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/platform/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/resource/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/script/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/sound/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/ui/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../engine/world/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../game/*.cpp) \
$(wildcard $(LOCAL_PATH)/../../game/component/*.cpp) 

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%) 

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../engine \
$(LOCAL_PATH)/../../game \
$(LOCAL_PATH)/../../lib \
$(LOCAL_PATH)/../../lib/freetype-2.5.2/include \
$(LOCAL_PATH)/../../lib/fmod-1.03.08/android/lowlevel/inc \
$(LOCAL_PATH)/../../lib/fmod-1.03.08/android/studio/inc \
$(LOCAL_PATH)/../../lib/gpg-cpp-sdk-1.1/android/include \

LOCAL_LDLIBS    := -llog -lGLESv2 -landroid
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/../../lib/gpg-cpp-sdk-1.1/android/lib/gnustl/$(TARGET_ARCH_ABI) -lgpg
LOCAL_LDFLAGS += -L$(NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.8/libs/$(TARGET_ARCH_ABI) -lgnustl_static
LOCAL_SHARED_LIBRARIES := gamefmod gamefmodstudio
LOCAL_STATIC_LIBRARIES := gamefreetype gamelua gamepng gametif gamez gamelibzip

include $(BUILD_SHARED_LIBRARY)
