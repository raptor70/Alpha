#include "lib_include.h"

#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable : 4996)
//#pragma warning(disable : 4005)

#define NO_DUMMY_DECL
#include INCLUDE_LIB(LIB_ZLIB , trees.c)
#include INCLUDE_LIB(LIB_ZLIB , adler32.c)
#include INCLUDE_LIB(LIB_ZLIB , compress.c)
#undef DO1
#undef DO8
#include INCLUDE_LIB(LIB_ZLIB , crc32.c)
#include INCLUDE_LIB(LIB_ZLIB , deflate.c)
#undef GZIP
#include INCLUDE_LIB(LIB_ZLIB , gzclose.c)
#include INCLUDE_LIB(LIB_ZLIB , gzlib.c)
#include INCLUDE_LIB(LIB_ZLIB , gzread.c)
#include INCLUDE_LIB(LIB_ZLIB , gzwrite.c)
#undef COPY
#include INCLUDE_LIB(LIB_ZLIB , infback.c)
#include INCLUDE_LIB(LIB_ZLIB , inffast.c)
#undef COPY
#undef PULLBYTE
#include INCLUDE_LIB(LIB_ZLIB , inflate.c)
#include INCLUDE_LIB(LIB_ZLIB , inftrees.c)
#include INCLUDE_LIB(LIB_ZLIB , uncompr.c)
#include INCLUDE_LIB(LIB_ZLIB , zutil.c)