#include "LullabyPch.h"
#include "Mesh.h"

Lullaby::VertexInputDescription Lullaby::Vertex::getVertexDescription() {
	VertexInputDescription description;

	//we will have just 1 vertex buffer binding, with a per-vertex rate
	constexpr vk::VertexInputBindingDescription mainBinding = {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = vk::VertexInputRate::eVertex
	};

	description.bindings.push_back(mainBinding);

	const vk::VertexInputAttributeDescription positionAttribute = {
		.location = 0,
		.binding = 0,
		.format = vk::Format::eR32G32B32Sfloat,
		.offset = offsetof(Vertex, _position)
	};
	description.attributes.push_back(positionAttribute);

	const vk::VertexInputAttributeDescription normalAttribute = {
		.location = 1,
		.binding = 0,
		.format = vk::Format::eR32G32B32Sfloat,
		.offset = offsetof(Vertex, _normal)
	};
	description.attributes.push_back(normalAttribute);

	const vk::VertexInputAttributeDescription colorAttribute = {
		.location = 2,
		.binding = 0,
		.format = vk::Format::eR32G32B32Sfloat,
		.offset = offsetof(Vertex, _color)
	};
	description.attributes.push_back(colorAttribute);

	return description;
}