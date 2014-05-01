#pragma once
#include "Packet.h"
#include <memory>
#include <functional>
namespace anet
{
	class HybridConnection
	{
	public:

		HybridConnection(std::function<void(bool)> onServerConnectedResult);

		//void SetHost(char* ip, unsigned short port);
		//void Connect();

		//void SendPacket(std::shared_ptr<Packet> packet);
		//void ReceivePackets();

		//void RegisterConnectionCallback(std::function<void(bool)> onConnected);
		//void RemoveConnectionCallback(std::function<void(bool)> onConnected);

	private:
		class Impl;
		Impl* pImpl;
	};
}