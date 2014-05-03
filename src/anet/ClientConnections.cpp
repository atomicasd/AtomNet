#include "anet/ClientConnections.h"
#include "ServerUDPConnections.h"
#include "ServerTCPConnections.h"
#include "anet/PacketInfo.h"
#include "anet/Client.h"
#include <boost/signals2.hpp>

#include <thread>
#include <future>
#include <unordered_map>

#include "UPNPService.h"

using namespace anet;

class ClientConnections::Impl
{
public:
	Impl(unsigned short port, std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient, ConnectionType connectionType);
public:
	std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient_;

	std::shared_ptr<ServerNetworkInterface> net_;
	std::thread *netThread_;

	std::unordered_map<short, std::shared_ptr<Client>> connectedClients_;
	boost::signals2::signal<void(std::shared_ptr<Client>)> sigClientCreated_;
	boost::signals2::signal<void(std::shared_ptr<Client>)> sigClientRemoved_;


	UPNPService *upnp_;
};

ClientConnections::Impl::Impl(unsigned short port, std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient, ConnectionType connectionType) :
onCreatedClient_(onCreatedClient)
{
	if (connectionType == ConnectionType::TCP)
	{
		net_.reset(new ServerTCPConnections(port));
	}
	else if (connectionType == ConnectionType::UDP)
	{
		net_.reset(new ServerUDPConnections(port));
	}

	netThread_ = new std::thread(&ServerNetworkInterface::Run, net_);

	upnp_ = new UPNPService(9000, ConnectionType::UDP);

	if (upnp_->Open() == UPNPResult::UPNP_SUCCESS)
		printf("UPNP SUCCESS\n");
}

//---

ClientConnections::ClientConnections(unsigned short port, std::function<std::shared_ptr<Client>(std::shared_ptr<Client>)> onCreatedClient, ConnectionType connectionType) : 
pImpl(new Impl(port, onCreatedClient, connectionType))
{

}

ClientConnections::~ClientConnections()
{
	delete pImpl;
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
			printf("New client made\n");
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
				printf("Client removed\n");
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

void ClientConnections::OnClientCreated(std::function<void(std::shared_ptr<Client>)> onClientCreated)
{
	pImpl->sigClientCreated_.connect(onClientCreated);
}

void ClientConnections::OnClientRemoved(std::function<void(std::shared_ptr<Client>)> onClientRemoved)
{
	pImpl->sigClientRemoved_.connect(onClientRemoved);
}
