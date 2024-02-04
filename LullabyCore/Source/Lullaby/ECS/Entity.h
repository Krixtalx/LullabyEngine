#pragma once
#include "Lullaby/Core/WorldManager.h"

namespace Lullaby {
	class Entity {
		flecs::entity _flecsEntity;

	public:
		Entity() = default;
		Entity(flecs::entity id);

		template<typename... T>
		bool hasComponent() const {
			return _flecsEntity.has<T>();
		}

		template<typename T>
		void addComponent() const {
			_flecsEntity.add<T>();
		}

		template<typename T>
		T* getComponent() const {
			return _flecsEntity.get<T>();
		}

		template<typename T>
		void removeComponent() const {
			_flecsEntity.remove<T>();
		}

		operator bool() const { return _flecsEntity.is_alive(); }
		operator flecs::entity() const { return _flecsEntity; }

		bool operator==(const Entity& other) const {
			return _flecsEntity == other._flecsEntity;
		}

		bool operator!=(const Entity& other) const {
			return !(*this == other);
		}
	};
}
