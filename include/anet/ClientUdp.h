#pragma once

#include <anet/impl/IClientNetwork.h>
#include <functional>

namespace anet
{
	class ClientUdp : public IClientNetwork
	{
	public:
		ClientUdp(std::function<void(bool)> clientConnectionCallbackResult);
		virtual ~ClientUdp();

		void SetHost(char* ip, unsigned short port);

		void Connect();

		void Disconnect();

		bool IsConnected();

		void SendPacket(Packet& packet);

		void ReceivePackets();

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
}