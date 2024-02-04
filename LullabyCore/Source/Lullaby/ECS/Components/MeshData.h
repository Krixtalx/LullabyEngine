#pragma once
namespace Lullaby {
	struct MeshData {
		std::vector<vec3> vertex;
		std::vector<uint32> index;
		std::vector<vec3> normals;
		std::vector<vec2> uv;
	};
}