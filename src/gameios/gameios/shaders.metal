#include <metal_stdlib>
#include <simd/simd.h>
#include "ShaderTypes.h"

using namespace metal;


typedef struct
{
	packed_float3 position;
	packed_float2 coord;
	packed_float3 normal;
    packed_float4 color;
} vertex_t;

struct ColorInOut {
    float4 position [[position]];
    float4 color;
	float2 coord;
};

// vertex shader function
vertex ColorInOut lighting_vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                  constant uniforms_t& uniforms [[ buffer(1) ]],
                                  unsigned int vid [[ vertex_id ]])
{
	ColorInOut out;
	float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
	out.position = uniforms.mvp * in_position;
    out.position.z = 0;
    out.color = uniforms.color * float4(vertex_array[vid].color);
	out.coord = float2(vertex_array[vid].coord) + uniforms.coorddelta;
    
    return out;
}

/*fragment half4 lighting_fragment(ColorInOut in [[stage_in]])
{
    return half4(1.0,0.0,0.0,1.0);//in.color;
};*/

// fragment shader function
fragment float4 lighting_fragment(ColorInOut in [[stage_in]]
                                    ,texture2d<float>  tex2D     [[ texture(0) ]]
                                  , sampler s [[ sampler(0) ]])
{
    //return half4(1.0,0.0,0.0,1.0);
	//return float4(1.0f,0.0f,0.0f,1.0f);
	//constexpr sampler quad_sampler;
    return float4(in.color * tex2D.sample(s, in.coord));

};

