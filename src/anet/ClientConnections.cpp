#include <anet/ClientConnections.h>
#include <anet/impl/IServerNetwork.h>
#include <anet/PacketInfo.h>
#include <anet/Client.h>

#include <thread>
#include <future>

#include <map>
#include <unordered_map>

using namespace anet;

typedef std::function<void(unsigned short id)> ClientConnectedCallback;
typedef std::function<void(unsigned short id)> ClientDisconnectedCallback;

class ClientConnections::Impl
{
public:
	Impl();

	std::shared_ptr<IServerNetwork> net_;

	std::unordered_map<unsigned short, std::shared_ptr<Client>> connectedClients_;

	std::map<unsigned short, ClientConnectedCallback> clientConnectedEvents_;

	std::map<unsigned short, ClientDisconnectedCallback> clientDisconnectedEvents_;

	unsigned short functionCounter_;

	std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient_;
	
};

ClientConnections::Impl::Impl()
{

}

//---

ClientConnections::ClientConnections(
	std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient,
	std::shared_ptr<IServerNetwork> implementation) :
	pImpl(new Impl())
{
	pImpl->onCreatedClient_ = onCreatedClient;

	pImpl->net_ = implementation;

	pImpl->functionCounter_ = 0;
}

ClientConnections::~ClientConnections()
{

}

void ClientConnections::Update()
{
	if(pImpl->net_->HasNewSockets())
	{
		for(short id : pImpl->net_->GetNewSockets())
		{
			std::shared_ptr<Client> client(new Client(id, this));

			pImpl->connectedClients_[id] = pImpl->onCreatedClient_(client);

			for (auto func : pImpl->clientConnectedEvents_)
			{
				func.second(id);
			}
		}
	}

	if(pImpl->net_->HasRemovedSockets())
	{
		for(short id : pImpl->net_->GetRemovedSockets())
		{
			auto it = pImpl->connectedClients_.find(id);

			if (it != pImpl->connectedClients_.end())
			{
				for (auto func : pImpl->clientDisconnectedEvents_)
				{
					func.second(id);
				}

				pImpl->connectedClients_.erase(it);
			}
		}
	}

	auto packets = pImpl->net_->GetPackets();

	for (auto it = packets->begin(); it != packets->end(); it++)
	{
		PacketInfo* pInfo = it->get();

		if (pImpl->connectedClients_.find(pInfo->id) != pImpl->connectedClients_.end())
		{
			pImpl->connectedClients_[pInfo->id]->HandlePacket(*pInfo->packet);
		}
	}

	packets->clear();
}

void ClientConnections::SendPacket(std::shared_ptr<PacketInfo> pInfo)
{
	pImpl->net_->SendPacket(pInfo);
}

std::shared_ptr<ClientConnections::FunctionToken> ClientConnections::OnClientConnected(ClientConnectedCallback callback)
{
	std::shared_ptr<FunctionToken> token(new FunctionToken(Events::CLIENT_CONNECTED, pImpl->functionCounter_, this));

	pImpl->clientConnectedEvents_[pImpl->functionCounter_] = callback;

	pImpl->functionCounter_++;

	return token;
}

std::shared_ptr<ClientConnections::FunctionToken> ClientConnections::OnClientDisconnected(ClientConnectedCallback callback)
{
	std::shared_ptr<FunctionToken> token(new FunctionToken(Events::CLIENT_DISCONNECTED, pImpl->functionCounter_, this));

	pImpl->clientDisconnectedEvents_[pImpl->functionCounter_] = callback;

	pImpl->functionCounter_++;

	return token;
}

void ClientConnections::UnRegisterEvent(int type, unsigned short id)
{
	switch(type)
	{
		case Events::CLIENT_CONNECTED:
		{
			auto it = pImpl->clientConnectedEvents_.find(id);

			if (it != pImpl->clientConnectedEvents_.end())
			{
				pImpl->clientConnectedEvents_.erase(it);
			}

			break;
		}
		case Events::CLIENT_DISCONNECTED:
		{
			auto it = pImpl->clientDisconnectedEvents_.find(id);

			if (it != pImpl->clientDisconnectedEvents_.end())
			{
				pImpl->clientDisconnectedEvents_.erase(it);
			}

			break;
		}

		default:
		break;

	}
}
