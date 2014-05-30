#include <anet/impl/ServerUdp.h>
#include <anet/PacketInfo.h>

#include <iostream>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>

#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "UdpMessages.h"

#define FPS 1.0f/60.0f

using namespace anet;

class ServerUdp::Impl
{
public:
	Impl(unsigned short port);
	void Update();

public:
	RakNet::RakPeerInterface *peer_;

	bool hasNewOpenSockets;
	std::mutex newConMutex;
	std::vector<short> newOpenSockets;

	bool hasDisconnectedSockets;
	std::mutex dcmutex;
	std::vector<short> disconnectedSockets;

public:
	std::mutex inmutex;
	std::vector<std::shared_ptr<PacketInfo>> incoming[2];
	bool inflag;

	std::mutex outmutex;
	std::vector<std::shared_ptr<PacketInfo>> outgoing[2];
	bool outflag;

public:
	std::map<short, RakNet::SystemAddress> connections_;
	RakNet::SystemAddress* GetSocket(short id);
	short GetId(RakNet::SystemAddress);

	short AddSocketConnection(RakNet::SystemAddress socket);
	short RemoveSocketConnection(RakNet::SystemAddress socket);
};

ServerUdp::Impl::Impl(unsigned short port) :
	peer_(RakNet::RakPeerInterface::GetInstance())
{

	inflag = false;
	outflag = false;

	hasNewOpenSockets = false;
	hasDisconnectedSockets = false;
	RakNet::SocketDescriptor sd(port,0);
	peer_->Startup(35, &sd, 1);

	peer_->SetMaximumIncomingConnections(5);
	peer_->SetOccasionalPing(true);

	printf("Started listening on port %hu\n", port);
}


void ServerUdp::Impl::Update()
{
	//Send

	outmutex.lock();
	std::vector<std::shared_ptr<PacketInfo>>* packets = &outgoing[outflag];
	outflag = !outflag;
	outmutex.unlock();

	for(auto it = packets->begin(); it != packets->end(); it++)
	{
		PacketInfo* p = it->get();

		RakNet::SystemAddress* addr = GetSocket(p->id);
		if(addr)
		{
			const char* data = (const char*)p->packet.GetData();
			const int length = (const int)p->packet.GetDataSize();
			peer_->Send(data, length, HIGH_PRIORITY, RELIABLE_ORDERED, 0, *addr, false);
		}
	}

	if (packets->size() > 0)
	{
		packets->clear();
	}

	//Receive

	RakNet::Packet *packet;
	for (packet=peer_->Receive(); packet; peer_->DeallocatePacket(packet), packet=peer_->Receive())
	{
		switch(packet->data[0])
		{
			case ID_NEW_INCOMING_CONNECTION:
				{
					short id = AddSocketConnection(packet->systemAddress);
					{
						std::lock_guard<std::mutex> lock(newConMutex);
						newOpenSockets.push_back(id);
						hasNewOpenSockets = true;
					}
				}

				break;

				case ID_DISCONNECTION_NOTIFICATION:
				case ID_CONNECTION_LOST:
					{
						short id = RemoveSocketConnection(packet->systemAddress);
						{
							std::lock_guard<std::mutex> lock(dcmutex);
							disconnectedSockets.push_back(id);
							hasDisconnectedSockets = true;
						}
					}
					break;

				case ID_USER_PACKET_ENUM:
				{
					short id = GetId(packet->systemAddress);
												
					if (id < 0)
						break;

					unsigned char* data = packet->data + sizeof(anet::Int8);
					unsigned int length = packet->length - sizeof(anet::Int8);

					std::shared_ptr<PacketInfo> pInfo(new PacketInfo(data, length));

					pInfo->id = id;

					inmutex.lock();
					incoming[inflag].push_back(pInfo);
					inmutex.unlock();
				}

				break;
					
			default:
				break;
		}
	}
}

RakNet::SystemAddress* ServerUdp::Impl::GetSocket(short id)
{
	std::map<short, RakNet::SystemAddress>::iterator it = connections_.find(id);

	if(it != connections_.end())
		return &it->second;

	return NULL;
}

short ServerUdp::Impl::GetId(RakNet::SystemAddress socket)
{
	for (auto it = connections_.begin(); it != connections_.end(); it++)
	{
		if(it->second.systemIndex == socket.systemIndex)
			return it->first;
	}

	return -1;
}

short ServerUdp::Impl::AddSocketConnection(RakNet::SystemAddress socket)
{
	short id = 0;

	for(auto it = connections_.begin(); it != connections_.end(); it++)
	{
		if(it->first == id)
		{
			id++;
			continue;
		}
		else
		{
			break;
		}
	}

	connections_[id] = socket;
	return id;
}

short ServerUdp::Impl::RemoveSocketConnection(RakNet::SystemAddress socket)
{
	for (auto it = connections_.begin(); it != connections_.end(); it++)
	{
		if(strcmp(it->second.ToString(), it->second.ToString()) == 0 && it->second.GetPort() == socket.GetPort())
		{
			short id = it->first;
			connections_.erase(it);
			return id;
		}
	}

	return -1;
}

//---

ServerUdp::ServerUdp(unsigned short port)
	: IServerNetwork(port), pImpl(new ServerUdp::Impl(port))
{
	
}

ServerUdp::~ServerUdp()
{

}

void ServerUdp::Update()
{
	pImpl->Update();
}


void ServerUdp::SendPacket(std::shared_ptr<PacketInfo> pinfo)
{
	pImpl->outmutex.lock();
	pImpl->outgoing[pImpl->outflag].push_back(pinfo);
	pImpl->outmutex.unlock();
}

std::vector<std::shared_ptr<PacketInfo>>* ServerUdp::GetPackets()
{
	pImpl->inmutex.lock();

	auto packets = &pImpl->incoming[pImpl->inflag];
	pImpl->inflag = !pImpl->inflag;

	pImpl->inmutex.unlock();
	return packets;
}

bool ServerUdp::DisconnectedSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->dcmutex);
	return pImpl->hasDisconnectedSockets;
}

std::vector<short> ServerUdp::GetDisconnectedSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->dcmutex);
	pImpl->hasDisconnectedSockets = false;
	std::vector<short> vec = pImpl->disconnectedSockets;
	pImpl->disconnectedSockets.clear();
	return vec;
}

bool ServerUdp::NewOpenSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->newConMutex);
	return pImpl->hasNewOpenSockets;
}

std::vector<short> ServerUdp::GetNewOpenSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->newConMutex);
	pImpl->hasNewOpenSockets = false;
		
	std::vector<short> vec = pImpl->newOpenSockets;
	pImpl->newOpenSockets.clear();
	return vec;
}