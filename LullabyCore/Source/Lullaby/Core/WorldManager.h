#pragma once
#include <flecs.h>

#include "Lullaby/ECS/Components/MeshData.h"
#include "Lullaby/Utilities/Singleton.h"

namespace Lullaby {
	class Renderer;
	class Entity;

	class WorldManager : public Singleton<WorldManager> {
		friend class Renderer;
		std::vector<flecs::world> _worlds;
		u16 _currentWorld;

		flecs::query<MeshData> _meshQuery;
	public:
		WorldManager();
		WorldManager(const WorldManager&) = delete;
		flecs::entity createEntity() const;
		flecs::world& getWorld();
	};
}
