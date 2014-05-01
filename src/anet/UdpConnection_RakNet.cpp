#include "UdpConnection.h"

#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "UdpMessages.h"

#include "MessageIdentifiers.h"

#include <functional>
#include <iostream>
#include "UdpMessages.h"
using namespace anet;

class UdpConnection::Impl
{
public:
	Impl();

public:
	RakNet::RakPeerInterface* peer_;
	RakNet::SocketDescriptor sd_;

	char* ip_;
	unsigned short port_;
	std::function<void(bool)> callback_;
};

UdpConnection::Impl::Impl() :
	peer_(RakNet::RakPeerInterface::GetInstance())
{
	peer_->Startup(1,&sd_, 1);
}

UdpConnection::UdpConnection(std::function<void(bool)> clientConnectionCallbackResult) :
pImpl(new Impl()), ClientNetworkInterface()
{
	pImpl->callback_ = clientConnectionCallbackResult;
}

UdpConnection::~UdpConnection()
{
	RakNet::RakPeerInterface::DestroyInstance(pImpl->peer_);
	delete pImpl;
}

void UdpConnection::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;
}

void UdpConnection::Connect()
{
	pImpl->peer_->Connect(pImpl->ip_, pImpl->port_, 0,0);
	std::cout << "Connecting to " << pImpl->ip_ << ":" << pImpl->port_ << std::endl;
}

void UdpConnection::Disconnect()
{
	pImpl->peer_->Shutdown(0);
}

bool UdpConnection::IsConnected()
{
	return pImpl->peer_->IsActive();
}

void UdpConnection::ReceivePackets()
{
	RakNet::Packet *packet;
	RakNet::RakPeerInterface* peer = pImpl->peer_;

	for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
	{
		switch (packet->data[0])
		{

			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "ID_DISCONNECTION_NOTIFICATION" << std::endl;
				break;

			case ID_CONNECTION_LOST:
				std::cout << "ID_CONNECTION_LOST" << std::endl;
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "ID_NO_FREE_INCOMING_CONNECTIONS" << std::endl;
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED:
				pImpl->callback_(true);
				std::cout << "Connected through port" << peer->GetMyBoundAddress().GetPort() << std::endl;
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				pImpl->callback_(false);
				break;

			case ID_GAME_MESSAGE_1:
				packets.push_back( std::shared_ptr<Packet>(new Packet(packet->data, packet->length)));
				break;

			default:
				std::cout << packet->data[0] << std::endl;
				break;
		}
	}
}

void UdpConnection::SendPacket(std::shared_ptr<Packet> packet)
{
	RakNet::SystemAddress addr;
	addr.FromString(pImpl->ip_);
	addr.SetPortHostOrder(pImpl->port_);

	const char* data = (const char*)packet->GetData();
	const int length = (const int)packet->GetDataSize();

	RakNet::BitStream bs;
	RakNet::MessageID messageId = ID_USER_PACKET_ENUM;
	bs.Write(messageId);
	bs.Write(data, length);

	int result = pImpl->peer_->Send(&bs, IMMEDIATE_PRIORITY, UNRELIABLE, 0, addr, false);

	std::cout << result << std::endl;
}
