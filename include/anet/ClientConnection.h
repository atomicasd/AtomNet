#pragma once
#include "Packet.h"
#include <memory>
#include <functional>
#include <anet/ConnectionTypes.h>
#include <anet/impl/ClientNetworkInterface.h>
namespace anet
{
	class Process;
	class ClientConnection
	{
	public:
		ClientConnection(std::shared_ptr<ClientNetworkInterface> implementation);

		void SetHost(char* ip, unsigned short port);

		void Connect();

		void Disconnect();

		void AddProcess(int type, std::shared_ptr<Process> process);

		void RemoveProcess(int type);

		void SendPacket(std::shared_ptr<Packet> packet);

		void ReceivePackets();

		void RegisterConnectionCallback(std::function<void(bool)> onConnected);

		void RemoveConnectionCallback(std::function<void(bool)> onConnected);

	private:
		class Impl;
		Impl* pImpl;
	};
}