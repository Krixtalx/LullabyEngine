#include "stdafx.h"
#include "Mesh.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

Lullaby::VertexInputDescription Lullaby::Vertex::getVertexDescription() {
	VertexInputDescription description;

	//we will have just 1 vertex buffer binding, with a per-vertex rate
	constexpr VkVertexInputBindingDescription mainBinding = {
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	description._bindings.push_back(mainBinding);

	const VkVertexInputAttributeDescription positionAttribute = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, _position)
	};
	description._attributes.push_back(positionAttribute);

	const VkVertexInputAttributeDescription normalAttibute = {
		.location = 1,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, _normal)
	};
	description._attributes.push_back(normalAttibute);

	const VkVertexInputAttributeDescription colorAttribute = {
		.location = 2,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, _color)
	};
	description._attributes.push_back(colorAttribute);

	return description;
}

bool Lullaby::Mesh::loadFromObj(const std::string& filename) {
	//attrib will contain the vertex arrays of the file
	tinyobj::attrib_t attrib;
	//shapes contains the info for each separate object in the file
	std::vector<tinyobj::shape_t> shapes;
	//materials contains the information about the material of each shape, but we won't use it.
	std::vector<tinyobj::material_t> materials;

	//error and warning output from the load function
	std::string err;

	//load the OBJ file
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), nullptr);

	//if we have any error, print it to the console, and break the mesh loading.
	//This happens if the file can't be found or is malformed
	if (!err.empty()) {
		if (err.substr(0, 4) == "WARN")
			fmt::print(fg(fmt::color::yellow), "{}\n", err);
		else
			fmt::print(fg(fmt::color::red), "{}\n", err);
	}

	if (!ret)
		return false;

	// Loop over shapes
	for (auto& shape : shapes) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

			//hardcode loading to triangles
			int fv = 3;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

				//vertex position
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				//vertex normal
				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

				//copy it into our vertex
				Vertex new_vert;
				new_vert._position.x = vx;
				new_vert._position.y = vy;
				new_vert._position.z = vz;

				new_vert._normal.x = nx;
				new_vert._normal.y = ny;
				new_vert._normal.z = nz;

				//we are setting the vertex color as the vertex normal. This is just for display purposes
				new_vert._color = new_vert._normal;

				_vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}

	return true;
}
