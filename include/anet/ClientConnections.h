#pragma once
#include <functional>
#include <memory>
#include <anet/Client.h>
#include <anet/ConnectionTypes.h>
#include <anet/impl/IServerNetwork.h>

namespace anet
{
	typedef std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> FuncClientSetup;
	typedef std::function<void(std::shared_ptr<Client>)> FuncClient;

	class PacketInfo;
	class ClientConnections
	{
	public:

		ClientConnections(FuncClientSetup onCreatedClient, std::shared_ptr<IServerNetwork> implementation);

		virtual ~ClientConnections();

		void ProcessClients();

		void SendPacket(PacketInfo* pInfo);

		void OnClientCreated(FuncClient onClientCreated);

		void OnClientRemoved(FuncClient onClientRemoved);

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};