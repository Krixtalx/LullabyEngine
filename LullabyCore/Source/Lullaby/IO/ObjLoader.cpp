#include "LullabyPch.h"
#include "ObjLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Lullaby/ECS/Components/MeshData.h"

namespace Lullaby {
	void ObjLoader::parse(const std::string& pathToFile) {
		//TODO: Add meshoptimizer, properly load objs

		//attrib will contain the vertex arrays of the file
		tinyobj::attrib_t attrib;
		//shapes contains the info for each separate object in the file
		std::vector<tinyobj::shape_t> shapes;
		//materials contains the information about the material of each shape, but we won't use it.
		std::vector<tinyobj::material_t> materials;

		//error and warning output from the load function
		std::string err;

		//load the OBJ file
		bool ret = LoadObj(&attrib, &shapes, &materials, &err, pathToFile.c_str(), nullptr);

		//if we have any error, print it to the console, and break the mesh loading.
		//This happens if the file can't be found or is malformed
		if (!err.empty()) {
			if (err.substr(0, 4) == "WARN")
				print(fg(fmt::color::yellow), "{}\n", err);
			else
				print(fg(fmt::color::red), "{}\n", err);
		}

		if (ret) {
			MeshData meshData;
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
						const tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

						//vertex position
						tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
						tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
						tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
						//vertex normal
						tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
						tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
						tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

						meshData.vertex.emplace_back(vx, vy, vz);
						meshData.normals.emplace_back(nx, ny, nz);
					}
					index_offset += fv;
				}
			}
		}
	}
}
