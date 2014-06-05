#include <anet/ServerUdp.h>
#include <anet/PacketInfo.h>
#include <anet/impl/SocketContainer.h>
#include <anet/impl/DoubleBufferedContainer.h>

#include <iostream>
#include <memory>

#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "UdpMessages.h"

#define FPS 1.0f/60.0f

using namespace anet;

class ServerUdp::Impl : public SocketContainer<RakNet::RakNetGUID>
{
public:
	Impl(unsigned short port, unsigned short maxplayers);
	void Update();

public:
	RakNet::RakPeerInterface *peer_;

public:
	DoubleBufferedContainer<std::shared_ptr<PacketInfo>> incoming_;
	DoubleBufferedContainer<std::shared_ptr<PacketInfo>> outgoing_;
};

ServerUdp::Impl::Impl(unsigned short port, unsigned short maxplayers) :
	peer_(RakNet::RakPeerInterface::GetInstance())
{
	RakNet::SocketDescriptor sd(port,0);

	peer_->Startup(maxplayers, &sd, 1);
	peer_->SetMaximumIncomingConnections(5);
	peer_->SetOccasionalPing(true);
}

void ServerUdp::Impl::Update()
{
	//Send

	auto packets = outgoing_.Get();

	for(auto it = packets->begin(); it != packets->end(); it++)
	{
		PacketInfo* p = it->get();
		RakNet::RakNetGUID* guid = GetSocket(p->id);

		if (guid)
		{
			RakNet::SystemAddress addr = peer_->GetSystemAddressFromGuid(*guid);

			const char* data = (const char*)p->packet->GetData();
			const int length = (const int)p->packet->GetDataSize();

			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)ID_USER_PACKET_ENUM);
			bs.Write(data, length);

			peer_->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, addr, false);
		}
	}

	packets->clear();

	//Receive

	RakNet::Packet *packet;
	for (packet=peer_->Receive(); packet; peer_->DeallocatePacket(packet), packet=peer_->Receive())
	{
		switch(packet->data[0])
		{
			std::cout << packet->data << std::endl;
			case ID_NEW_INCOMING_CONNECTION:
			{
				AddSocket(packet->guid);
				break;
			}

			case ID_CONNECTION_BANNED:
			case ID_DISCONNECTION_NOTIFICATION:
			case ID_CONNECTION_LOST:
			{
				RemoveSocket(packet->guid);
				break;
			}
			case ID_USER_PACKET_ENUM:
			{
				short id = GetId(packet->guid);

				if (id < 0)
					break;

				unsigned char* data = packet->data + sizeof(RakNet::MessageID);
				unsigned int length = packet->length - sizeof(RakNet::MessageID);

				std::shared_ptr<PacketInfo> pInfo(new PacketInfo(data, length));

				pInfo->id = id;

				incoming_.Add(pInfo);

				break;
			}
					
			default:
				break;
		}
	}
}

//---

ServerUdp::ServerUdp(unsigned short port, unsigned short maxplayers)
: IServerNetwork(), pImpl(new ServerUdp::Impl(port, maxplayers))
{
	
}

ServerUdp::~ServerUdp()
{

}

void ServerUdp::Update()
{
	pImpl->Update();
}

void ServerUdp::SendPacket(std::shared_ptr<PacketInfo> p)
{
	pImpl->outgoing_.Add(p);
}

std::vector<std::shared_ptr<PacketInfo>>* ServerUdp::GetPackets()
{
	std::vector<std::shared_ptr<PacketInfo>>* packets = pImpl->incoming_.Get();

	return packets;
}

bool ServerUdp::HasNewSockets()
{
	return pImpl->HasNewSockets();
}

bool ServerUdp::HasRemovedSockets()
{
	return pImpl->HasRemovedSockets();
}

std::vector<unsigned short> ServerUdp::GetNewSockets()
{
	return pImpl->GetNewSockets();
}

std::vector<unsigned short> ServerUdp::GetRemovedSockets()
{
	return pImpl->GetRemovedSockets();
}