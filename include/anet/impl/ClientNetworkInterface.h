#pragma once

#include <vector>
#include <memory>
#include "anet/Packet.h"
#include <functional>
namespace anet
{
	class ClientNetworkInterface
	{
	public:
		ClientNetworkInterface() {};
		virtual ~ClientNetworkInterface() {};

		virtual void SetHost(char* ip, unsigned short port) = 0;
		virtual void Connect() = 0;
		virtual void Disconnect() = 0;
		virtual bool IsConnected() = 0;

		virtual void SendPacket(std::shared_ptr<Packet> packet) = 0;
		virtual void ReceivePackets() = 0;

		std::vector<std::shared_ptr<Packet>> packets;

	private:
		class Impl;
		Impl* pImpl;
	};
}