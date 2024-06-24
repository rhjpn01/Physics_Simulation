#define NV_FLOW_CPU_SHADER
#include "NvFlowMath.h"
#include "NvFlowResourceCPU.h"
#ifndef NV_FLOW_CPU_SHADER_DISABLE
namespace ShapeCS
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

struct ShapeRendererParams
{
	NvFlowCPU_Float4x4 projection;
	NvFlowCPU_Float4x4 view;
	NvFlowCPU_Float4x4 projectionInv;
	NvFlowCPU_Float4x4 viewInv;

	NvFlowCPU_Float4 rayDir00;
	NvFlowCPU_Float4 rayDir10;
	NvFlowCPU_Float4 rayDir01;
	NvFlowCPU_Float4 rayDir11;

	NvFlowCPU_Float4 rayOrigin00;
	NvFlowCPU_Float4 rayOrigin10;
	NvFlowCPU_Float4 rayOrigin01;
	NvFlowCPU_Float4 rayOrigin11;

	float width;
	float height;
	float widthInv;
	float heightInv;

	NvFlowCPU_Uint numSpheres;
	float clearDepth;
	NvFlowCPU_Uint isReverseZ;
	NvFlowCPU_Uint pad3;

	NvFlowCPU_Float4 clearColor;
};








//struct ShapeRendererParams;

struct _Groupshared
{
}; 

struct _Globals
{
	NvFlowCPU_ConstantBuffer<ShapeRendererParams> _paramsIn;
	NvFlowCPU_StructuredBuffer<NvFlowCPU_Float4> _spherePositionRadiusIn;
	NvFlowCPU_RWTexture2D<float> _depthOut;
	NvFlowCPU_RWTexture2D<NvFlowCPU_Float4> _colorOut;
}; 

NV_FLOW_FORCE_INLINE bool raySphereIntersection_0_1_2_3_4(_Globals* _globals,  NvFlowCPU_Float3 o,NvFlowCPU_Float3 d,NvFlowCPU_Float3 c,float r,float &t)
{
	bool ret = false;
	NvFlowCPU_Float3 l = c - o;
	float s = NvFlowCPU_dot(l, d);
	float l2 = NvFlowCPU_dot(l, l);
	float r2 = r * r;
	if (s >= 0.f || l2 <= r2)
	{
		float s2 = s * s;
		float m2 = l2 - s2;
		if (m2 <= r2)
		{
			float q = NvFlowCPU_sqrt(r2 - m2);
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


NV_FLOW_FORCE_INLINE void _main(_Globals* _globals,  NvFlowCPU_Uint3 cpu_DispatchThreadID, NvFlowCPU_Uint3 cpu_GroupID, NvFlowCPU_Uint3 cpu_GroupThreadID)
{
	NvFlowCPU_Int2 tidx = NvFlowCPU_Int2(cpu_DispatchThreadID.rg());

	NvFlowCPU_Float2 pixelCenter = NvFlowCPU_Float2(tidx) + 0.5f;
	NvFlowCPU_Float2 inUV = pixelCenter * NvFlowCPU_Float2(_globals->_paramsIn.data->widthInv, _globals->_paramsIn.data->heightInv);

	float w00 = (1.f - inUV.x) * (1.f - inUV.y);
	float w10 = (inUV.x) * (1.f - inUV.y);
	float w01 = (1.f - inUV.x) * (inUV.y);
	float w11 = (inUV.x) * (inUV.y);

	NvFlowCPU_Float3 rayDir = NvFlowCPU_normalize(w00 * _globals->_paramsIn.data->rayDir00.rgb() + w10 * _globals->_paramsIn.data->rayDir10.rgb() + w01 * _globals->_paramsIn.data->rayDir01.rgb() + w11 * _globals->_paramsIn.data->rayDir11.rgb());
	NvFlowCPU_Float3 rayOrigin = w00 * _globals->_paramsIn.data->rayOrigin00.rgb() + w10 * _globals->_paramsIn.data->rayOrigin10.rgb() + w01 * _globals->_paramsIn.data->rayOrigin01.rgb() + w11 * _globals->_paramsIn.data->rayOrigin11.rgb();

	float globalDepth = _globals->_paramsIn.data->clearDepth;
	NvFlowCPU_Float4 globalColor = _globals->_paramsIn.data->clearColor;

	for (NvFlowCPU_Uint sphereIdx = 0u; sphereIdx < _globals->_paramsIn.data->numSpheres; sphereIdx++)
	{
		NvFlowCPU_Float4 spherePositionRadius = _globals->_spherePositionRadiusIn[sphereIdx];
		float hitT = -1.f;
		if (raySphereIntersection_0_1_2_3_4(_globals,  rayOrigin,rayDir,spherePositionRadius.rgb(),spherePositionRadius.w,hitT))
		{
			NvFlowCPU_Float4 worldPos = NvFlowCPU_Float4(rayDir * hitT + rayOrigin, 1.f);

			NvFlowCPU_Float4 clipPos = NvFlowCPU_mul(worldPos, _globals->_paramsIn.data->view);
			clipPos = NvFlowCPU_mul(clipPos, _globals->_paramsIn.data->projection);
			
			float depth = clipPos.z / clipPos.w;

			bool isVisible = bool(_globals->_paramsIn.data->isReverseZ) ? (depth > globalDepth) : (depth < globalDepth);
			if (isVisible)
			{
				NvFlowCPU_Float3 n = NvFlowCPU_normalize(worldPos.rgb() - spherePositionRadius.rgb());

				NvFlowCPU_Float4 color = NvFlowCPU_Float4(NvFlowCPU_abs(n), 1.f);

				globalDepth = depth;
				globalColor = color;
			}
		}
	}

	NvFlowCPU_textureWrite(_globals->_depthOut, tidx, (globalDepth));
	NvFlowCPU_textureWrite(_globals->_colorOut, tidx, (globalColor));
}

void _mainBlock(void* smemPool, NvFlowCPU_Uint3 groupID, NvFlowCPU_Uint numDescriptorWrites, const NvFlowDescriptorWrite* descriptorWrites, NvFlowCPU_Resource** resources)
{
	_Globals globals = {};
	NvFlowCPU_Resource* globals_bindings_src[4] = {};
	for(NvFlowCPU_Uint descriptorIdx = 0u; descriptorIdx < numDescriptorWrites; descriptorIdx++)
	{
		globals_bindings_src[descriptorWrites[descriptorIdx].write.vulkan.binding] = resources[descriptorIdx];
	};
	globals._paramsIn.bind(globals_bindings_src[0]);
	globals._spherePositionRadiusIn.bind(globals_bindings_src[1]);
	globals._depthOut.bind(globals_bindings_src[2]);
	globals._colorOut.bind(globals_bindings_src[3]);
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



} // namespace ShapeCS end
void* ShapeCS_cpu = (void*)ShapeCS::_mainBlock;
#else
void* ShapeCS_cpu = 0;
#endif
#undef NV_FLOW_CPU_SHADER


