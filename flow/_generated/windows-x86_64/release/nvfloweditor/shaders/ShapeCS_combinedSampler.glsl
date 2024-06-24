#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_samplerless_texture_functions : enable
double asdouble(uint x, uint y) { return packDouble2x32(uvec2(x, y)); }
uint f32tof16(float value) { return packHalf2x16(vec2(value, 0)); }
float f16tof32(uint value) { return unpackHalf2x16(value).x; }

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2014-2022 NVIDIA Corporation. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2014-2022 NVIDIA Corporation. All rights reserved.

struct ShapeRendererParams
{
	mat4 projection;
	mat4 view;
	mat4 projectionInv;
	mat4 viewInv;

	vec4 rayDir00;
	vec4 rayDir10;
	vec4 rayDir01;
	vec4 rayDir11;

	vec4 rayOrigin00;
	vec4 rayOrigin10;
	vec4 rayOrigin01;
	vec4 rayOrigin11;

	float width;
	float height;
	float widthInv;
	float heightInv;

	uint numSpheres;
	float clearDepth;
	uint isReverseZ;
	uint pad3;

	vec4 clearColor;
};

layout(std140, binding = 0) uniform paramsIn_t
{
	ShapeRendererParams paramsIn;
};

layout(std430, binding = 1) readonly buffer spherePositionRadiusIn_t
{
	vec4 spherePositionRadiusIn[];
};

layout(binding = 2) uniform writeonly image2D depthOut;
layout(binding = 3) uniform writeonly image2D colorOut;


bool raySphereIntersection_0_1_2_3_4( vec3 o,vec3 d,vec3 c,float r,inout float t)
{
	bool ret = false;
	vec3 l = c - o;
	float s = dot(l, d);
	float l2 = dot(l, l);
	float r2 = r * r;
	if (s >= 0.f || l2 <= r2)
	{
		float s2 = s * s;
		float m2 = l2 - s2;
		if (m2 <= r2)
		{
			float q = sqrt(r2 - m2);
			if (l2 > r2)
			{
				t = s - q;
			}
			else
			{
				t = s + q;
			}
			ret = t > 0.f;
		}
	}
	return ret;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main(void)
{
	ivec2 tidx = ivec2(gl_GlobalInvocationID.xy);

	vec2 pixelCenter = vec2(tidx) + 0.5f;
	vec2 inUV = pixelCenter * vec2(paramsIn.widthInv, paramsIn.heightInv);

	float w00 = (1.f - inUV.x) * (1.f - inUV.y);
	float w10 = (inUV.x) * (1.f - inUV.y);
	float w01 = (1.f - inUV.x) * (inUV.y);
	float w11 = (inUV.x) * (inUV.y);

	vec3 rayDir = normalize(w00 * paramsIn.rayDir00.xyz + w10 * paramsIn.rayDir10.xyz + w01 * paramsIn.rayDir01.xyz + w11 * paramsIn.rayDir11.xyz);
	vec3 rayOrigin = w00 * paramsIn.rayOrigin00.xyz + w10 * paramsIn.rayOrigin10.xyz + w01 * paramsIn.rayOrigin01.xyz + w11 * paramsIn.rayOrigin11.xyz;

	float globalDepth = paramsIn.clearDepth;
	vec4 globalColor = paramsIn.clearColor;

	for (uint sphereIdx = 0u; sphereIdx < paramsIn.numSpheres; sphereIdx++)
	{
		vec4 spherePositionRadius = spherePositionRadiusIn[sphereIdx];
		float hitT = -1.f;
		if (raySphereIntersection_0_1_2_3_4( rayOrigin,rayDir,spherePositionRadius.xyz,spherePositionRadius.w,hitT))
		{
			vec4 worldPos = vec4(rayDir * hitT + rayOrigin, 1.f);

			vec4 clipPos = (worldPos * paramsIn.view);
			clipPos = (clipPos * paramsIn.projection);
			
			float depth = clipPos.z / clipPos.w;

			bool isVisible = bool(paramsIn.isReverseZ) ? (depth > globalDepth) : (depth < globalDepth);
			if (isVisible)
			{
				vec3 n = normalize(worldPos.xyz - spherePositionRadius.xyz);

				vec4 color = vec4(abs(n), 1.f);

				globalDepth = depth;
				globalColor = color;
			}
		}
	}

	imageStore(depthOut, tidx, (globalDepth).xxxx);
	imageStore(colorOut, tidx, (globalColor).xyzw);
}



