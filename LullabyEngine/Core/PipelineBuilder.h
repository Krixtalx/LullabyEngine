#pragma once
#include <unordered_map>

namespace Lullaby {

	class PipelineBuilder : public Singleton<PipelineBuilder> {
		friend Singleton;

	private:
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
	public:
		virtual ~PipelineBuilder() = default;

		static VkShaderModule* loadShaderModule(const VkDevice& device, const std::string& path);
	};
}