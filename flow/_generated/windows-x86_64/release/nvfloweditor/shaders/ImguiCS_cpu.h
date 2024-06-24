#define NV_FLOW_CPU_SHADER
#include "NvFlowMath.h"
#include "NvFlowResourceCPU.h"
#ifndef NV_FLOW_CPU_SHADER_DISABLE
namespace ImguiCS
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
}; 

struct _Globals
{
	NvFlowCPU_ConstantBuffer<ImguiRendererParams> _paramsIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Float4> _vertexPosTexCoordIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint> _vertexColorIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint> _indicesIn;
	NvFlowCPU_StructuredBuffer<ImguiRendererDrawCmd> _drawCmdsIn;
	NvFlowCPU_Texture2D<NvFlowCPU_Float4> _textureIn;
	NvFlowCPU_SamplerState _samplerIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint> _triangleIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Uint2> _triangleRangeIn;
	NvFlowCPU_Texture2D<NvFlowCPU_Float4> _colorIn;
	NvFlowCPU_RWTexture2D<NvFlowCPU_Float4> _colorOut;
}; 

NV_FLOW_FORCE_INLINE NvFlowCPU_Float4 convertColor_0(_Globals* _globals,  NvFlowCPU_Uint val)
{
	return NvFlowCPU_Float4(
		((val >>  0) & 255) * (1.f / 255.f),
		((val >>  8) & 255) * (1.f / 255.f),
		((val >> 16) & 255) * (1.f / 255.f),
		((val >> 24) & 255) * (1.f / 255.f)
	);
}

NV_FLOW_FORCE_INLINE NvFlowCPU_Float3 computeBary_0_1_2_3(_Globals* _globals,  NvFlowCPU_Float2 p1,NvFlowCPU_Float2 p2,NvFlowCPU_Float2 p3,NvFlowCPU_Float2 p)
{
	float det = (p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y);
	float b1 = (p2.y - p3.y) * (p.x - p3.x) + (p3.x - p2.x) * (p.y - p3.y);
	float b2 = (p3.y - p1.y) * (p.x - p3.x) + (p1.x - p3.x) * (p.y - p3.y);
	b1 = b1 / det;
	b2 = b2 / det;
	float b3 = 1.f - b1 - b2;
	return NvFlowCPU_Float3(b1, b2, b3);
}

