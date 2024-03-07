#include "LullabyPch.h"
#include "PipelineBuilder.h"

#include "../VKInitializers.h"

Lullaby::PipelineBuilder::PipelineBuilder() = default;

Lullaby::PipelineBuilder::~PipelineBuilder() = default;

vk::Pipeline Lullaby::PipelineBuilder::buildPipeline(PipelineInfo& pipelineInfo, const vk::Device& device, const vk::RenderPass& renderPass) {
	//make viewport state from our stored viewport and scissor.
	//at the moment we won't support multiple viewports or scissors
	const vk::PipelineViewportStateCreateInfo viewportState = {
		.sType = vk::StructureType::ePipelineViewportStateCreateInfo,
		.pNext = nullptr,
		.viewportCount = 1,
		.pViewports = &pipelineInfo._viewport,
		.scissorCount = 1,
		.pScissors = &pipelineInfo._scissor
	};

	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	const vk::PipelineColorBlendStateCreateInfo colorBlending = {
		.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo,
		.pNext = nullptr,
		.logicOpEnable = VK_FALSE,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &pipelineInfo._colorBlendAttachment
	};

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	const vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {
		.sType = vk::StructureType::eGraphicsPipelineCreateInfo,
		.pNext = nullptr,
		.stageCount = static_cast<u32>(pipelineInfo._shaderStages.size()),
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
	const auto temp = device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineCreateInfo);
	Helpers::checkVulkanError(temp.result, "creating pipeline");
	const vk::Pipeline newPipeline = temp.value;
	return newPipeline;

}

vk::ShaderModule Lullaby::PipelineBuilder::loadShaderModule(const vk::Device& device, const std::string& path) {
	//open the file. With cursor at the end
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		return nullptr;

	//find what the size of the file is by looking up the location of the cursor
	//because the cursor is at the end, it gives the size directly in bytes
	const size_t fileSize = file.tellg();

	//spirv expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
	std::vector<u32> buffer(fileSize / sizeof(u32));

	//put file cursor at beginning
	file.seekg(0);

	//load the entire file into the buffer
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

	//now that the file is loaded into the buffer, we can close it
	file.close();

	//create a new shader module, using the buffer we loaded
	const vk::ShaderModuleCreateInfo createInfo = {
		.sType = vk::StructureType::eShaderModuleCreateInfo,
		.pNext = nullptr,
		//codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
		.codeSize = buffer.size() * sizeof(u32),
		.pCode = buffer.data()
	};

	vk::ShaderModule shaderModule = nullptr;
	Helpers::checkVulkanError(device.createShaderModule(&createInfo, nullptr, &shaderModule), "creating shader module");

	return shaderModule;
}

vk::PipelineShaderStageCreateInfo Lullaby::PipelineBuilder::defaultShaderStageInfo(const vk::ShaderStageFlagBits stage, const vk::ShaderModule shaderModule) {
	const vk::PipelineShaderStageCreateInfo info{
		.sType = vk::StructureType::ePipelineShaderStageCreateInfo,
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

vk::PipelineVertexInputStateCreateInfo Lullaby::PipelineBuilder::defaultVertexInputInfo() {
	constexpr vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
		.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo,
		.pNext = nullptr,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0
	};
	return vertexInputInfo;
}

vk::PipelineInputAssemblyStateCreateInfo Lullaby::PipelineBuilder::defaultInputAssemblyInfo(const vk::PrimitiveTopology topology) {
	const vk::PipelineInputAssemblyStateCreateInfo info{
		.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo,
		.pNext = nullptr,
		.topology = topology,
		.primitiveRestartEnable = VK_FALSE
	};

	return info;
}

vk::PipelineRasterizationStateCreateInfo Lullaby::PipelineBuilder::defaultRasterizationInfo(const vk::PolygonMode polygonMode) {
	const vk::PipelineRasterizationStateCreateInfo info{
		.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo,
		.pNext = nullptr,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = polygonMode,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eCounterClockwise,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = .0f,
		.depthBiasClamp = .0f,
		.depthBiasSlopeFactor = .0f,
		.lineWidth = 1.0f
	};
	return info;
}

vk::PipelineMultisampleStateCreateInfo Lullaby::PipelineBuilder::defaultMultisampleInfo() {
	const vk::PipelineMultisampleStateCreateInfo info = {
		.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo,
		.pNext = nullptr,
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};
	return info;
}

vk::PipelineColorBlendAttachmentState Lullaby::PipelineBuilder::defaultColorBlendState() {
	const vk::PipelineColorBlendAttachmentState info{
		.blendEnable = VK_FALSE,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};
	return info;
}

vk::PipelineLayoutCreateInfo Lullaby::PipelineBuilder::defaultLayoutInfo() {
	const vk::PipelineLayoutCreateInfo info{
		.sType = vk::StructureType::ePipelineLayoutCreateInfo,
		.pNext = nullptr,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};
	return info;
}
