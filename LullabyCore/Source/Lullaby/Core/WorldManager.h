#pragma once
#include <flecs.h>

namespace Lullaby {
	class Renderer;
	class Entity;

	class WorldManager {
		friend class Renderer;
		std::vector<flecs::world> _worlds;
		uint16 _currentWorld;

	public:
		WorldManager() = default;
		Entity createEntity() const;

	};
}
