#pragma once
#include <memory>

template<typename T>
class Singleton {
protected:
	Singleton() = default;
	~Singleton() = default;

public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(Singleton) = delete;
	static std::unique_ptr<T> ptr;
	static T* getInstance();
};

// Static members initialization

template<typename T>
std::unique_ptr<T> Singleton<T>::ptr;

// Public methods

template<typename T>
T* Singleton<T>::getInstance() {
	if (!ptr.get()) {
		ptr = std::unique_ptr<T>(new T());
	}

	return ptr.get();
}

