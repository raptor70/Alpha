#pragma once
#ifndef __RENDERER_SHADER_H__
#define __RENDERER_SHADER_H__


// Shaders
enum SHADERS
{
	RDRSHADER_None = -1,
#define SHADER(_name,_vfx,_pfx,_combinations)	SHADER_##_name,
#include "renderer_shader_list.h"
#undef SHADER

	RDRSHADER_Count
};

#endif