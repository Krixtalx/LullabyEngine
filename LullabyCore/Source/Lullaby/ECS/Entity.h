#pragma once
#include "Lullaby/Core/Scene.h"

namespace Lullaby {
	class Entity {
		entt::entity _entityId;
		Scene* _scene;

	public:
		Entity() = default;
		Entity(entt::entity id, Scene* sourceScene);

		template<typename... T>
		bool hasComponent() const {
			return _scene->_registry.all_of<T>(_entityId);
		}

		template<typename T, typename... Args>
		void addComponent(Args&&... args) const {
			if (!hasComponent<T>())
				_scene->_registry.emplace<T>(_entityId, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent() const {
			return _scene->_registry.get<T>(_entityId);
		}

		template<typename T>
		void removeComponent() const {
			_scene->_registry.remove<T>(_entityId);
		}

		operator bool() const { return _entityId != entt::null; }
		operator entt::entity() const { return _entityId; }
		operator uint32_t() const { return (uint32_t)_entityId; }

		bool operator==(const Entity& other) const {
			return _entityId == other._entityId && _scene == other._scene;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	};
}
