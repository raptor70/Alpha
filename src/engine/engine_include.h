#ifndef NO_PRECOMP
#define _CRT_SECURE_NO_WARNINGS

#ifdef IOS
#define USE_METAL
#endif

#ifndef WIN32
#define MASTER
#endif

#define USE_VULKAN

//#define USE_FORCE_POWER_OF_TWO_TEXTURES
#define USE_CREATE_DATA_CACHE

#ifndef MASTER
	#define USE_PROFILER
	#define USE_GPU_PROFILER
	//#define USE_PIX_WINDOWS
	#define USE_LOGGER
	#define USE_DRAWDEBUG
	//#define USE_EDITOR
	#define USE_EDITOR_V2
	//#define USE_DEBUGOPENGL
	#define USE_DEBUGVULKAN
#endif

#ifdef _DEBUG
	//#define USE_MEMORY
#endif

#ifdef USE_EDITOR_V2
	#define USE_IMGUI
#endif

#ifdef WIN32
	#define USE_STEAM
#endif

#ifdef USE_PROFILER
	#define USE_OPTICK 1
#endif

#ifdef USE_GPU_PROFILER
	#define USE_RENDERDOC
#endif

#define USE_MULTITHREAD_RENDERING

#include <lib_include.h>

#ifdef ANDROID
	#include <jni.h>
	#include <android/log.h>
	#include <android/asset_manager.h>
	#include <android/asset_manager_jni.h>
	#include <new>
	#include <time.h>
	#include <fstream>
	#include <sstream>
	#include <sys/stat.h>
	#include <errno.h>
	#include <gpg/android_support.h>
	#include <gpg/android_initialization.h>
	#include <gpg/achievement.h>
	#include <gpg/achievement_manager.h>
	#include <gpg/builder.h>
	#include <gpg/debug.h>
	#include <gpg/default_callbacks.h>
	#include <gpg/game_services.h>
	#include <gpg/leaderboard.h>
	#include <gpg/leaderboard_manager.h>
	#include <gpg/platform_configuration.h>
	#include <gpg/player_manager.h>
	#include <gpg/score_page.h>
	#include <gpg/types.h>
	#include <gpg/snapshot_manager.h>
	#include <gpg/snapshot_metadata_change_builder.h>
	#include "libzip-0.11.2/lib/zip.h"
#endif


#include <stdlib.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>
#include <functional>

#ifdef IOS
    #include <fstream>
    #include <sys/stat.h>
    #include <CoreFoundation/CoreFoundation.h>
    #include <Foundation/Foundation.h>
    #ifdef USE_METAL
	#include <Metal/Metal.h>
	#include <QuartzCore/CAMetalLayer.h>
    #endif
#endif

#ifdef USE_STEAM
	#include INCLUDE_LIB(LIB_STEAM,public/steam/steam_api.h)
	#pragma LINK_LIB(LIB_STEAM,redistributable_bin/win64/steam_api64.lib)
#endif

#ifdef USE_RENDERDOC
#include INCLUDE_LIB(LIB_RENDERDOC, renderdoc_app.h)
#define RENDERDOC_EXE "../src/lib/"STRINGIFY_MACRO(EXPAND(LIB_RENDERDOC))"/qrenderdoc.exe"
#endif

#define LOG_DIR "log/"
#define SAVE_DIR "save/"
#define SAVE_VERSION 5
#define SAVE_FILE "game.sav"
#define CACHE_DIR "cache/"
#define	CACHE_VERSION	10
#define TMP_DIR "tmp/"
#define SCRIPT_DOC_FILE "../docs/params.txt"

#include "core/primitive.h"
#include "debug/debug.h"
#include "memory/memory.h"
#include "core/hash_map_of_.h"
#include "core/str.h"
#include "core/ptr_to_.h"
#include "core/array_of_.h"
#include "core/list_of_.h"
#include "core/tree_of_.h"
#include "core/visitor.h"
#include "debug/logger.h"
#include "debug/debug_flag_list.h"

#define RAPIDXML_NO_STDLIB
//#define RAPIDXML_NO_EXCEPTIONS
#undef assert
#define assert(a)		DEBUG_Require(a);
#include INCLUDE_LIB(LIB_RAPIDXML, rapidxml.hpp)
#include INCLUDE_LIB(LIB_RAPIDXML, rapidxml_print.hpp)
#undef assert

#ifdef USE_IMGUI
#define IM_ASSERT(_EXPR)  do { DEBUG_Require(_EXPR) } while (0)
#define ImTextureID S32
#include INCLUDE_LIB(LIB_IMGUI, imgui.h)
#endif

#include "core/name.h"
#include "script/script.h"

#include "core/color.h"
#include "math/math_constant.h"
#include "math/math_helper.h"
#include "math/vector2.h"
#include "math/vector3.h"
#include "math/vector4.h"
#include "math/quaternion.h"
#include "math/segment.h"
#include "math/box.h"
#include "math/sphere.h"
#include "math/rect.h"
#include "math/plane.h"
#include "math/matrix2x2.h"
#include "math/matrix3x3.h"
#include "math/matrix4x4.h"
#include "math/interpolation.h"

#include "params/params_table.h"

#include "renderer/renderer.h"
#include "3d/material.h"
#include "debug/debug_renderer.h"

#include "world/world.h"

#include "imgui/imgui_ex.h"

#include "sound/sound_manager.h"

#include "debug/profiler.h"

#endif
