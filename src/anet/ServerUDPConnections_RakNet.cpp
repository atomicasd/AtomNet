#include "ServerUDPConnections.h"
#include "anet/PacketInfo.h"

#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "UdpMessages.h"

#include <Windows.h>
#include <iostream>
#include <map>
#include <mutex>
#include <memory>

#define FPS 1.0f/60.0f

using namespace anet;

class ServerUDPConnections::Impl
{
public:
	Impl(unsigned short port);
	void Run();

public:
	bool running;
	RakNet::RakPeerInterface *peer_;

	bool hasNewOpenSockets;
	std::mutex newConMutex;
	std::vector<short> newOpenSockets;

	bool hasDisconnectedSockets;
	std::mutex dcmutex;
	std::vector<short> disconnectedSockets;

public:
	std::mutex inmutex;
	std::vector<PacketInfo*> incoming[2];
	bool inflag;

	std::mutex outmutex;
	std::vector<PacketInfo*> outgoing[2];
	bool outflag;

public:
	std::map<short, RakNet::SystemAddress> connections_;
	RakNet::SystemAddress* GetSocket(short id);
	short GetId(RakNet::SystemAddress);

	short AddSocketConnection(RakNet::SystemAddress socket);
	short RemoveSocketConnection(RakNet::SystemAddress socket);
};

ServerUDPConnections::Impl::Impl(unsigned short port) :
	peer_(RakNet::RakPeerInterface::GetInstance())
{
	running = true;

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


void ServerUDPConnections::Impl::Run()
{
	while(running)
	{
		Sleep(20);
		//Send
		outmutex.lock();
		std::vector<PacketInfo*>* packets = &outgoing[outflag];
		outflag = !outflag;
		outmutex.unlock();
		for(std::vector<PacketInfo*>::iterator it = packets->begin(); it != packets->end(); it++)
		{
			PacketInfo* p = (*it);
			RakNet::SystemAddress* addr = GetSocket(p->id);
			if(addr)
			{
				printf("Sending packet");
				const char* data = (const char*)p->packet.GetData();
				const int length = (const int)p->packet.GetDataSize();
				peer_->Send(data, length, HIGH_PRIORITY, RELIABLE_ORDERED, 0, *addr, false);
			}

			delete p;
		}

		packets->clear();

		//Receive

		RakNet::Packet *packet;
		for (packet=peer_->Receive(); packet; peer_->DeallocatePacket(packet), packet=peer_->Receive())
		{
			switch(packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
					{
						printf("Another client has connected.\n");

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
							printf("Dropped client.\n");

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
						PacketInfo* pInfo = new PacketInfo(packet->data, packet->length);
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
}

RakNet::SystemAddress* ServerUDPConnections::Impl::GetSocket(short id)
{
	std::map<short, RakNet::SystemAddress>::iterator it = connections_.find(id);

	if(it != connections_.end())
		return &it->second;

	return NULL;
}

short ServerUDPConnections::Impl::GetId(RakNet::SystemAddress socket)
{
	std::map<short, RakNet::SystemAddress>::iterator it = connections_.begin();
	for(; it != connections_.end(); it++)
	{
		if(it->second.systemIndex == socket.systemIndex)
			return it->first;
	}

	return -1;
}

short ServerUDPConnections::Impl::AddSocketConnection(RakNet::SystemAddress socket)
{
	short id = 0;

	if(connections_.size() == 0)
	{
		connections_[0] = socket;
		return id;
	}

	std::map<short, RakNet::SystemAddress>::iterator it = connections_.begin();
	for(; it != connections_.end(); it++)
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

short ServerUDPConnections::Impl::RemoveSocketConnection(RakNet::SystemAddress socket)
{
	std::map<short, RakNet::SystemAddress>::iterator it = connections_.begin();
	for(; it != connections_.end(); it++)
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

ServerUDPConnections::ServerUDPConnections(unsigned short port)
	: ServerNetworkInterface(port), pImpl(new ServerUDPConnections::Impl(port))
{
	
}

ServerUDPConnections::~ServerUDPConnections()
{
	delete pImpl;
}

void ServerUDPConnections::Run()
{
	pImpl->Run();
}

void ServerUDPConnections::SendPacket(PacketInfo* pinfo)
{
	pImpl->outmutex.lock();
	pImpl->outgoing[pImpl->outflag].push_back(pinfo);
	pImpl->outmutex.unlock();
}

std::vector<PacketInfo*>* ServerUDPConnections::GetPackets()
{
	pImpl->inmutex.lock();

	std::vector<PacketInfo*>* packets = &pImpl->incoming[pImpl->inflag];
	pImpl->inflag = !pImpl->inflag;

	pImpl->inmutex.unlock();
	return packets;
}

bool ServerUDPConnections::DisconnectedSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->dcmutex);
	return pImpl->hasDisconnectedSockets;
}

std::vector<short> ServerUDPConnections::GetDisconnectedSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->dcmutex);
	pImpl->hasDisconnectedSockets = false;
	std::vector<short> vec = pImpl->disconnectedSockets;
	pImpl->disconnectedSockets.clear();
	return vec;
}

bool ServerUDPConnections::NewOpenSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->newConMutex);
	return pImpl->hasNewOpenSockets;
}

std::vector<short> ServerUDPConnections::GetNewOpenSockets()
{
	std::lock_guard<std::mutex> lock(pImpl->newConMutex);
	pImpl->hasNewOpenSockets = false;
		
	std::vector<short> vec = pImpl->newOpenSockets;
	pImpl->newOpenSockets.clear();
	return vec;
}