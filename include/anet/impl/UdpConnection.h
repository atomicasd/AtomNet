#pragma once

#include <anet/impl/ClientNetworkInterface.h>
namespace anet
{
	class UdpConnection : public ClientNetworkInterface
	{
	public:
		UdpConnection(std::function<void(bool)> clientConnectionCallbackResult);
		virtual ~UdpConnection();

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