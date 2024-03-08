#include "LullabyPch.h"
#include "WorldManager.h"

#include "Lullaby/ECS/Components/Transform.h"

namespace Lullaby {
	WorldManager::WorldManager() {
		_worlds.emplace_back();
		_currentWorld = 0;
	}

	flecs::entity WorldManager::createEntity() const {
		auto entity = _worlds[_currentWorld].entity();
		entity.add<Transform>();
		return entity;
	}

	flecs::world& WorldManager::getWorld() {
		if (!_worlds.empty())
			return _worlds[_currentWorld];
	}
}
