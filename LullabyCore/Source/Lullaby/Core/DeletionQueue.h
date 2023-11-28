#pragma once
#include <deque>
#include <functional>

namespace Lullaby {
	class DeletionQueue {
		std::deque<std::function<void()>> _deletors;

	public:
		DeletionQueue() = default;
		~DeletionQueue();

		void addDeletor(std::function<void()>&& function);
		void flush();
	};
}