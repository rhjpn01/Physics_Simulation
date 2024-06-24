#define NV_FLOW_CPU_SHADER
#include "NvFlowMath.h"
#include "NvFlowResourceCPU.h"
#ifndef NV_FLOW_CPU_SHADER_DISABLE
namespace ImguiTileCS
{
using namespace NvFlowMath;


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
	NvFlowCPU_Uint numVertices;
	NvFlowCPU_Uint numIndices;
	NvFlowCPU_Uint numDrawCmds;
	NvFlowCPU_Uint numBlocks;

	float width;
	float height;
	float widthInv;
	float heightInv;

	NvFlowCPU_Uint tileGridDim_x;
	NvFlowCPU_Uint tileGridDim_y;
	NvFlowCPU_Uint tileGridDim_xy;
	NvFlowCPU_Uint tileDimBits;

	NvFlowCPU_Uint maxTriangles;
	NvFlowCPU_Uint tileNumTrianglesOffset;
	NvFlowCPU_Uint tileLocalScanOffset;
	NvFlowCPU_Uint tileLocalTotalOffset;

	NvFlowCPU_Uint tileGlobalScanOffset;
	NvFlowCPU_Uint numTileBuckets;
	NvFlowCPU_Uint numTileBucketPasses;
	NvFlowCPU_Uint pad3;
};

