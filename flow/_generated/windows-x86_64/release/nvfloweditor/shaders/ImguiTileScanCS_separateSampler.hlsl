
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
	float4 clipRect;
	uint elemCount;
	uint userTexture;
	uint vertexOffset;
	uint indexOffset;
};

cbuffer paramsIn_t : register(b0)
{
	ImguiRendererParams paramsIn;
};

RWStructuredBuffer<uint> tileCountOut : register(u0);
RWStructuredBuffer<uint> totalCountOut : register(u1);

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

groupshared uint sdata0[256u];
groupshared uint sdata1[256u];

groupshared uint stotalCount;


uint blockScan_0_1( uint threadIdx,uint val)
{
	uint localVal = val;
	sdata0[threadIdx] = localVal;

	GroupMemoryBarrierWithGroupSync();

	if (threadIdx >= 1) localVal += sdata0[threadIdx - 1];
	if (threadIdx >= 2) localVal += sdata0[threadIdx - 2];
	if (threadIdx >= 3) localVal += sdata0[threadIdx - 3];
	sdata1[threadIdx] = localVal;

	GroupMemoryBarrierWithGroupSync();

	if (threadIdx >= 4) localVal += sdata1[threadIdx - 4];
	if (threadIdx >= 8) localVal += sdata1[threadIdx - 8];
	if (threadIdx >= 12) localVal += sdata1[threadIdx - 12];
	sdata0[threadIdx] = localVal;

	GroupMemoryBarrierWithGroupSync();

	if (threadIdx >= 16) localVal += sdata0[threadIdx - 16];
	if (threadIdx >= 32) localVal += sdata0[threadIdx - 32];
	if (threadIdx >= 48) localVal += sdata0[threadIdx - 48];
	sdata1[threadIdx] = localVal;

	GroupMemoryBarrierWithGroupSync();

	if (threadIdx >= 64) localVal += sdata1[threadIdx - 64];
	if (threadIdx >= 128) localVal += sdata1[threadIdx - 128];
	if (threadIdx >= 192) localVal += sdata1[threadIdx - 192];

	// compute totalCount
	if (threadIdx == 0u)
	{
		stotalCount = sdata1[63] + sdata1[127] + sdata1[191] + sdata1[255];
	}

	return localVal;
}

[numthreads(256, 1, 1)]
void main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
	int tidx = int(dispatchThreadID.x);
	uint threadIdx = uint(tidx) & 255u;

	uint accumIdx = 0u;
	for (uint passIdx = 0u; passIdx < paramsIn.numTileBucketPasses; passIdx++)
	{
		uint tileBucketIdx = (passIdx << 8u) + threadIdx;

		uint bucketValue = (tileBucketIdx < paramsIn.numTileBuckets) ? tileCountOut[tileBucketIdx + paramsIn.tileLocalTotalOffset] : 0u;

		uint scanVal = blockScan_0_1( threadIdx,bucketValue);
		uint allocIdx = scanVal - bucketValue;

		if (tileBucketIdx < paramsIn.numTileBuckets)
		{
			tileCountOut[tileBucketIdx + paramsIn.tileGlobalScanOffset] = allocIdx + accumIdx;
		}

		GroupMemoryBarrierWithGroupSync();

		accumIdx += stotalCount;
	}

	if (threadIdx == 0u)
	{
		totalCountOut[0u] = accumIdx;
	}

	// temp feedback
	//totalCountOut[1u + tidx] = tileCountOut[tidx + paramsIn.tileNumTrianglesOffset];
}


