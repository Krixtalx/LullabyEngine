#include "LullabyPch.h"
#include "WorldManager.h"

#include "Lullaby/ECS/Entity.h"

namespace Lullaby {
	Entity WorldManager::createEntity() const {
		const auto newEntity = _worlds[_currentWorld].entity();
		return { newEntity };
	}
}