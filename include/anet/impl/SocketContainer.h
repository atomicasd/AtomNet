#pragma once
#include <vector>
#include <map>
#include <atomic>
#include <mutex>

template<class T>
class SocketContainer
{
public:

	T* GetSocket(unsigned short id)
	{
		std::lock_guard<std::mutex> lock(socketMutex_);

		auto it = sockets_.find(id);

		if (it != sockets_.end())
			return &it->second;

		return nullptr;
	}

	unsigned short GetId(T socket)
	{
		std::lock_guard<std::mutex> lock(socketMutex_);

		auto it = ids_.find(socket);
		
		if (it != ids_.end())
			return it->second;

		return -1;
	}

	unsigned short AddSocket(T socket)
	{
		std::lock_guard<std::mutex> lock(socketMutex_);

		short id = 0;

		for (auto it = sockets_.begin(); it != sockets_.end(); it++)
		{
			if (it->first == id)
			{
				id++;
				continue;
			}

			break;
		}

		sockets_[id] = socket;
		ids_[socket] = id;

		newSockets_.push_back(id);
		hasNewSockets_ = true;

		return id;
	}

	void RemoveSocket(T socket)
	{
		std::lock_guard<std::mutex> lock(socketMutex_);

		for (auto it = sockets_.begin(); it != sockets_.end(); it++)
		{
			if (it->second == socket)
			{
				short id = it->first;
				sockets_.erase(it);

				ids_.erase(socket);

				removedSockets_.push_back(id);
				hasRemovedSockets_ = true;

				return;
			}
		}

		return;
	}

	bool HasNewSockets()
	{
		return hasNewSockets_;
	}

	bool HasRemovedSockets()
	{
		return hasRemovedSockets_;
	}

	std::vector<unsigned short> GetNewSockets()
	{
		std::lock_guard<std::mutex> lock(socketMutex_);

		std::vector<unsigned short> vec = newSockets_;
		newSockets_.clear();
		hasNewSockets_ = false;

		return vec;
	}

	std::vector<unsigned short> GetRemovedSockets()
	{
		std::lock_guard<std::mutex> lock(socketMutex_);

		std::vector<unsigned short> vec = removedSockets_;
		removedSockets_.clear();
		hasRemovedSockets_ = false;

		return vec;
	}

private:
	std::mutex socketMutex_;

	std::map<unsigned short, T> sockets_;
	std::map<T, unsigned short> ids_;

	std::vector<unsigned short> newSockets_;
	std::vector<unsigned short> removedSockets_;

	std::atomic<bool> hasNewSockets_;
	std::atomic<bool> hasRemovedSockets_;
};