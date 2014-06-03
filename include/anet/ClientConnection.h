#pragma once

#include <anet/Process.h>
#include <anet/Packet.h>
#include <anet/ConnectionTypes.h>
#include <anet/impl/IClientNetwork.h>

#include <memory>
#include <functional>

namespace anet
{
	class ClientConnection
	{
	public:
		ClientConnection(std::shared_ptr<IClientNetwork> implementation);
		virtual ~ClientConnection();

		void SetHost(char* ip, unsigned short port);

		void Connect();

		void Disconnect();

		void AddProcess(std::shared_ptr<Process> process);

		void RemoveProcess(int type);

		void SendPacket(std::shared_ptr<Packet> packet);

		void Update();

	private:
		class Impl;
		//std::unique_ptr<Impl> pImpl //Compile error on MSVC
		std::shared_ptr<Impl> pImpl;
	};
}