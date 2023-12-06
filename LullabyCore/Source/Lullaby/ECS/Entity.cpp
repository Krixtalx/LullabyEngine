#include "LullabyPch.h"
#include "Entity.h"

namespace Lullaby {
	Entity::Entity(const entt::entity id, Scene* sourceScene) :
		_entityId(id), _scene(sourceScene) {
	}
}
