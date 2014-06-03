#pragma once

#include <functional>
#include <memory>
#include <anet/Client.h>
#include <anet/ConnectionTypes.h>
#include <anet/PacketInfo.h>
#include <anet/impl/IServerNetwork.h>

#include <iostream>

namespace anet
{
	typedef enum Events
	{
		CLIENT_CONNECTED,
		CLIENT_DISCONNECTED
	} Events;

	class ClientConnections
	{
	public:
		class FunctionToken
		{
		public:
			FunctionToken(int type, unsigned short id, ClientConnections* manager)
			{
				type_ = id;
				id_ = id;
				manager_ = manager;
			};

			~FunctionToken()
			{
				manager_->UnRegisterEvent(type_, id_);
			}

		private:
			int type_;
			unsigned short id_;
			ClientConnections* manager_;
		};

	public:

		ClientConnections(std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient, std::shared_ptr<IServerNetwork> implementation);

		virtual ~ClientConnections();

		void Update();

		void SendPacket(std::shared_ptr<PacketInfo> pInfo);

		std::shared_ptr<FunctionToken> OnClientConnected(std::function<void(unsigned short id)> callback);
		std::shared_ptr<FunctionToken> OnClientDisconnected(std::function<void(unsigned short id)> callback);

	private:
		void UnRegisterEvent(int type, unsigned short id);

		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};