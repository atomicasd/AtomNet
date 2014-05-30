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
	typedef std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> FuncClientSetup;

	typedef std::function<void(unsigned short id)> ClientConnectedCallback;
	typedef std::function<void(unsigned short id)> ClientDisconnectedCallback;

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

		ClientConnections(FuncClientSetup onCreatedClient, std::shared_ptr<IServerNetwork> implementation);

		virtual ~ClientConnections();

		void Update();

		void SendPacket(std::shared_ptr<PacketInfo> pInfo);

		std::shared_ptr<FunctionToken> OnClientConnected(ClientConnectedCallback callback);
		std::shared_ptr<FunctionToken> OnClientDisconnected(ClientDisconnectedCallback callback);

	private:
		void UnRegisterEvent(int type, unsigned short id);

		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};