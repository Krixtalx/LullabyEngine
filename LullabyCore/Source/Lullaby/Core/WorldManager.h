#pragma once
#include <flecs.h>

#include "Lullaby/ECS/Components/MeshData.h"
#include "Lullaby/Utilities/Singleton.h"

namespace Lullaby {
	class Renderer;
	class Entity;

	class WorldManager : public Singleton<WorldManager> {
		friend class Renderer;
		flecs::world gameWorld;
		flecs::world editorWorld;

	public:
		WorldManager();
		WorldManager(const WorldManager&) = delete;
		flecs::entity createEntity() const;
		flecs::entity createEditorEntity() const;

		flecs::world& getGameWorld() { return gameWorld; }
		flecs::world& getEditorWorld() { return editorWorld; }
	};
}
