
typedef unsigned char BYTE;
typedef unsigned int uint32_t;
#include "ImguiCS_combinedSampler.glsl.h"
#include "ImguiCS_separateSampler.glsl.h"
#include "ImguiCS_separateSampler.hlsl.h"
#include "ImguiCS_cpu_decl.h"

struct ImguiCS_Pipeline
{
	NvFlowContextInterface* contextInterface;
	NvFlowComputePipeline* pipeline;
};

inline void ImguiCS_init(NvFlowContextInterface* contextInterface, NvFlowContext* context, ImguiCS_Pipeline* ptr)
{
	ptr->contextInterface = contextInterface;
	NvFlowContextConfig config = {};
	ptr->contextInterface->getContextConfig(context, &config);
	NvFlowBindingDesc bindingDescs[11] = {};
	NvFlowComputePipelineDesc desc = {};
	if (config.api == eNvFlowContextApi_vulkan && config.textureBinding == eNvFlowTextureBindingType_combinedSampler)
	{
		bindingDescs[0].type = eNvFlowDescriptorType_constantBuffer;
		bindingDescs[0].bindingDesc.vulkan.binding = 0;
		bindingDescs[0].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[0].bindingDesc.vulkan.set = 0;

		bindingDescs[1].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[1].bindingDesc.vulkan.binding = 1;
		bindingDescs[1].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[1].bindingDesc.vulkan.set = 0;

		bindingDescs[2].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[2].bindingDesc.vulkan.binding = 2;
		bindingDescs[2].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[2].bindingDesc.vulkan.set = 0;

		bindingDescs[3].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[3].bindingDesc.vulkan.binding = 3;
		bindingDescs[3].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[3].bindingDesc.vulkan.set = 0;

		bindingDescs[4].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[4].bindingDesc.vulkan.binding = 4;
		bindingDescs[4].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[4].bindingDesc.vulkan.set = 0;

		bindingDescs[5].type = eNvFlowDescriptorType_textureSampler;
		bindingDescs[5].bindingDesc.vulkan.binding = 5;
		bindingDescs[5].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[5].bindingDesc.vulkan.set = 0;

		bindingDescs[6].type = eNvFlowDescriptorType_textureSampler;
		bindingDescs[6].bindingDesc.vulkan.binding = 6;
		bindingDescs[6].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[6].bindingDesc.vulkan.set = 0;

		bindingDescs[7].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[7].bindingDesc.vulkan.binding = 7;
		bindingDescs[7].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[7].bindingDesc.vulkan.set = 0;

		bindingDescs[8].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[8].bindingDesc.vulkan.binding = 8;
		bindingDescs[8].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[8].bindingDesc.vulkan.set = 0;

		bindingDescs[9].type = eNvFlowDescriptorType_textureSampler;
		bindingDescs[9].bindingDesc.vulkan.binding = 9;
		bindingDescs[9].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[9].bindingDesc.vulkan.set = 0;

		bindingDescs[10].type = eNvFlowDescriptorType_rwTexture;
		bindingDescs[10].bindingDesc.vulkan.binding = 10;
		bindingDescs[10].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[10].bindingDesc.vulkan.set = 0;

		desc.numBindingDescs = 11;
		desc.bindingDescs = bindingDescs;
		desc.bytecode.data = ImguiCS_combinedSampler_glsl;
		desc.bytecode.sizeInBytes = sizeof(ImguiCS_combinedSampler_glsl);
	}
	if (config.api == eNvFlowContextApi_vulkan && config.textureBinding == eNvFlowTextureBindingType_separateSampler)
	{
		bindingDescs[0].type = eNvFlowDescriptorType_constantBuffer;
		bindingDescs[0].bindingDesc.vulkan.binding = 0;
		bindingDescs[0].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[0].bindingDesc.vulkan.set = 0;

		bindingDescs[1].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[1].bindingDesc.vulkan.binding = 1;
		bindingDescs[1].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[1].bindingDesc.vulkan.set = 0;

		bindingDescs[2].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[2].bindingDesc.vulkan.binding = 2;
		bindingDescs[2].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[2].bindingDesc.vulkan.set = 0;

		bindingDescs[3].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[3].bindingDesc.vulkan.binding = 3;
		bindingDescs[3].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[3].bindingDesc.vulkan.set = 0;

		bindingDescs[4].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[4].bindingDesc.vulkan.binding = 4;
		bindingDescs[4].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[4].bindingDesc.vulkan.set = 0;

		bindingDescs[5].type = eNvFlowDescriptorType_texture;
		bindingDescs[5].bindingDesc.vulkan.binding = 5;
		bindingDescs[5].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[5].bindingDesc.vulkan.set = 0;

		bindingDescs[6].type = eNvFlowDescriptorType_sampler;
		bindingDescs[6].bindingDesc.vulkan.binding = 6;
		bindingDescs[6].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[6].bindingDesc.vulkan.set = 0;

		bindingDescs[7].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[7].bindingDesc.vulkan.binding = 7;
		bindingDescs[7].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[7].bindingDesc.vulkan.set = 0;

		bindingDescs[8].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[8].bindingDesc.vulkan.binding = 8;
		bindingDescs[8].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[8].bindingDesc.vulkan.set = 0;

		bindingDescs[9].type = eNvFlowDescriptorType_texture;
		bindingDescs[9].bindingDesc.vulkan.binding = 9;
		bindingDescs[9].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[9].bindingDesc.vulkan.set = 0;

		bindingDescs[10].type = eNvFlowDescriptorType_rwTexture;
		bindingDescs[10].bindingDesc.vulkan.binding = 10;
		bindingDescs[10].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[10].bindingDesc.vulkan.set = 0;

		desc.numBindingDescs = 11;
		desc.bindingDescs = bindingDescs;
		desc.bytecode.data = ImguiCS_separateSampler_glsl;
		desc.bytecode.sizeInBytes = sizeof(ImguiCS_separateSampler_glsl);
	}
	if (config.api == eNvFlowContextApi_d3d12 && config.textureBinding == eNvFlowTextureBindingType_separateSampler)
	{
		bindingDescs[0].type = eNvFlowDescriptorType_constantBuffer;
		bindingDescs[0].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_b;
		bindingDescs[0].bindingDesc.d3d12.registerBegin = 0;
		bindingDescs[0].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[0].bindingDesc.d3d12.space = 0;

		bindingDescs[1].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[1].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[1].bindingDesc.d3d12.registerBegin = 0;
		bindingDescs[1].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[1].bindingDesc.d3d12.space = 0;

		bindingDescs[2].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[2].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[2].bindingDesc.d3d12.registerBegin = 1;
		bindingDescs[2].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[2].bindingDesc.d3d12.space = 0;

		bindingDescs[3].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[3].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[3].bindingDesc.d3d12.registerBegin = 2;
		bindingDescs[3].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[3].bindingDesc.d3d12.space = 0;

		bindingDescs[4].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[4].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[4].bindingDesc.d3d12.registerBegin = 3;
		bindingDescs[4].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[4].bindingDesc.d3d12.space = 0;

		bindingDescs[5].type = eNvFlowDescriptorType_texture;
		bindingDescs[5].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[5].bindingDesc.d3d12.registerBegin = 4;
		bindingDescs[5].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[5].bindingDesc.d3d12.space = 0;

		bindingDescs[6].type = eNvFlowDescriptorType_sampler;
		bindingDescs[6].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_s;
		bindingDescs[6].bindingDesc.d3d12.registerBegin = 0;
		bindingDescs[6].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[6].bindingDesc.d3d12.space = 0;

		bindingDescs[7].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[7].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[7].bindingDesc.d3d12.registerBegin = 5;
		bindingDescs[7].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[7].bindingDesc.d3d12.space = 0;

		bindingDescs[8].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[8].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[8].bindingDesc.d3d12.registerBegin = 6;
		bindingDescs[8].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[8].bindingDesc.d3d12.space = 0;

		bindingDescs[9].type = eNvFlowDescriptorType_texture;
		bindingDescs[9].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		bindingDescs[9].bindingDesc.d3d12.registerBegin = 7;
		bindingDescs[9].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[9].bindingDesc.d3d12.space = 0;

		bindingDescs[10].type = eNvFlowDescriptorType_rwTexture;
		bindingDescs[10].bindingDesc.d3d12.registerHlsl = eNvFlowRegisterHlsl_u;
		bindingDescs[10].bindingDesc.d3d12.registerBegin = 0;
		bindingDescs[10].bindingDesc.d3d12.numRegisters = 1;
		bindingDescs[10].bindingDesc.d3d12.space = 0;

		desc.numBindingDescs = 11;
		desc.bindingDescs = bindingDescs;
		desc.bytecode.data = ImguiCS_separateSampler_hlsl;
		desc.bytecode.sizeInBytes = sizeof(ImguiCS_separateSampler_hlsl);
	}
	if (config.api == eNvFlowContextApi_cpu && config.textureBinding == eNvFlowTextureBindingType_separateSampler)
	{
		bindingDescs[0].type = eNvFlowDescriptorType_constantBuffer;
		bindingDescs[0].bindingDesc.vulkan.binding = 0;
		bindingDescs[0].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[0].bindingDesc.vulkan.set = 0;

		bindingDescs[1].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[1].bindingDesc.vulkan.binding = 1;
		bindingDescs[1].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[1].bindingDesc.vulkan.set = 0;

		bindingDescs[2].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[2].bindingDesc.vulkan.binding = 2;
		bindingDescs[2].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[2].bindingDesc.vulkan.set = 0;

		bindingDescs[3].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[3].bindingDesc.vulkan.binding = 3;
		bindingDescs[3].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[3].bindingDesc.vulkan.set = 0;

		bindingDescs[4].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[4].bindingDesc.vulkan.binding = 4;
		bindingDescs[4].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[4].bindingDesc.vulkan.set = 0;

		bindingDescs[5].type = eNvFlowDescriptorType_texture;
		bindingDescs[5].bindingDesc.vulkan.binding = 5;
		bindingDescs[5].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[5].bindingDesc.vulkan.set = 0;

		bindingDescs[6].type = eNvFlowDescriptorType_sampler;
		bindingDescs[6].bindingDesc.vulkan.binding = 6;
		bindingDescs[6].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[6].bindingDesc.vulkan.set = 0;

		bindingDescs[7].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[7].bindingDesc.vulkan.binding = 7;
		bindingDescs[7].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[7].bindingDesc.vulkan.set = 0;

		bindingDescs[8].type = eNvFlowDescriptorType_structuredBuffer;
		bindingDescs[8].bindingDesc.vulkan.binding = 8;
		bindingDescs[8].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[8].bindingDesc.vulkan.set = 0;

		bindingDescs[9].type = eNvFlowDescriptorType_texture;
		bindingDescs[9].bindingDesc.vulkan.binding = 9;
		bindingDescs[9].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[9].bindingDesc.vulkan.set = 0;

		bindingDescs[10].type = eNvFlowDescriptorType_rwTexture;
		bindingDescs[10].bindingDesc.vulkan.binding = 10;
		bindingDescs[10].bindingDesc.vulkan.descriptorCount = 1;
		bindingDescs[10].bindingDesc.vulkan.set = 0;

		desc.numBindingDescs = 11;
		desc.bindingDescs = bindingDescs;
		desc.bytecode.data = ImguiCS_cpu;
		desc.bytecode.sizeInBytes = sizeof(ImguiCS_cpu);
	}
	ptr->pipeline = ptr->contextInterface->createComputePipeline(context, &desc);
};

