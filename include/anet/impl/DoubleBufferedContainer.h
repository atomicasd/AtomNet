#pragma once
#include <vector>
#include <memory>
#include <mutex>

template<class T>
class DoubleBufferedContainer
{
public:
	DoubleBufferedContainer()
	{
		flag_ = false;
	}

	std::vector<T>* Get()
	{
		mutex_.lock();

		flag_ = !flag_;

		mutex_.unlock();

		return &container_[flag_];
	}

	void Add(T item)
	{
		mutex_.lock();

		container_[flag_].push_back(item);

		mutex_.unlock();
	}

private:
	bool flag_;
	std::mutex mutex_;
	std::vector<T> container_[2];
};