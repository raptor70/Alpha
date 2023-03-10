#ifndef __PROFILER_PIX_H__
#define __PROFILER_PIX_H__

#ifdef USE_PIX_WINDOWS
#define USE_PIX
#include INCLUDE_LIB(LIB_PIX,Include/WinPixEventRuntime/pix3.h)
#pragma LINK_LIB(LIB_PIX, Bin/WinPixEventRuntime.lib)

#define PROFILER_PIX_Begin(__COLOR__,__NAME__)	PIXBeginEvent(PIX_COLOR(ROUNDINT(__COLOR__.r*255.f),ROUNDINT(__COLOR__.g*255.f),ROUNDINT(__COLOR__.b*255.f)), __NAME__)
#define PROFILER_PIX_End()				PIXEndEvent()
#else
#define PROFILER_PIX_Begin(__COLOR__,__NAME__)
#define PROFILER_PIX_End()
#endif

#endif