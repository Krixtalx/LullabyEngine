#include "stdafx.h"
#include "PipelineBuilder.h"

#include "../VKInitializers.h"

Lullaby::PipelineBuilder::PipelineBuilder() = default;

Lullaby::PipelineBuilder::~PipelineBuilder() = default;

VkPipeline Lullaby::PipelineBuilder::buildPipeline(PipelineInfo& pipelineInfo, const VkDevice& device, const VkRenderPass& renderPass) {
	//make viewport state from our stored viewport and scissor.
	//at the moment we won't support multiple viewports or scissors
	const VkPipelineViewportStateCreateInfo viewportState = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.viewportCount = 1,
		.pViewports = &pipelineInfo._viewport,
		.scissorCount = 1,
		.pScissors = &pipelineInfo._scissor
	};

	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	const VkPipelineColorBlendStateCreateInfo colorBlending = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &pipelineInfo._colorBlendAttachment
	};

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	const VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = nullptr,
		.stageCount = static_cast<uint32_t>(pipelineInfo._shaderStages.size()),
		.pStages = pipelineInfo._shaderStages.data(),
		.pVertexInputState = &pipelineInfo._vertexInputInfo,
		.pInputAssemblyState = &pipelineInfo._inputAssembly,
		.pViewportState = &viewportState,
		.pRasterizationState = &pipelineInfo._rasterizer,
		.pMultisampleState = &pipelineInfo._multisampling,
		.pDepthStencilState = &pipelineInfo._depthStencil,
		.pColorBlendState = &colorBlending,
		.layout = pipelineInfo._pipelineLayout,
		.renderPass = renderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
	};

	VkPipeline newPipeline = nullptr;
	Helpers::checkVulkanError(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &newPipeline), "creating pipeline");
	return newPipeline;

}

VkShaderModule Lullaby::PipelineBuilder::loadShaderModule(const VkDevice& device, const std::string& path) {
	//open the file. With cursor at the end
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return nullptr;

	//find what the size of the file is by looking up the location of the cursor
	//because the cursor is at the end, it gives the size directly in bytes
	const size_t fileSize = file.tellg();

	//spirv expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	//put file cursor at beginning
	file.seekg(0);

	//load the entire file into the buffer
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

	//now that the file is loaded into the buffer, we can close it
	file.close();

	//create a new shader module, using the buffer we loaded
	const VkShaderModuleCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		//codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
		.codeSize = buffer.size() * sizeof(uint32_t),
		.pCode = buffer.data()
	};

	VkShaderModule shaderModule = nullptr;
	vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

	return shaderModule;
}

VkPipelineShaderStageCreateInfo Lullaby::PipelineBuilder::defaultShaderStageInfo(const VkShaderStageFlagBits stage, const VkShaderModule shaderModule) {
	const VkPipelineShaderStageCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.pNext = nullptr,
		//Shader stage
		.stage = stage,
		//Shader module that contains the code for this stage
		.module = shaderModule,
		//Entry point of the shader. 
		.pName = "main",
	};

	return info;
}

VkPipelineVertexInputStateCreateInfo Lullaby::PipelineBuilder::defaultVertexInputInfo() {
	constexpr VkPipelineVertexInputStateCreateInfo vertexInputInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0
	};
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo Lullaby::PipelineBuilder::defaultInputAssemblyInfo(const VkPrimitiveTopology topology) {
	const VkPipelineInputAssemblyStateCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.pNext = nullptr,
		.topology = topology,
		.primitiveRestartEnable = VK_FALSE
	};

	return info;
}

VkPipelineRasterizationStateCreateInfo Lullaby::PipelineBuilder::defaultRasterizationInfo(const VkPolygonMode polygonMode) {
	const VkPipelineRasterizationStateCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.pNext = nullptr,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = polygonMode,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = .0f,
		.depthBiasClamp = .0f,
		.depthBiasSlopeFactor = .0f,
		.lineWidth = 1.0f
	};
	return info;
}

VkPipelineMultisampleStateCreateInfo Lullaby::PipelineBuilder::defaultMultisampleInfo() {
	const VkPipelineMultisampleStateCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.pNext = nullptr,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};
	return info;
}

VkPipelineColorBlendAttachmentState Lullaby::PipelineBuilder::defaultColorBlendState() {
	const VkPipelineColorBlendAttachmentState info{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};
	return info;
}

VkPipelineLayoutCreateInfo Lullaby::PipelineBuilder::defaultLayoutInfo() {
	const VkPipelineLayoutCreateInfo info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};
	return info;
}
