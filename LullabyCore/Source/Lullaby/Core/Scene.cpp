#include "LullabyPch.h"
#include "Scene.h"

#include "Lullaby/ECS/Entity.h"

namespace Lullaby {
	Entity Scene::createEntity() const {
		const auto newEntity = _world.entity();
		return { newEntity };
	}
}