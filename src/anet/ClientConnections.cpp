#include "anet/ClientConnections.h"
#include "anet/impl/ServerNetworkInterface.h"
#include "anet/PacketInfo.h"
#include "anet/Client.h"

#include <boost/signals2.hpp>

#include <thread>
#include <future>
#include <unordered_map>

using namespace anet;

class ClientConnections::Impl
{
public:
	Impl();
public:
	FuncClientSetup onCreatedClient_;

	std::shared_ptr<ServerNetworkInterface> net_;

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
	std::shared_ptr<ServerNetworkInterface> implementation) :
	pImpl(new Impl())
{
	pImpl->onCreatedClient_ = onCreatedClient;

	pImpl->net_ = implementation;
}

ClientConnections::~ClientConnections()
{
}

void ClientConnections::ProcessClients()
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
			std::unordered_map<short, std::shared_ptr<Client>>::iterator it = pImpl->connectedClients_.find(id);
			if (it != pImpl->connectedClients_.end())
			{
				pImpl->sigClientRemoved_(it->second);
				pImpl->connectedClients_.erase(it);
			}
		}
	}

	std::vector<PacketInfo*>& packets = *pImpl->net_->GetPackets();
	for(PacketInfo* pInfo : packets)
	{
		pImpl->connectedClients_[pInfo->id]->HandlePacket(pInfo->packet);
		delete pInfo;
	}
	packets.clear();
}

void ClientConnections::SendPacket(PacketInfo* pInfo)
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
