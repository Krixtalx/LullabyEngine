#include "LullabyPch.h"
#include "WorldManager.h"

#include "Lullaby/ECS/Components/Transform.h"

namespace Lullaby {
	WorldManager::WorldManager() {
	}

	flecs::entity WorldManager::createEntity() const {
		auto entity = gameWorld.entity();
		entity.add<Transform>();
		return entity;
	}

	flecs::entity WorldManager::createEditorEntity() const {
		auto entity = editorWorld.entity();
		return entity;
	}
}