NV_FLOW_FORCE_INLINE bool edgeTest_0_1_2_3(_Globals* _globals,  NvFlowCPU_Float2 edgeA_fp,NvFlowCPU_Float2 edgeB_fp,NvFlowCPU_Float2 inside_fp,NvFlowCPU_Float2 pt_fp)
{
	NvFlowCPU_Int2 edgeA = NvFlowCPU_Int2(edgeA_fp);
	NvFlowCPU_Int2 edgeB = NvFlowCPU_Int2(edgeB_fp);
	NvFlowCPU_Int2 inside = NvFlowCPU_Int2(inside_fp);
	NvFlowCPU_Int2 pt = NvFlowCPU_Int2(pt_fp);

	NvFlowCPU_Int2 m = edgeB - edgeA;
	bool isBelow;
	if (NvFlowCPU_abs(m.y) > NvFlowCPU_abs(m.x))
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

NV_FLOW_FORCE_INLINE NvFlowCPU_Float4 leaf_0_1(_Globals* _globals,  NvFlowCPU_Float2 p,NvFlowCPU_Uint triangleIdx)
{
	NvFlowCPU_Uint indexOffsetLocal = 3u * triangleIdx;

	NvFlowCPU_Uint idx0 = _globals->_indicesIn[indexOffsetLocal + 0];
	NvFlowCPU_Uint idx1 = _globals->_indicesIn[indexOffsetLocal + 1];
	NvFlowCPU_Uint idx2 = _globals->_indicesIn[indexOffsetLocal + 2];

	NvFlowCPU_Float4 pos0 = _globals->_vertexPosTexCoordIn[idx0];
	NvFlowCPU_Float4 pos1 = _globals->_vertexPosTexCoordIn[idx1];
	NvFlowCPU_Float4 pos2 = _globals->_vertexPosTexCoordIn[idx2];

	NvFlowCPU_Float2 p1 = 16.f * pos0.rg();
	NvFlowCPU_Float2 p2 = 16.f * pos1.rg();
	NvFlowCPU_Float2 p3 = 16.f * pos2.rg();

	bool passed = edgeTest_0_1_2_3(_globals,  p1,p2,p3,p);
	passed = passed && edgeTest_0_1_2_3(_globals,  p2,p3,p1,p); 
	passed = passed && edgeTest_0_1_2_3(_globals,  p3,p1,p2,p);

	NvFlowCPU_Float4 c = NvFlowCPU_Float4(0.f, 0.f, 0.f, 0.f);
	if (passed)
	{
		NvFlowCPU_Float3 b = computeBary_0_1_2_3(_globals,  p1,p2,p3,p);

		c = b.x * convertColor_0(_globals,  _globals->_vertexColorIn[idx0]) +
			b.y * convertColor_0(_globals,  _globals->_vertexColorIn[idx1]) +
			b.z * convertColor_0(_globals,  _globals->_vertexColorIn[idx2]);

		NvFlowCPU_Float2 uv = b.x * pos0.ba() + b.y * pos1.ba() + b.z * pos2.ba();

		c *= NvFlowCPU_textureSampleLevel(_globals->_textureIn, _globals->_samplerIn, uv, 0.0);
	}
	return c;
}


NV_FLOW_FORCE_INLINE void _main(_Globals* _globals,  NvFlowCPU_Uint3 cpu_DispatchThreadID, NvFlowCPU_Uint3 cpu_GroupID, NvFlowCPU_Uint3 cpu_GroupThreadID)
{
	NvFlowCPU_Int2 tidx = NvFlowCPU_Int2(cpu_DispatchThreadID.rg());

	NvFlowCPU_Float4 color = NvFlowCPU_textureRead(_globals->_colorIn, tidx);

	NvFlowCPU_Int2 tileIdx = NvFlowCPU_Int2(
		tidx.x >> _globals->_paramsIn.data->tileDimBits,
		tidx.y >> _globals->_paramsIn.data->tileDimBits
	);
	int tileIdx1D = int(tileIdx.y * _globals->_paramsIn.data->tileGridDim_x + tileIdx.x);

	if (tileIdx.y >= int(_globals->_paramsIn.data->tileGridDim_y))
	{
		color.y = 1.f;
	}

	NvFlowCPU_Uint2 triangleListRange = _globals->_triangleRangeIn[tileIdx1D];

	NvFlowCPU_Float2 tidxf = NvFlowCPU_Float2(tidx) + NvFlowCPU_Float2(0.5f, 0.5f);
	NvFlowCPU_Float2 p = 16.f * tidxf;

	for (NvFlowCPU_Uint listIdx = 0u; listIdx < triangleListRange.y; listIdx++)
	{
		NvFlowCPU_Uint triangleIdxRaw = _globals->_triangleIn[triangleListRange.x + listIdx];
		NvFlowCPU_Uint triangleIdx = triangleIdxRaw & 0x00FFFFFF;
		NvFlowCPU_Uint drawCmdIdx = triangleIdxRaw >> 24u;

		NvFlowCPU_Float4 c = leaf_0_1(_globals,  p,triangleIdx);

		//color = (1.f - c.w) * color + c.w * c;

		NvFlowCPU_Float4 clipRect = _globals->_drawCmdsIn[drawCmdIdx].clipRect;
		// (x1, y1, x2, y2)
		if (tidx.x >= clipRect.x && tidx.y >= clipRect.y &&
			tidx.x < clipRect.z && tidx.y < clipRect.w)
		{
			color = (1.f - c.w) * color + c.w * c;
		}
	}

	NvFlowCPU_textureWrite(_globals->_colorOut, tidx, (color));
}

void _mainBlock(void* smemPool, NvFlowCPU_Uint3 groupID, NvFlowCPU_Uint numDescriptorWrites, const NvFlowDescriptorWrite* descriptorWrites, NvFlowCPU_Resource** resources)
{
	_Globals globals = {};
	NvFlowCPU_Resource* globals_bindings_src[11] = {};
	for(NvFlowCPU_Uint descriptorIdx = 0u; descriptorIdx < numDescriptorWrites; descriptorIdx++)
	{
		globals_bindings_src[descriptorWrites[descriptorIdx].write.vulkan.binding] = resources[descriptorIdx];
	};
	globals._paramsIn.bind(globals_bindings_src[0]);
	globals._vertexPosTexCoordIn.bind(globals_bindings_src[1]);
	globals._vertexColorIn.bind(globals_bindings_src[2]);
	globals._indicesIn.bind(globals_bindings_src[3]);
	globals._drawCmdsIn.bind(globals_bindings_src[4]);
	globals._textureIn.bind(globals_bindings_src[5]);
	globals._samplerIn.bind(globals_bindings_src[6]);
	globals._triangleIn.bind(globals_bindings_src[7]);
	globals._triangleRangeIn.bind(globals_bindings_src[8]);
	globals._colorIn.bind(globals_bindings_src[9]);
	globals._colorOut.bind(globals_bindings_src[10]);
	for(NvFlowCPU_Uint threadIdx_k = 0u; threadIdx_k < (1); threadIdx_k++)
	{
		for(NvFlowCPU_Uint threadIdx_j = 0u; threadIdx_j < (8); threadIdx_j++)
		{
			for(NvFlowCPU_Uint threadIdx_i = 0u; threadIdx_i < (8); threadIdx_i++)
			{
				NvFlowCPU_Uint3 cpu_DispatchThreadID(groupID.x * (8) + threadIdx_i, groupID.y * (8) + threadIdx_j, groupID.z * (1) + threadIdx_k);
				NvFlowCPU_Uint3 groupThreadID(threadIdx_i, threadIdx_j, threadIdx_k);
				_main(&globals, cpu_DispatchThreadID, groupID, groupThreadID);
			}
		}
	}
}



} // namespace ImguiCS end
void* ImguiCS_cpu = (void*)ImguiCS::_mainBlock;
#else
void* ImguiCS_cpu = 0;
#endif
#undef NV_FLOW_CPU_SHADER


