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

		void AddProcess(int type, std::shared_ptr<Process> process);
		void Removeprocess(int type);

		void HandlePacket(Packet& packet);

		short GetId() const;
	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};