inline void ImguiCS_destroy(NvFlowContext* context, ImguiCS_Pipeline* ptr)
{
	ptr->contextInterface->destroyComputePipeline(context, ptr->pipeline);
}

struct ImguiCS_PassParams
{
	NvFlowBufferTransient* paramsIn;
	NvFlowBufferTransient* vertexPosTexCoordIn;
	NvFlowBufferTransient* vertexColorIn;
	NvFlowBufferTransient* indicesIn;
	NvFlowBufferTransient* drawCmdsIn;
	NvFlowTextureTransient* textureIn;
	NvFlowSampler* samplerIn;
	NvFlowBufferTransient* triangleIn;
	NvFlowBufferTransient* triangleRangeIn;
	NvFlowTextureTransient* colorIn;
	NvFlowTextureTransient* colorOut;
};

inline void ImguiCS_addPassCompute(NvFlowContext* context, ImguiCS_Pipeline* ptr, NvFlowUint3 gridDim, const ImguiCS_PassParams* params)
{
	NvFlowContextConfig config = {};
	ptr->contextInterface->getContextConfig(context, &config);
	NvFlowDescriptorWrite descriptorWrites[11] = {};
	NvFlowResource resources[11] = {};
	NvFlowPassComputeParams passComputeParams = {};
	passComputeParams.pipeline = ptr->pipeline;
	passComputeParams.gridDim = gridDim;
	if (config.api == eNvFlowContextApi_vulkan && config.textureBinding == eNvFlowTextureBindingType_combinedSampler)
	{
		NvFlowSampler* defaultSampler = ptr->contextInterface->getDefaultSampler(context);

		descriptorWrites[0].type = eNvFlowDescriptorType_constantBuffer;
		descriptorWrites[0].write.vulkan.binding = 0;
		descriptorWrites[0].write.vulkan.arrayIndex = 0;
		descriptorWrites[0].write.vulkan.set = 0;
		resources[0].bufferTransient = params->paramsIn;

		descriptorWrites[1].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[1].write.vulkan.binding = 1;
		descriptorWrites[1].write.vulkan.arrayIndex = 0;
		descriptorWrites[1].write.vulkan.set = 0;
		resources[1].bufferTransient = params->vertexPosTexCoordIn;

		descriptorWrites[2].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[2].write.vulkan.binding = 2;
		descriptorWrites[2].write.vulkan.arrayIndex = 0;
		descriptorWrites[2].write.vulkan.set = 0;
		resources[2].bufferTransient = params->vertexColorIn;

		descriptorWrites[3].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[3].write.vulkan.binding = 3;
		descriptorWrites[3].write.vulkan.arrayIndex = 0;
		descriptorWrites[3].write.vulkan.set = 0;
		resources[3].bufferTransient = params->indicesIn;

		descriptorWrites[4].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[4].write.vulkan.binding = 4;
		descriptorWrites[4].write.vulkan.arrayIndex = 0;
		descriptorWrites[4].write.vulkan.set = 0;
		resources[4].bufferTransient = params->drawCmdsIn;

		descriptorWrites[5].type = eNvFlowDescriptorType_textureSampler;
		descriptorWrites[5].write.vulkan.binding = 5;
		descriptorWrites[5].write.vulkan.arrayIndex = 0;
		descriptorWrites[5].write.vulkan.set = 0;
		resources[5].textureTransient = params->textureIn;
		resources[5].sampler = defaultSampler;

		descriptorWrites[6].type = eNvFlowDescriptorType_textureSampler;
		descriptorWrites[6].write.vulkan.binding = 6;
		descriptorWrites[6].write.vulkan.arrayIndex = 0;
		descriptorWrites[6].write.vulkan.set = 0;
		resources[6].textureTransient = params->textureIn;
		resources[6].sampler = params->samplerIn;

		descriptorWrites[7].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[7].write.vulkan.binding = 7;
		descriptorWrites[7].write.vulkan.arrayIndex = 0;
		descriptorWrites[7].write.vulkan.set = 0;
		resources[7].bufferTransient = params->triangleIn;

		descriptorWrites[8].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[8].write.vulkan.binding = 8;
		descriptorWrites[8].write.vulkan.arrayIndex = 0;
		descriptorWrites[8].write.vulkan.set = 0;
		resources[8].bufferTransient = params->triangleRangeIn;

		descriptorWrites[9].type = eNvFlowDescriptorType_textureSampler;
		descriptorWrites[9].write.vulkan.binding = 9;
		descriptorWrites[9].write.vulkan.arrayIndex = 0;
		descriptorWrites[9].write.vulkan.set = 0;
		resources[9].textureTransient = params->colorIn;
		resources[9].sampler = defaultSampler;

		descriptorWrites[10].type = eNvFlowDescriptorType_rwTexture;
		descriptorWrites[10].write.vulkan.binding = 10;
		descriptorWrites[10].write.vulkan.arrayIndex = 0;
		descriptorWrites[10].write.vulkan.set = 0;
		resources[10].textureTransient = params->colorOut;

		passComputeParams.numDescriptorWrites = 11;
		passComputeParams.descriptorWrites = descriptorWrites;
		passComputeParams.resources = resources;
		passComputeParams.debugLabel = "ImguiCS";
	}
	if (config.api == eNvFlowContextApi_vulkan && config.textureBinding == eNvFlowTextureBindingType_separateSampler)
	{
		descriptorWrites[0].type = eNvFlowDescriptorType_constantBuffer;
		descriptorWrites[0].write.vulkan.binding = 0;
		descriptorWrites[0].write.vulkan.arrayIndex = 0;
		descriptorWrites[0].write.vulkan.set = 0;
		resources[0].bufferTransient = params->paramsIn;

		descriptorWrites[1].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[1].write.vulkan.binding = 1;
		descriptorWrites[1].write.vulkan.arrayIndex = 0;
		descriptorWrites[1].write.vulkan.set = 0;
		resources[1].bufferTransient = params->vertexPosTexCoordIn;

		descriptorWrites[2].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[2].write.vulkan.binding = 2;
		descriptorWrites[2].write.vulkan.arrayIndex = 0;
		descriptorWrites[2].write.vulkan.set = 0;
		resources[2].bufferTransient = params->vertexColorIn;

		descriptorWrites[3].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[3].write.vulkan.binding = 3;
		descriptorWrites[3].write.vulkan.arrayIndex = 0;
		descriptorWrites[3].write.vulkan.set = 0;
		resources[3].bufferTransient = params->indicesIn;

		descriptorWrites[4].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[4].write.vulkan.binding = 4;
		descriptorWrites[4].write.vulkan.arrayIndex = 0;
		descriptorWrites[4].write.vulkan.set = 0;
		resources[4].bufferTransient = params->drawCmdsIn;

		descriptorWrites[5].type = eNvFlowDescriptorType_texture;
		descriptorWrites[5].write.vulkan.binding = 5;
		descriptorWrites[5].write.vulkan.arrayIndex = 0;
		descriptorWrites[5].write.vulkan.set = 0;
		resources[5].textureTransient = params->textureIn;

		descriptorWrites[6].type = eNvFlowDescriptorType_sampler;
		descriptorWrites[6].write.vulkan.binding = 6;
		descriptorWrites[6].write.vulkan.arrayIndex = 0;
		descriptorWrites[6].write.vulkan.set = 0;
		resources[6].sampler = params->samplerIn;

		descriptorWrites[7].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[7].write.vulkan.binding = 7;
		descriptorWrites[7].write.vulkan.arrayIndex = 0;
		descriptorWrites[7].write.vulkan.set = 0;
		resources[7].bufferTransient = params->triangleIn;

		descriptorWrites[8].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[8].write.vulkan.binding = 8;
		descriptorWrites[8].write.vulkan.arrayIndex = 0;
		descriptorWrites[8].write.vulkan.set = 0;
		resources[8].bufferTransient = params->triangleRangeIn;

		descriptorWrites[9].type = eNvFlowDescriptorType_texture;
		descriptorWrites[9].write.vulkan.binding = 9;
		descriptorWrites[9].write.vulkan.arrayIndex = 0;
		descriptorWrites[9].write.vulkan.set = 0;
		resources[9].textureTransient = params->colorIn;

		descriptorWrites[10].type = eNvFlowDescriptorType_rwTexture;
		descriptorWrites[10].write.vulkan.binding = 10;
		descriptorWrites[10].write.vulkan.arrayIndex = 0;
		descriptorWrites[10].write.vulkan.set = 0;
		resources[10].textureTransient = params->colorOut;

		passComputeParams.numDescriptorWrites = 11;
		passComputeParams.descriptorWrites = descriptorWrites;
		passComputeParams.resources = resources;
		passComputeParams.debugLabel = "ImguiCS";
	}
	if (config.api == eNvFlowContextApi_d3d12 && config.textureBinding == eNvFlowTextureBindingType_separateSampler)
	{
		descriptorWrites[0].type = eNvFlowDescriptorType_constantBuffer;
		descriptorWrites[0].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_b;
		descriptorWrites[0].write.d3d12.registerIndex = 0;
		descriptorWrites[0].write.d3d12.space = 0;
		resources[0].bufferTransient = params->paramsIn;

		descriptorWrites[1].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[1].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[1].write.d3d12.registerIndex = 0;
		descriptorWrites[1].write.d3d12.space = 0;
		resources[1].bufferTransient = params->vertexPosTexCoordIn;

		descriptorWrites[2].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[2].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[2].write.d3d12.registerIndex = 1;
		descriptorWrites[2].write.d3d12.space = 0;
		resources[2].bufferTransient = params->vertexColorIn;

		descriptorWrites[3].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[3].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[3].write.d3d12.registerIndex = 2;
		descriptorWrites[3].write.d3d12.space = 0;
		resources[3].bufferTransient = params->indicesIn;

		descriptorWrites[4].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[4].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[4].write.d3d12.registerIndex = 3;
		descriptorWrites[4].write.d3d12.space = 0;
		resources[4].bufferTransient = params->drawCmdsIn;

		descriptorWrites[5].type = eNvFlowDescriptorType_texture;
		descriptorWrites[5].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[5].write.d3d12.registerIndex = 4;
		descriptorWrites[5].write.d3d12.space = 0;
		resources[5].textureTransient = params->textureIn;

		descriptorWrites[6].type = eNvFlowDescriptorType_sampler;
		descriptorWrites[6].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_s;
		descriptorWrites[6].write.d3d12.registerIndex = 0;
		descriptorWrites[6].write.d3d12.space = 0;
		resources[6].sampler = params->samplerIn;

		descriptorWrites[7].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[7].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[7].write.d3d12.registerIndex = 5;
		descriptorWrites[7].write.d3d12.space = 0;
		resources[7].bufferTransient = params->triangleIn;

		descriptorWrites[8].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[8].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[8].write.d3d12.registerIndex = 6;
		descriptorWrites[8].write.d3d12.space = 0;
		resources[8].bufferTransient = params->triangleRangeIn;

		descriptorWrites[9].type = eNvFlowDescriptorType_texture;
		descriptorWrites[9].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_t;
		descriptorWrites[9].write.d3d12.registerIndex = 7;
		descriptorWrites[9].write.d3d12.space = 0;
		resources[9].textureTransient = params->colorIn;

		descriptorWrites[10].type = eNvFlowDescriptorType_rwTexture;
		descriptorWrites[10].write.d3d12.registerHlsl = eNvFlowRegisterHlsl_u;
		descriptorWrites[10].write.d3d12.registerIndex = 0;
		descriptorWrites[10].write.d3d12.space = 0;
		resources[10].textureTransient = params->colorOut;

		passComputeParams.numDescriptorWrites = 11;
		passComputeParams.descriptorWrites = descriptorWrites;
		passComputeParams.resources = resources;
		passComputeParams.debugLabel = "ImguiCS";
	}
	if (config.api == eNvFlowContextApi_cpu && config.textureBinding == eNvFlowTextureBindingType_separateSampler)
	{
		descriptorWrites[0].type = eNvFlowDescriptorType_constantBuffer;
		descriptorWrites[0].write.vulkan.binding = 0;
		descriptorWrites[0].write.vulkan.arrayIndex = 0;
		descriptorWrites[0].write.vulkan.set = 0;
		resources[0].bufferTransient = params->paramsIn;

		descriptorWrites[1].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[1].write.vulkan.binding = 1;
		descriptorWrites[1].write.vulkan.arrayIndex = 0;
		descriptorWrites[1].write.vulkan.set = 0;
		resources[1].bufferTransient = params->vertexPosTexCoordIn;

		descriptorWrites[2].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[2].write.vulkan.binding = 2;
		descriptorWrites[2].write.vulkan.arrayIndex = 0;
		descriptorWrites[2].write.vulkan.set = 0;
		resources[2].bufferTransient = params->vertexColorIn;

		descriptorWrites[3].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[3].write.vulkan.binding = 3;
		descriptorWrites[3].write.vulkan.arrayIndex = 0;
		descriptorWrites[3].write.vulkan.set = 0;
		resources[3].bufferTransient = params->indicesIn;

		descriptorWrites[4].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[4].write.vulkan.binding = 4;
		descriptorWrites[4].write.vulkan.arrayIndex = 0;
		descriptorWrites[4].write.vulkan.set = 0;
		resources[4].bufferTransient = params->drawCmdsIn;

		descriptorWrites[5].type = eNvFlowDescriptorType_texture;
		descriptorWrites[5].write.vulkan.binding = 5;
		descriptorWrites[5].write.vulkan.arrayIndex = 0;
		descriptorWrites[5].write.vulkan.set = 0;
		resources[5].textureTransient = params->textureIn;

		descriptorWrites[6].type = eNvFlowDescriptorType_sampler;
		descriptorWrites[6].write.vulkan.binding = 6;
		descriptorWrites[6].write.vulkan.arrayIndex = 0;
		descriptorWrites[6].write.vulkan.set = 0;
		resources[6].sampler = params->samplerIn;

		descriptorWrites[7].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[7].write.vulkan.binding = 7;
		descriptorWrites[7].write.vulkan.arrayIndex = 0;
		descriptorWrites[7].write.vulkan.set = 0;
		resources[7].bufferTransient = params->triangleIn;

		descriptorWrites[8].type = eNvFlowDescriptorType_structuredBuffer;
		descriptorWrites[8].write.vulkan.binding = 8;
		descriptorWrites[8].write.vulkan.arrayIndex = 0;
		descriptorWrites[8].write.vulkan.set = 0;
		resources[8].bufferTransient = params->triangleRangeIn;

		descriptorWrites[9].type = eNvFlowDescriptorType_texture;
		descriptorWrites[9].write.vulkan.binding = 9;
		descriptorWrites[9].write.vulkan.arrayIndex = 0;
		descriptorWrites[9].write.vulkan.set = 0;
		resources[9].textureTransient = params->colorIn;

		descriptorWrites[10].type = eNvFlowDescriptorType_rwTexture;
		descriptorWrites[10].write.vulkan.binding = 10;
		descriptorWrites[10].write.vulkan.arrayIndex = 0;
		descriptorWrites[10].write.vulkan.set = 0;
		resources[10].textureTransient = params->colorOut;

		passComputeParams.numDescriptorWrites = 11;
		passComputeParams.descriptorWrites = descriptorWrites;
		passComputeParams.resources = resources;
		passComputeParams.debugLabel = "ImguiCS";
	}
	ptr->contextInterface->addPassCompute(context, &passComputeParams);
};
