#pragma once

#include <memory>

namespace anet
{
	class Process;
	class Packet;
	class Client
	{
	public:
		Client(short id);
		virtual ~Client();

		void AttachProcess(int processType, std::shared_ptr<Process> process);
		void HandlePacket(Packet& packet);
		short GetId();
	private:
		class Impl;
		Impl* pImpl;
	};
};