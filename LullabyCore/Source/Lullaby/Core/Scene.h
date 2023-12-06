#pragma once
#include "entt/entt.hpp"
namespace Lullaby {
	class Renderer;
	class Entity;

	class Scene {
		friend class Entity;
		friend class Renderer;
		entt::registry _registry;

	public:
		Scene() = default;
		Entity createEntity();
	};
}
