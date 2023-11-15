#pragma once

namespace Lullaby {
	struct PipelineInfo {
		std::vector<VkPipelineShaderStageCreateInfo>	_shaderStages;
		VkPipelineVertexInputStateCreateInfo			_vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo			_inputAssembly;
		VkViewport										_viewport;
		VkRect2D										_scissor;
		VkPipelineRasterizationStateCreateInfo			_rasterizer;
		VkPipelineColorBlendAttachmentState				_colorBlendAttachment;
		VkPipelineDepthStencilStateCreateInfo			_depthStencil;
		VkPipelineMultisampleStateCreateInfo			_multisampling;
		VkPipelineLayout								_pipelineLayout;
	};

	class PipelineBuilder final {
		PipelineBuilder();
		~PipelineBuilder();

	public:
		static VkPipeline buildPipeline(PipelineInfo& pipelineInfo, const VkDevice& device, const VkRenderPass& renderPass);

		static VkShaderModule loadShaderModule(const VkDevice& device, const std::string& path);
		static VkPipelineShaderStageCreateInfo defaultShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule);
		static VkPipelineVertexInputStateCreateInfo defaultVertexInputInfo();
		static VkPipelineInputAssemblyStateCreateInfo defaultInputAssemblyInfo(VkPrimitiveTopology topology);
		static VkPipelineRasterizationStateCreateInfo defaultRasterizationInfo(VkPolygonMode polygonMode);
		static VkPipelineMultisampleStateCreateInfo defaultMultisampleInfo();
		static VkPipelineColorBlendAttachmentState defaultColorBlendState();
		static VkPipelineLayoutCreateInfo defaultLayoutInfo();
	};
}