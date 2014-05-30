#pragma once

#include <anet/impl/IClientNetwork.h>
#include <functional>

namespace anet
{
	class ClientTcp : public IClientNetwork
	{
	public:
		ClientTcp(std::function<void(bool)> clientConnectionCallbackResult);
		virtual ~ClientTcp();

		void SetHost(char* ip, unsigned short port);

		void Connect();

		void Disconnect();

		bool IsConnected();

		void SendPacket(std::shared_ptr<Packet> packet);

		void ReceivePackets();

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
}