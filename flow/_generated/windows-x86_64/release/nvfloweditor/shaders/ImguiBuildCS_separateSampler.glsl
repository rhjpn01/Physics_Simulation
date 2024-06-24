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

struct ImguiRendererParams
{
	uint numVertices;
	uint numIndices;
	uint numDrawCmds;
	uint numBlocks;

	float width;
	float height;
	float widthInv;
	float heightInv;

	uint tileGridDim_x;
	uint tileGridDim_y;
	uint tileGridDim_xy;
	uint tileDimBits;

	uint maxTriangles;
	uint tileNumTrianglesOffset;
	uint tileLocalScanOffset;
	uint tileLocalTotalOffset;

	uint tileGlobalScanOffset;
	uint numTileBuckets;
	uint numTileBucketPasses;
	uint pad3;
};

struct ImguiRendererDrawCmd
{
	vec4 clipRect;
	uint elemCount;
	uint userTexture;
	uint vertexOffset;
	uint indexOffset;
};

layout(std140, binding = 0) uniform paramsIn_t
{
	ImguiRendererParams paramsIn;
};

layout(std430, binding = 1) readonly buffer vertexPosTexCoordIn_t
{
	vec4 vertexPosTexCoordIn[];
};
layout(std430, binding = 2) readonly buffer vertexColorIn_t
{
	uint vertexColorIn[];
};
layout(std430, binding = 3) readonly buffer indicesIn_t
{
	uint indicesIn[];
};
layout(std430, binding = 4) readonly buffer drawCmdsIn_t
{
	ImguiRendererDrawCmd drawCmdsIn[];
};

layout(std430, binding = 5) buffer treeOut_t
{
	ivec4 treeOut[];
};

shared ivec4 sdata0[256];
shared ivec4 sdata1[64];


ivec4 accumMinMax_0_1( ivec4 a,ivec4 b)
{
	if (b.x == b.z && b.y == b.w)
	{
		return a;
	}
	else
	{
		return ivec4(
			min(a.xy, b.xy),
			max(a.zw, b.zw)
		);
	}
}

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
void main(void)
{
	int tidx = int(gl_GlobalInvocationID.x);
	uint threadIdx = uint(tidx) & 255u;

	ivec4 minMaxPos = ivec4(0, 0, 0, 0);
	if ((3 * tidx) < int(paramsIn.numIndices))
	{
		uint indexOffset = 3u * uint(tidx);

		// TODO: lookup vertexOffset in drawCmd
		uint vertexOffset = 0u;

		uint idx0 = indicesIn[indexOffset + 0] + vertexOffset;
		uint idx1 = indicesIn[indexOffset + 1] + vertexOffset;
		uint idx2 = indicesIn[indexOffset + 2] + vertexOffset;

		vec2 pos0 = vertexPosTexCoordIn[idx0].xy;
		vec2 pos1 = vertexPosTexCoordIn[idx1].xy;
		vec2 pos2 = vertexPosTexCoordIn[idx2].xy;

		vec2 minPos = min(pos0, min(pos1, pos2));
		vec2 maxPos = max(pos0, max(pos1, pos2));

		minPos = floor(minPos);
		maxPos = -floor(-maxPos) + vec2(1.f, 1.f);

		minMaxPos = ivec4(minPos, maxPos);
	}

	uint treeBaseIdx = (1u + 4u + 16u + 64u + 256u) * (tidx >> 8u);

	sdata0[threadIdx] = minMaxPos;
	treeOut[treeBaseIdx + threadIdx + (1u + 4u + 16u + 64u)] = minMaxPos;

	groupMemoryBarrier(); barrier();

	if (threadIdx < 64u)
	{
		minMaxPos = sdata0[4u * threadIdx + 0u];
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata0[4u*threadIdx+1u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata0[4u*threadIdx+2u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata0[4u*threadIdx+3u]);

		sdata1[threadIdx] = minMaxPos;
		treeOut[treeBaseIdx + threadIdx + (1u + 4u + 16u)] = minMaxPos;
	}

	groupMemoryBarrier(); barrier();

	if (threadIdx < 16u)
	{
		minMaxPos = sdata1[4u * threadIdx + 0u];
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata1[4u*threadIdx+1u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata1[4u*threadIdx+2u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata1[4u*threadIdx+3u]);

		sdata0[threadIdx] = minMaxPos;
		treeOut[treeBaseIdx + threadIdx + (1u + 4u)] = minMaxPos;
	}

	groupMemoryBarrier(); barrier();

	if (threadIdx < 4u)
	{
		minMaxPos = sdata0[4u * threadIdx + 0u];
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata0[4u*threadIdx+1u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata0[4u*threadIdx+2u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata0[4u*threadIdx+3u]);

		sdata1[threadIdx] = minMaxPos;
		treeOut[treeBaseIdx + threadIdx + (1u)] = minMaxPos;
	}

	groupMemoryBarrier(); barrier();

	if (threadIdx < 1u)
	{
		minMaxPos = sdata1[4u * threadIdx + 0u];
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata1[4u*threadIdx+1u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata1[4u*threadIdx+2u]);
		minMaxPos = accumMinMax_0_1( minMaxPos,sdata1[4u*threadIdx+3u]);

		//sdata0[threadIdx] = minMaxPos;
		treeOut[treeBaseIdx + threadIdx + (0u)] = minMaxPos;
	}
}


