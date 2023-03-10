#ifndef _SHARED_TYPES_H_
#define _SHARED_TYPES_H_

#import <simd/simd.h>

#ifdef __cplusplus

typedef struct
{
	simd::float4x4 mvp;
	simd::float4   color;
	simd::float2   coorddelta;
	
} uniforms_t;

#endif

#endif