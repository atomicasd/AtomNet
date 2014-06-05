#pragma once

#include <memory>

namespace anet
{
	class Process;
	class Packet;
	class ClientConnections;
	class Client
	{
	public:
		Client(short id, anet::ClientConnections* connections);
		virtual ~Client();

		void AddProcess(std::shared_ptr<Process> process);

		void RemoveProcess(int type);

		void HandlePacket(Packet& packet);

		void SendPacket(Packet& packet);

		short GetId() const;
	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};