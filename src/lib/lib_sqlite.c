#include "lib_include.h"

#define SQLITE_ENABLE_FTS4				1
#define SQLITE_ENABLE_RTREE				1
#define SQLITE_ENABLE_COLUMN_METADATA	1
#include INCLUDE_LIB(LIB_SQLITE, sqlite3.c)
