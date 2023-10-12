#pragma once

namespace Lullaby {

	class PipelineBuilder {


	public:
		std::vector<VkPipelineShaderStageCreateInfo>	_shaderStages;
		VkPipelineVertexInputStateCreateInfo			_vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo			_inputAssembly;
		VkViewport										_viewport;
		VkRect2D										_scissor;
		VkPipelineRasterizationStateCreateInfo			_rasterizer;
		VkPipelineColorBlendAttachmentState				_colorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo			_multisampling;
		VkPipelineLayout								_pipelineLayout;

		PipelineBuilder() = default;
		virtual ~PipelineBuilder() = default;

		VkPipeline buildPipeline(VkDevice& device, VkRenderPass& renderPass);

		static VkShaderModule* loadShaderModule(const VkDevice& device, const std::string& path);
		static VkPipelineVertexInputStateCreateInfo defaultVertexInputInfo();
	};
}