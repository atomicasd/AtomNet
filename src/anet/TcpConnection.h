#pragma once

#include "ClientNetworkInterface.h"
namespace anet
{
	class TcpConnection : public ClientNetworkInterface
	{
	public:
		TcpConnection(std::function<void(bool)> clientConnectionCallbackResult);
		virtual ~TcpConnection();

		void SetHost(char* ip, unsigned short port);
		void Connect();
		void Disconnect();
		bool IsConnected();

		void SendPacket(std::shared_ptr<Packet> packet);
		void ReceivePackets();

	private:
		class Impl;
		Impl* pImpl;
	};
}