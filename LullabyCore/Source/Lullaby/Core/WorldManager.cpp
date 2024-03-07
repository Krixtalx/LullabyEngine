#include "LullabyPch.h"
#include "WorldManager.h"

namespace Lullaby {
	WorldManager::WorldManager() {
		_worlds.emplace_back();
		_currentWorld = 0;
	}


	flecs::entity WorldManager::createEntity() const {
		return _worlds[_currentWorld].entity();
	}

	flecs::world& WorldManager::getWorld() {
		if (!_worlds.empty())
			return _worlds[_currentWorld];
	}
}
