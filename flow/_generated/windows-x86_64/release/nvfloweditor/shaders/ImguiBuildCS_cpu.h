#define NV_FLOW_CPU_SHADER
#include "NvFlowMath.h"
#include "NvFlowResourceCPU.h"
#ifndef NV_FLOW_CPU_SHADER_DISABLE
namespace ImguiBuildCS
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













//struct ImguiRendererParams;
//struct ImguiRendererDrawCmd;

struct _Groupshared
{
	NvFlowCPU_GroupsharedArray<NvFlowCPU_Int4, 256> _sdata0;
	NvFlowCPU_GroupsharedArray<NvFlowCPU_Int4, 64> _sdata1;
}; 

struct _Globals
{
	NvFlowCPU_ConstantBuffer<ImguiRendererParams> _paramsIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Float4> _vertexPosTexCoordIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint> _vertexColorIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint> _indicesIn;
	NvFlowCPU_StructuredBuffer<ImguiRendererDrawCmd> _drawCmdsIn;
	NvFlowCPU_RWStructuredBuffer<NvFlowCPU_Int4> _treeOut;
}; 

NV_FLOW_FORCE_INLINE NvFlowCPU_Int4 accumMinMax_0_1(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Int4 a,NvFlowCPU_Int4 b)
{
	if (b.x == b.z && b.y == b.w)
	{
		return a;
	}
	else
	{
		return NvFlowCPU_Int4(
			NvFlowCPU_min(a.rg(), b.rg()),
			NvFlowCPU_max(a.ba(), b.ba())
		);
	}
}


NV_FLOW_FORCE_INLINE void _main(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Uint3 cpu_DispatchThreadID, NvFlowCPU_Uint3 cpu_GroupID, NvFlowCPU_Uint3 cpu_GroupThreadID)
{
	int tidx = int(cpu_DispatchThreadID.x);
	NvFlowCPU_Uint threadIdx = NvFlowCPU_Uint(tidx) & 255u;

	NvFlowCPU_Int4 minMaxPos = NvFlowCPU_Int4(0, 0, 0, 0);
	if ((3 * tidx) < int(_globals->_paramsIn.data->numIndices))
	{
		NvFlowCPU_Uint indexOffset = 3u * NvFlowCPU_Uint(tidx);

		// TODO: lookup vertexOffset in drawCmd
		NvFlowCPU_Uint vertexOffset = 0u;

		NvFlowCPU_Uint idx0 = _globals->_indicesIn[indexOffset + 0] + vertexOffset;
		NvFlowCPU_Uint idx1 = _globals->_indicesIn[indexOffset + 1] + vertexOffset;
		NvFlowCPU_Uint idx2 = _globals->_indicesIn[indexOffset + 2] + vertexOffset;

		NvFlowCPU_Float2 pos0 = _globals->_vertexPosTexCoordIn[idx0].rg();
		NvFlowCPU_Float2 pos1 = _globals->_vertexPosTexCoordIn[idx1].rg();
		NvFlowCPU_Float2 pos2 = _globals->_vertexPosTexCoordIn[idx2].rg();

		NvFlowCPU_Float2 minPos = NvFlowCPU_min(pos0, NvFlowCPU_min(pos1, pos2));
		NvFlowCPU_Float2 maxPos = NvFlowCPU_max(pos0, NvFlowCPU_max(pos1, pos2));

		minPos = NvFlowCPU_floor(minPos);
		maxPos = -NvFlowCPU_floor(-maxPos) + NvFlowCPU_Float2(1.f, 1.f);

		minMaxPos = NvFlowCPU_Int4(minPos, maxPos);
	}

	NvFlowCPU_Uint treeBaseIdx = (1u + 4u + 16u + 64u + 256u) * (tidx >> 8u);

	NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata0, threadIdx, (minMaxPos));
	if (_groupshared_pass == _groupshared_sync_count) {_globals->_treeOut[treeBaseIdx + threadIdx + (1u + 4u + 16u + 64u)] = minMaxPos;}

	_groupshared_sync_count++;

	if (threadIdx < 64u)
	{
		minMaxPos = NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u * threadIdx + 0u);
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u*threadIdx+1u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u*threadIdx+2u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u*threadIdx+3u));

		NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata1, threadIdx, (minMaxPos));
		if (_groupshared_pass == _groupshared_sync_count) {_globals->_treeOut[treeBaseIdx + threadIdx + (1u + 4u + 16u)] = minMaxPos;}
	}

	_groupshared_sync_count++;

	if (threadIdx < 16u)
	{
		minMaxPos = NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u * threadIdx + 0u);
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u*threadIdx+1u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u*threadIdx+2u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u*threadIdx+3u));

		NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata0, threadIdx, (minMaxPos));
		if (_groupshared_pass == _groupshared_sync_count) {_globals->_treeOut[treeBaseIdx + threadIdx + (1u + 4u)] = minMaxPos;}
	}

	_groupshared_sync_count++;

	if (threadIdx < 4u)
	{
		minMaxPos = NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u * threadIdx + 0u);
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u*threadIdx+1u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u*threadIdx+2u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, 4u*threadIdx+3u));

		NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata1, threadIdx, (minMaxPos));
		if (_groupshared_pass == _groupshared_sync_count) {_globals->_treeOut[treeBaseIdx + threadIdx + (1u)] = minMaxPos;}
	}

	_groupshared_sync_count++;

	if (threadIdx < 1u)
	{
		minMaxPos = NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u * threadIdx + 0u);
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u*threadIdx+1u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u*threadIdx+2u));
		minMaxPos = accumMinMax_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  minMaxPos,NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 4u*threadIdx+3u));

		//sdata0[threadIdx] = minMaxPos;
		if (_groupshared_pass == _groupshared_sync_count) {_globals->_treeOut[treeBaseIdx + threadIdx + (0u)] = minMaxPos;}
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
	globals._vertexPosTexCoordIn.bind(globals_bindings_src[1]);
	globals._vertexColorIn.bind(globals_bindings_src[2]);
	globals._indicesIn.bind(globals_bindings_src[3]);
	globals._drawCmdsIn.bind(globals_bindings_src[4]);
	globals._treeOut.bind(globals_bindings_src[5]);
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


} // namespace ImguiBuildCS end
void* ImguiBuildCS_cpu = (void*)ImguiBuildCS::_mainBlock;
#else
void* ImguiBuildCS_cpu = 0;
#endif
#undef NV_FLOW_CPU_SHADER


