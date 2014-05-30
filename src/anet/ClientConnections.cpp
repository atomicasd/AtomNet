#include <anet/ClientConnections.h>
#include <anet/impl/IServerNetwork.h>
#include <anet/PacketInfo.h>
#include <anet/Client.h>

#include <boost/signals2.hpp>

#include <thread>
#include <future>
#include <unordered_map>

using namespace anet;

class ClientConnections::Impl
{
public:
	Impl();

	FuncClientSetup onCreatedClient_;

	std::shared_ptr<IServerNetwork> net_;

	std::unordered_map<short, std::shared_ptr<Client>> connectedClients_;
	boost::signals2::signal<void(std::shared_ptr<Client>)> sigClientCreated_;
	boost::signals2::signal<void(std::shared_ptr<Client>)> sigClientRemoved_;
};

ClientConnections::Impl::Impl()
{

}

//---

ClientConnections::ClientConnections(
	FuncClientSetup onCreatedClient, 
	std::shared_ptr<IServerNetwork> implementation) :
	pImpl(new Impl())
{
	pImpl->onCreatedClient_ = onCreatedClient;

	pImpl->net_ = implementation;
}

ClientConnections::~ClientConnections()
{

}

void ClientConnections::Update()
{
	if(pImpl->net_->NewOpenSockets())
	{
		for(short id : pImpl->net_->GetNewOpenSockets())
		{
			std::shared_ptr<Client> client(new Client(id));
			pImpl->connectedClients_[id] = pImpl->onCreatedClient_(client);
			pImpl->sigClientCreated_(client);
		}
	}

	if(pImpl->net_->DisconnectedSockets())
	{
		for(short id : pImpl->net_->GetDisconnectedSockets())
		{
			auto it = pImpl->connectedClients_.find(id);
			if (it != pImpl->connectedClients_.end())
			{
				pImpl->sigClientRemoved_(it->second);
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
			pImpl->connectedClients_[pInfo->id]->HandlePacket(pInfo->packet);
		}
	}

	packets->clear();
}

void ClientConnections::SendPacket(std::shared_ptr<PacketInfo> pInfo)
{
	pImpl->net_->SendPacket(pInfo);
}

void ClientConnections::OnClientCreated(FuncClient onClientCreated)
{
	pImpl->sigClientCreated_.connect(onClientCreated);
}

void ClientConnections::OnClientRemoved(FuncClient onClientRemoved)
{
	pImpl->sigClientRemoved_.connect(onClientRemoved);
}
