#pragma once
#include <flecs.h>
namespace Lullaby {
	class Renderer;
	class Entity;

	class Scene {
		friend class Entity;
		friend class Renderer;
		flecs::world _world;

	public:
		Scene() = default;
		Entity createEntity() const;
	};
}
