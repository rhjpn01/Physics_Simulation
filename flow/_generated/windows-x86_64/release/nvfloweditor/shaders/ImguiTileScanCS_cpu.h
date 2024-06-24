#define NV_FLOW_CPU_SHADER
#include "NvFlowMath.h"
#include "NvFlowResourceCPU.h"
#ifndef NV_FLOW_CPU_SHADER_DISABLE
namespace ImguiTileScanCS
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
	NvFlowCPU_RWStructuredBuffer<NvFlowCPU_Uint> _tileCountOut;
	NvFlowCPU_RWStructuredBuffer<NvFlowCPU_Uint> _totalCountOut;
}; 

NV_FLOW_FORCE_INLINE NvFlowCPU_Uint blockScan_0_1(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Uint threadIdx,NvFlowCPU_Uint val)
{
	NvFlowCPU_Uint localVal = val;
	NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata0, threadIdx, (localVal));

	_groupshared_sync_count++;

	if (threadIdx >= 1) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, threadIdx - 1);
	if (threadIdx >= 2) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, threadIdx - 2);
	if (threadIdx >= 3) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, threadIdx - 3);
	NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata1, threadIdx, (localVal));

	_groupshared_sync_count++;

	if (threadIdx >= 4) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, threadIdx - 4);
	if (threadIdx >= 8) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, threadIdx - 8);
	if (threadIdx >= 12) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, threadIdx - 12);
	NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata0, threadIdx, (localVal));

	_groupshared_sync_count++;

	if (threadIdx >= 16) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, threadIdx - 16);
	if (threadIdx >= 32) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, threadIdx - 32);
	if (threadIdx >= 48) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata0, threadIdx - 48);
	NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._sdata1, threadIdx, (localVal));

	_groupshared_sync_count++;

	if (threadIdx >= 64) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, threadIdx - 64);
	if (threadIdx >= 128) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, threadIdx - 128);
	if (threadIdx >= 192) localVal += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, threadIdx - 192);

	// compute totalCount
	if (threadIdx == 0u)
	{
		NvFlowCPU_swrite(_groupshared_pass, _groupshared_sync_count, _groupshared[_groupshared_sync_count]._stotalCount, (NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 63) + NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 127) + NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 191) + NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._sdata1, 255)));
	}

	return localVal;
}


NV_FLOW_FORCE_INLINE void _main(_Globals* _globals, _Groupshared* _groupshared, int _groupshared_pass, int& _groupshared_sync_count,  NvFlowCPU_Uint3 cpu_DispatchThreadID, NvFlowCPU_Uint3 cpu_GroupID, NvFlowCPU_Uint3 cpu_GroupThreadID)
{
	int tidx = int(cpu_DispatchThreadID.x);
	NvFlowCPU_Uint threadIdx = NvFlowCPU_Uint(tidx) & 255u;

	NvFlowCPU_Uint accumIdx = 0u;
	for (NvFlowCPU_Uint passIdx = 0u; passIdx < _globals->_paramsIn.data->numTileBucketPasses; passIdx++)
	{
		NvFlowCPU_Uint tileBucketIdx = (passIdx << 8u) + threadIdx;

		NvFlowCPU_Uint bucketValue = (tileBucketIdx < _globals->_paramsIn.data->numTileBuckets) ? _globals->_tileCountOut[tileBucketIdx + _globals->_paramsIn.data->tileLocalTotalOffset] : 0u;

		NvFlowCPU_Uint scanVal = blockScan_0_1(_globals, _groupshared, _groupshared_pass, _groupshared_sync_count,  threadIdx,bucketValue);
		NvFlowCPU_Uint allocIdx = scanVal - bucketValue;

		if (tileBucketIdx < _globals->_paramsIn.data->numTileBuckets)
		{
			if (_groupshared_pass == _groupshared_sync_count) {_globals->_tileCountOut[tileBucketIdx + _globals->_paramsIn.data->tileGlobalScanOffset] = allocIdx + accumIdx;}
		}

		_groupshared_sync_count++;

		accumIdx += NvFlowCPU_sread(_groupshared[_groupshared_sync_count]._stotalCount);
	}

	if (threadIdx == 0u)
	{
		if (_groupshared_pass == _groupshared_sync_count) {_globals->_totalCountOut[0u] = accumIdx;}
	}

	// temp feedback
	//totalCountOut[1u + tidx] = tileCountOut[tidx + paramsIn.tileNumTrianglesOffset];
}

void _mainBlock(void* smemPool, NvFlowCPU_Uint3 groupID, NvFlowCPU_Uint numDescriptorWrites, const NvFlowDescriptorWrite* descriptorWrites, NvFlowCPU_Resource** resources)
{
	_Globals globals = {};
	NvFlowCPU_Resource* globals_bindings_src[3] = {};
	for(NvFlowCPU_Uint descriptorIdx = 0u; descriptorIdx < numDescriptorWrites; descriptorIdx++)
	{
		globals_bindings_src[descriptorWrites[descriptorIdx].write.vulkan.binding] = resources[descriptorIdx];
	};
	globals._paramsIn.bind(globals_bindings_src[0]);
	globals._tileCountOut.bind(globals_bindings_src[1]);
	globals._totalCountOut.bind(globals_bindings_src[2]);
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


} // namespace ImguiTileScanCS end
void* ImguiTileScanCS_cpu = (void*)ImguiTileScanCS::_mainBlock;
#else
void* ImguiTileScanCS_cpu = 0;
#endif
#undef NV_FLOW_CPU_SHADER


