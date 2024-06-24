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

layout(std430, binding = 1) readonly buffer treeIn_t
{
	ivec4 treeIn[];
};
layout(std430, binding = 2) readonly buffer tileCountIn_t
{
	uint tileCountIn[];
};
layout(std430, binding = 3) readonly buffer drawCmdsIn_t
{
	ImguiRendererDrawCmd drawCmdsIn[];
};

layout(std430, binding = 4) buffer triangleOut_t
{
	uint triangleOut[];
};
layout(std430, binding = 5) buffer triangleRangeOut_t
{
	uvec2 triangleRangeOut[];
};

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

shared uint sdata0[256u];
shared uint sdata1[256u];

shared uint stotalCount;


bool overlapTest_0_1( ivec4 minMaxA,ivec4 minMaxB)
{
	bool ret;
	if (minMaxB.x == minMaxB.z &&
		minMaxB.y == minMaxB.w)
	{
		ret = false;
	}
	else
	{
		ret = !(
			minMaxA.x > minMaxB.z || minMaxB.x > minMaxA.z ||
			minMaxA.y > minMaxB.w || minMaxB.y > minMaxA.w
			);
	}
	return ret;
}

void writeTriangle_0_1_2( inout uint hitIdx,uint triangleWriteOffset,uint triangleIdx)
{
	uint index = 3u * triangleIdx;
	uint drawCmdIdx = paramsIn.numDrawCmds - 1u;
	for (; drawCmdIdx < paramsIn.numDrawCmds; drawCmdIdx--)
	{
		if (index >= drawCmdsIn[drawCmdIdx].indexOffset)
		{
			break;
		}
	}

	triangleOut[hitIdx + triangleWriteOffset] = triangleIdx | (drawCmdIdx << 24u);
	hitIdx++;
}

void countHits_0_1_2_3( inout uint hitIdx,ivec4 idxMinMax,uint blockIdx,uint triangleWriteOffset)
{
	uint treeBaseOffset = (1u + 4u + 16u + 64u + 256u) * blockIdx;

	ivec4 minMaxPos = treeIn[treeBaseOffset];

	if (overlapTest_0_1( idxMinMax,minMaxPos))
	{
		uint treeletOffset0 = treeBaseOffset + (1u);
		uint treeletOffset1 = treeBaseOffset + (1u + 4u);
		uint treeletOffset2 = treeBaseOffset + (1u + 4u + 16u);
		uint treeletOffset3 = treeBaseOffset + (1u + 4u + 16u + 64u);
		for (uint childIdx0 = 0u; childIdx0 < 4u; childIdx0++)
		{
			uint idx0 = childIdx0;
			ivec4 minMaxPos = treeIn[idx0 + treeletOffset0];
			if (overlapTest_0_1( idxMinMax,minMaxPos))
			{
				for (uint childIdx1 = 0u; childIdx1 < 4u; childIdx1++)
				{
					uint idx1 = 4u * idx0 + childIdx1;
					ivec4 minMaxPos = treeIn[idx1 + treeletOffset1];
					if (overlapTest_0_1( idxMinMax,minMaxPos))
					{
						for (uint childIdx2 = 0u; childIdx2 < 4u; childIdx2++)
						{
							uint idx2 = 4u * idx1 + childIdx2;
							ivec4 minMaxPos = treeIn[idx2 + treeletOffset2];
							if (overlapTest_0_1( idxMinMax,minMaxPos))
							{
								for (uint childIdx3 = 0u; childIdx3 < 4u; childIdx3++)
								{
									uint idx3 = 4u * idx2 + childIdx3;
									ivec4 minMaxPos = treeIn[idx3 + treeletOffset3];
									if (overlapTest_0_1( idxMinMax,minMaxPos))
									{
										uint triangleIdx = (blockIdx << 8u) + idx3;

										writeTriangle_0_1_2( hitIdx,triangleWriteOffset,triangleIdx);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
void main(void)
{
	int tidx = int(gl_GlobalInvocationID.x);
	uint threadIdx = uint(tidx) & 255u;

	ivec2 tileIdx = ivec2(
		tidx % paramsIn.tileGridDim_x,
		tidx / paramsIn.tileGridDim_x
	);

	if (tileIdx.y < int(paramsIn.tileGridDim_y))
	{
		uint hitIdx = 0u;

		// add local and global offsets together
		uint globalOffset = tileCountIn[(tidx >> 8u) + paramsIn.tileGlobalScanOffset];
		uint localOffset = tileCountIn[tidx + paramsIn.tileLocalScanOffset];
		uint triangleWriteOffset = globalOffset + localOffset;

		uint tileDim = 1u << paramsIn.tileDimBits;

		ivec4 idxMinMax = ivec4(
			tileIdx * tileDim - ivec2(1, 1),
			tileIdx * tileDim + tileDim - ivec2(1, 1)
		);

		for (uint blockIdx = 0u; blockIdx < paramsIn.numBlocks; blockIdx++)
		{
			countHits_0_1_2_3( hitIdx,idxMinMax,blockIdx,triangleWriteOffset);
		}

		// write out range
		triangleRangeOut[tidx] = uvec2(triangleWriteOffset, hitIdx);
	}
}


