#pragma once
#include <functional>
#include <memory>
#include <anet/Client.h>
#include <anet/ConnectionTypes.h>
#include <anet/IpPort.h>

namespace anet
{
	class PacketInfo;
	class ClientConnections
	{
	public:

		ClientConnections(unsigned short port, std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient, ConnectionType connectionType = ConnectionType::TCP);

		ClientConnections(std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient, IpPort& rendevouzServer);

		virtual ~ClientConnections();

		void ProcessClients();
		void SendPacket(PacketInfo* pInfo);

		void OnClientCreated(std::function<void(std::shared_ptr<Client>)> onClientCreated);
		void OnClientRemoved(std::function<void(std::shared_ptr<Client>)> onClientRemoved);

	private:
		class Impl;
		Impl* pImpl;
	};
};