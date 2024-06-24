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

layout(binding = 5) uniform texture2D textureIn;
layout(binding = 6) uniform sampler samplerIn;

layout(std430, binding = 7) readonly buffer triangleIn_t
{
	uint triangleIn[];
};
layout(std430, binding = 8) readonly buffer triangleRangeIn_t
{
	uvec2 triangleRangeIn[];
};

layout(binding = 9) uniform texture2D colorIn;

layout(binding = 10) uniform writeonly image2D colorOut;


vec4 convertColor_0( uint val)
{
	return vec4(
		((val >>  0) & 255) * (1.f / 255.f),
		((val >>  8) & 255) * (1.f / 255.f),
		((val >> 16) & 255) * (1.f / 255.f),
		((val >> 24) & 255) * (1.f / 255.f)
	);
}

vec3 computeBary_0_1_2_3( vec2 p1,vec2 p2,vec2 p3,vec2 p)
{
	float det = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
	float b1 = (p2.y - p3.y) * (p.x - p3.x) + (p3.x - p2.x) * (p.y - p3.y);
	float b2 = (p3.y - p1.y) * (p.x - p3.x) + (p1.x - p3.x) * (p.y - p3.y);
	b1 = b1 / det;
	b2 = b2 / det;
	float b3 = 1.f - b1 - b2;
	return vec3(b1, b2, b3);
}

bool edgeTest_0_1_2_3( vec2 edgeA_fp,vec2 edgeB_fp,vec2 inside_fp,vec2 pt_fp)
{
	ivec2 edgeA = ivec2(edgeA_fp);
	ivec2 edgeB = ivec2(edgeB_fp);
	ivec2 inside = ivec2(inside_fp);
	ivec2 pt = ivec2(pt_fp);

	ivec2 m = edgeB - edgeA;
	bool isBelow;
	if (abs(m.y) > abs(m.x))
	{
		int insX_num = (edgeB.y - inside.y) * edgeA.x - (edgeA.y - inside.y) * edgeB.x;
		int insX = int(float(insX_num) / float(m.y));
		bool insideIsBelow = inside.x < insX;

		int cmpX_num = (edgeB.y - pt.y) * edgeA.x - (edgeA.y - pt.y) * edgeB.x;
		int cmpX = int(float(cmpX_num) / float(m.y));
		isBelow = insideIsBelow ? pt.x < cmpX : pt.x >= cmpX;
	}
	else
	{
		int insY_num = (edgeB.x - inside.x) * edgeA.y - (edgeA.x - inside.x) * edgeB.y;
		int insY = int(float(insY_num) / float(m.x));
		bool insideIsBelow = inside.y < insY;

		int cmpY_num = (edgeB.x - pt.x) * edgeA.y - (edgeA.x - pt.x) * edgeB.y;
		int cmpY = int(float(cmpY_num) / float(m.x));
		isBelow = insideIsBelow ? pt.y < cmpY : pt.y >= cmpY;
	}
	return isBelow;
}

vec4 leaf_0_1( vec2 p,uint triangleIdx)
{
	uint indexOffsetLocal = 3u * triangleIdx;

	uint idx0 = indicesIn[indexOffsetLocal + 0];
	uint idx1 = indicesIn[indexOffsetLocal + 1];
	uint idx2 = indicesIn[indexOffsetLocal + 2];

	vec4 pos0 = vertexPosTexCoordIn[idx0];
	vec4 pos1 = vertexPosTexCoordIn[idx1];
	vec4 pos2 = vertexPosTexCoordIn[idx2];

	vec2 p1 = 16.f * pos0.xy;
	vec2 p2 = 16.f * pos1.xy;
	vec2 p3 = 16.f * pos2.xy;

	bool passed = edgeTest_0_1_2_3( p1,p2,p3,p);
	passed = passed && edgeTest_0_1_2_3( p2,p3,p1,p); 
	passed = passed && edgeTest_0_1_2_3( p3,p1,p2,p);

	vec4 c = vec4(0.f, 0.f, 0.f, 0.f);
	if (passed)
	{
		vec3 b = computeBary_0_1_2_3( p1,p2,p3,p);

		c = b.x * convertColor_0( vertexColorIn[idx0]) +
			b.y * convertColor_0( vertexColorIn[idx1]) +
			b.z * convertColor_0( vertexColorIn[idx2]);

		vec2 uv = b.x * pos0.zw + b.y * pos1.zw + b.z * pos2.zw;

		c *= textureLod(sampler2D(textureIn, samplerIn), uv, 0.0).xyzw;
	}
	return c;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main(void)
{
	ivec2 tidx = ivec2(gl_GlobalInvocationID.xy);

	vec4 color = texelFetch(colorIn, tidx, 0).xyzw;

	ivec2 tileIdx = ivec2(
		tidx.x >> paramsIn.tileDimBits,
		tidx.y >> paramsIn.tileDimBits
	);
	int tileIdx1D = int(tileIdx.y * paramsIn.tileGridDim_x + tileIdx.x);

	if (tileIdx.y >= int(paramsIn.tileGridDim_y))
	{
		color.g = 1.f;
	}

	uvec2 triangleListRange = triangleRangeIn[tileIdx1D];

	vec2 tidxf = vec2(tidx) + vec2(0.5f, 0.5f);
	vec2 p = 16.f * tidxf;

	for (uint listIdx = 0u; listIdx < triangleListRange.y; listIdx++)
	{
		uint triangleIdxRaw = triangleIn[triangleListRange.x + listIdx];
		uint triangleIdx = triangleIdxRaw & 0x00FFFFFF;
		uint drawCmdIdx = triangleIdxRaw >> 24u;

		vec4 c = leaf_0_1( p,triangleIdx);

		//color = (1.f - c.w) * color + c.w * c;

		vec4 clipRect = drawCmdsIn[drawCmdIdx].clipRect;
		// (x1, y1, x2, y2)
		if (tidx.x >= clipRect.x && tidx.y >= clipRect.y &&
			tidx.x < clipRect.z && tidx.y < clipRect.w)
		{
			color = (1.f - c.w) * color + c.w * c;
		}
	}

	imageStore(colorOut, tidx, (color).xyzw);
}



