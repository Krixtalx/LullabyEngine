#pragma once
namespace Lullaby {
	struct PipelineInfo {
		std::vector<vk::PipelineShaderStageCreateInfo>	_shaderStages;
		vk::PipelineVertexInputStateCreateInfo			_vertexInputInfo;
		vk::PipelineInputAssemblyStateCreateInfo		_inputAssembly;
		vk::Viewport									_viewport;
		vk::Rect2D										_scissor;
		vk::PipelineRasterizationStateCreateInfo		_rasterizer;
		vk::PipelineColorBlendAttachmentState			_colorBlendAttachment;
		vk::PipelineDepthStencilStateCreateInfo			_depthStencil;
		vk::PipelineMultisampleStateCreateInfo			_multisampling;
		vk::PipelineLayout								_pipelineLayout;
	};

	class PipelineBuilder final {
		PipelineBuilder();
		~PipelineBuilder();

	public:
		static vk::Pipeline buildPipeline(PipelineInfo& pipelineInfo, const vk::Device& device, const vk::RenderPass& renderPass);

		static vk::ShaderModule loadShaderModule(const vk::Device& device, const std::string& path);
		static vk::PipelineShaderStageCreateInfo defaultShaderStageInfo(vk::ShaderStageFlagBits stage, vk::ShaderModule shaderModule);
		static vk::PipelineVertexInputStateCreateInfo defaultVertexInputInfo();
		static vk::PipelineInputAssemblyStateCreateInfo defaultInputAssemblyInfo(vk::PrimitiveTopology topology);
		static vk::PipelineRasterizationStateCreateInfo defaultRasterizationInfo(vk::PolygonMode polygonMode);
		static vk::PipelineMultisampleStateCreateInfo defaultMultisampleInfo();
		static vk::PipelineColorBlendAttachmentState defaultColorBlendState();
		static vk::PipelineLayoutCreateInfo defaultLayoutInfo();
	};
}