struct ImguiRendererDrawCmd
{
	NvFlowCPU_Float4 clipRect;
	NvFlowCPU_Uint elemCount;
	NvFlowCPU_Uint userTexture;
	NvFlowCPU_Uint vertexOffset;
	NvFlowCPU_Uint indexOffset;
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






//struct ImguiRendererParams;
//struct ImguiRendererDrawCmd;

struct _Groupshared
{
	NvFlowCPU_GroupsharedArray<NvFlowCPU_Uint, 256u> _sdata0;
	NvFlowCPU_GroupsharedArray<NvFlowCPU_Uint, 256u> _sdata1;
	NvFlowCPU_Groupshared<NvFlowCPU_Uint> _stotalCount;
}; 

struct _Globals
{
	NvFlowCPU_ConstantBuffer<ImguiRendererParams> _paramsIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Int4> _treeIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint> _tileCountIn;
	NvFlowCPU_StructuredBuffer<ImguiRendererDrawCmd> _drawCmdsIn;
	NvFlowCPU_RWStructuredBuffer<NvFlowCPU_Uint> _triangleOut;
	NvFlowCPU_RWStructuredBuffer<NvFlowCPU_Uint2> _triangleRangeOut;
}; 

NV_FLOW_FORCE_INLINE bool overlapTest_0_1(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Int4 minMaxA,NvFlowCPU_Int4 minMaxB)
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

NV_FLOW_FORCE_INLINE void writeTriangle_0_1_2(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Uint &hitIdx,NvFlowCPU_Uint triangleWriteOffset,NvFlowCPU_Uint triangleIdx)
{
	NvFlowCPU_Uint index = 3u * triangleIdx;
	NvFlowCPU_Uint drawCmdIdx = _globals->_paramsIn.data->numDrawCmds - 1u;
	for (; drawCmdIdx < _globals->_paramsIn.data->numDrawCmds; drawCmdIdx--)
	{
		if (index >= _globals->_drawCmdsIn[drawCmdIdx].indexOffset)
		{
			break;
		}
	}

	if (_groupshared_pass == _groupshared_sync_count) {_globals->_triangleOut[hitIdx + triangleWriteOffset] = triangleIdx | (drawCmdIdx << 24u);}
	hitIdx++;
}

NV_FLOW_FORCE_INLINE void countHits_0_1_2_3(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Uint &hitIdx,NvFlowCPU_Int4 idxMinMax,NvFlowCPU_Uint blockIdx,NvFlowCPU_Uint triangleWriteOffset)
{
	NvFlowCPU_Uint treeBaseOffset = (1u + 4u + 16u + 64u + 256u) * blockIdx;

	NvFlowCPU_Int4 minMaxPos = _globals->_treeIn[treeBaseOffset];

	if (overlapTest_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  idxMinMax,minMaxPos))
	{
		NvFlowCPU_Uint treeletOffset0 = treeBaseOffset + (1u);
		NvFlowCPU_Uint treeletOffset1 = treeBaseOffset + (1u + 4u);
		NvFlowCPU_Uint treeletOffset2 = treeBaseOffset + (1u + 4u + 16u);
		NvFlowCPU_Uint treeletOffset3 = treeBaseOffset + (1u + 4u + 16u + 64u);
		for (NvFlowCPU_Uint childIdx0 = 0u; childIdx0 < 4u; childIdx0++)
		{
			NvFlowCPU_Uint idx0 = childIdx0;
			NvFlowCPU_Int4 minMaxPos = _globals->_treeIn[idx0 + treeletOffset0];
			if (overlapTest_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  idxMinMax,minMaxPos))
			{
				for (NvFlowCPU_Uint childIdx1 = 0u; childIdx1 < 4u; childIdx1++)
				{
					NvFlowCPU_Uint idx1 = 4u * idx0 + childIdx1;
					NvFlowCPU_Int4 minMaxPos = _globals->_treeIn[idx1 + treeletOffset1];
					if (overlapTest_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  idxMinMax,minMaxPos))
					{
						for (NvFlowCPU_Uint childIdx2 = 0u; childIdx2 < 4u; childIdx2++)
						{
							NvFlowCPU_Uint idx2 = 4u * idx1 + childIdx2;
							NvFlowCPU_Int4 minMaxPos = _globals->_treeIn[idx2 + treeletOffset2];
							if (overlapTest_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  idxMinMax,minMaxPos))
							{
								for (NvFlowCPU_Uint childIdx3 = 0u; childIdx3 < 4u; childIdx3++)
								{
									NvFlowCPU_Uint idx3 = 4u * idx2 + childIdx3;
									NvFlowCPU_Int4 minMaxPos = _globals->_treeIn[idx3 + treeletOffset3];
									if (overlapTest_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  idxMinMax,minMaxPos))
									{
										NvFlowCPU_Uint triangleIdx = (blockIdx << 8u) + idx3;

										writeTriangle_0_1_2(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  hitIdx,triangleWriteOffset,triangleIdx);
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


NV_FLOW_FORCE_INLINE void _main(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Uint3 cpu_DispatchThreadID, NvFlowCPU_Uint3 cpu_GroupID, NvFlowCPU_Uint3 cpu_GroupThreadID)
{
	int tidx = int(cpu_DispatchThreadID.x);
	NvFlowCPU_Uint threadIdx = NvFlowCPU_Uint(tidx) & 255u;

	NvFlowCPU_Int2 tileIdx = NvFlowCPU_Int2(
		tidx % _globals->_paramsIn.data->tileGridDim_x,
		tidx / _globals->_paramsIn.data->tileGridDim_x
	);

	if (tileIdx.y < int(_globals->_paramsIn.data->tileGridDim_y))
	{
		NvFlowCPU_Uint hitIdx = 0u;

		// add local and global offsets together
		NvFlowCPU_Uint globalOffset = _globals->_tileCountIn[(tidx >> 8u) + _globals->_paramsIn.data->tileGlobalScanOffset];
		NvFlowCPU_Uint localOffset = _globals->_tileCountIn[tidx + _globals->_paramsIn.data->tileLocalScanOffset];
		NvFlowCPU_Uint triangleWriteOffset = globalOffset + localOffset;

		NvFlowCPU_Uint tileDim = 1u << _globals->_paramsIn.data->tileDimBits;

		NvFlowCPU_Int4 idxMinMax = NvFlowCPU_Int4(
			tileIdx * tileDim - NvFlowCPU_Int2(1, 1),
			tileIdx * tileDim + tileDim - NvFlowCPU_Int2(1, 1)
		);

		for (NvFlowCPU_Uint blockIdx = 0u; blockIdx < _globals->_paramsIn.data->numBlocks; blockIdx++)
		{
			countHits_0_1_2_3(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  hitIdx,idxMinMax,blockIdx,triangleWriteOffset);
		}

		// write out range
		if (_groupshared_pass == _groupshared_sync_count) {_globals->_triangleRangeOut[tidx] = NvFlowCPU_Uint2(triangleWriteOffset, hitIdx);}
	}
}

void _mainBlock(void* smemPool, NvFlowCPU_Uint3 groupID, NvFlowCPU_Uint numDescriptorWrites, const NvFlowDescriptorWrite* descriptorWrites, NvFlowCPU_Resource** resources)
{
	_Globals globals = {};
	NvFlowCPU_Resource* globals_bindings_src[6] = {};
	for(NvFlowCPU_Uint descriptorIdx = 0u; descriptorIdx < numDescriptorWrites; descriptorIdx++)
	{
		globals_bindings_src[descriptorWrites[descriptorIdx].write.vulkan.binding] = resources[descriptorIdx];
	};
	globals._paramsIn.bind(globals_bindings_src[0]);
	globals._treeIn.bind(globals_bindings_src[1]);
	globals._tileCountIn.bind(globals_bindings_src[2]);
	globals._drawCmdsIn.bind(globals_bindings_src[3]);
	globals._triangleOut.bind(globals_bindings_src[4]);
	globals._triangleRangeOut.bind(globals_bindings_src[5]);
	_Groupshared* smem = (_Groupshared*)smemPool;
	int groupshared_sync_count = 0;
	for(int groupshared_pass = 0; groupshared_pass <= groupshared_sync_count;)
	{
		if (groupshared_pass)
		{
			smem[groupshared_pass] = smem[groupshared_pass - 1];
		}
		for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
		{
			for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (1); threadIdx_j++)
			{
				for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (256); threadIdx_i++)
				{
					NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (256) + threadIdx_i, groupID.y * (1) + threadIdx_j, groupID.z * (1) + threadIdx_k);
					NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
					groupshared_sync_count = 0;
					_main(&globals, smem, groupshared_pass, groupshared_sync_count, cpu_DispatchThreadID, groupID, groupThreadID);
				}
			}
		}
		break;
	}
	for(int groupshared_pass = 1; groupshared_pass <= groupshared_sync_count;)
	{
		if (groupshared_pass)
		{
			smem[groupshared_pass] = smem[groupshared_pass - 1];
		}
		for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
		{
			for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (1); threadIdx_j++)
			{
				for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (256); threadIdx_i++)
				{
					NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (256) + threadIdx_i, groupID.y * (1) + threadIdx_j, groupID.z * (1) + threadIdx_k);
					NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
					groupshared_sync_count = 0;
					_main(&globals, smem, groupshared_pass, groupshared_sync_count, cpu_DispatchThreadID, groupID, groupThreadID);
				}
			}
		}
		break;
	}
	for(int groupshared_pass = 2; groupshared_pass <= groupshared_sync_count;)
	{
		if (groupshared_pass)
		{
			smem[groupshared_pass] = smem[groupshared_pass - 1];
		}
		for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
		{
			for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (1); threadIdx_j++)
			{
				for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (256); threadIdx_i++)
				{
					NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (256) + threadIdx_i, groupID.y * (1) + threadIdx_j, groupID.z * (1) + threadIdx_k);
					NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
					groupshared_sync_count = 0;
					_main(&globals, smem, groupshared_pass, groupshared_sync_count, cpu_DispatchThreadID, groupID, groupThreadID);
				}
			}
		}
		break;
	}
	for(int groupshared_pass = 3; groupshared_pass <= groupshared_sync_count;)
	{
		if (groupshared_pass)
		{
			smem[groupshared_pass] = smem[groupshared_pass - 1];
		}
		for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
		{
			for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (1); threadIdx_j++)
			{
				for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (256); threadIdx_i++)
				{
					NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (256) + threadIdx_i, groupID.y * (1) + threadIdx_j, groupID.z * (1) + threadIdx_k);
					NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
					groupshared_sync_count = 0;
					_main(&globals, smem, groupshared_pass, groupshared_sync_count, cpu_DispatchThreadID, groupID, groupThreadID);
				}
			}
		}
		break;
	}
	for(int groupshared_pass = 4; groupshared_pass <= groupshared_sync_count;)
	{
		if (groupshared_pass)
		{
			smem[groupshared_pass] = smem[groupshared_pass - 1];
		}
		for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
		{
			for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (1); threadIdx_j++)
			{
				for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (256); threadIdx_i++)
				{
					NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (256) + threadIdx_i, groupID.y * (1) + threadIdx_j, groupID.z * (1) + threadIdx_k);
					NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
					groupshared_sync_count = 0;
					_main(&globals, smem, groupshared_pass, groupshared_sync_count, cpu_DispatchThreadID, groupID, groupThreadID);
				}
			}
		}
		break;
	}
	for(int groupshared_pass = 5; groupshared_pass <= groupshared_sync_count; groupshared_pass++)
	{
		if (groupshared_pass)
		{
			smem[groupshared_pass] = smem[groupshared_pass - 1];
		}
		for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
		{
			for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (1); threadIdx_j++)
			{
				for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (256); threadIdx_i++)
				{
					NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (256) + threadIdx_i, groupID.y * (1) + threadIdx_j, groupID.z * (1) + threadIdx_k);
					NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
					groupshared_sync_count = 0;
					_main(&globals, smem, groupshared_pass, groupshared_sync_count, cpu_DispatchThreadID, groupID, groupThreadID);
				}
			}
		}
	}
}


} // namespace ImguiTileCS end
void* ImguiTileCS_cpu = (void*)ImguiTileCS::_mainBlock;
#else
void* ImguiTileCS_cpu = 0;
#endif
#undef NV_FLOW_CPU_SHADER


