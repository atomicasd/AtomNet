#pragma once
#include "Packet.h"
#include <memory>
#include <functional>
#include <anet/ConnectionTypes.h>
namespace anet
{
	class ClientConnection
	{
	public:
		ClientConnection(ConnectionType type);

		void SetHost(char* ip, unsigned short port);
		void Connect();
		void Disconnect();

		void SendPacket(std::shared_ptr<Packet> packet);
		void ReceivePackets();

		void RegisterConnectionCallback(std::function<void(bool)> onConnected);
		void RemoveConnectionCallback(std::function<void(bool)> onConnected);

	private:
		class Impl;
		Impl* pImpl;
	};
}