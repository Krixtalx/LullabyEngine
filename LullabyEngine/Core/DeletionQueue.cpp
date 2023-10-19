#include "stdafx.h"
#include "DeletionQueue.h"

Lullaby::DeletionQueue::~DeletionQueue() {
	flush();
}

void Lullaby::DeletionQueue::addDeletor(std::function<void()>&& function) {
	_deletors.push_back(function);
}

void Lullaby::DeletionQueue::flush() {
	// reverse iterate the deletion queue to execute all the functions
	for (auto it = _deletors.rbegin(); it != _deletors.rend(); ++it) {
		(*it)(); //call the function
	}

	_deletors.clear();
}
