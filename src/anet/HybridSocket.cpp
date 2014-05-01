#include "HybridSocket.h"
#include "RakPeer.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "UdpMessages.h"
#include <iostream>
using namespace anet;

class HybridSocket::Impl
{
public:
	Impl();

	char* ip_;
	unsigned short port_;
	std::function<void(bool)> callback_;

	RakNet::RakPeerInterface* peer_;
	RakNet::SocketDescriptor sd_;
};

HybridSocket::Impl::Impl()
{
	peer_ = RakNet::RakPeerInterface::GetInstance();

	peer_->Startup(20, &sd_, 1);
}

HybridSocket::HybridSocket(std::function<void(bool)> clientConnectionCallbackResult) :
	pImpl(new Impl())
{
	pImpl->callback_ = clientConnectionCallbackResult;
}

void HybridSocket::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;
}

void HybridSocket::Connect()
{
	pImpl->peer_->Connect(pImpl->ip_, pImpl->port_, 0,0);
	std::cout << "Connecting to " << pImpl->ip_ << ":" << pImpl->port_ << std::endl;

	bool connecting = true;

	while(connecting)
	{
		Sleep(100);

		RakNet::Packet *packet;
		RakNet::RakPeerInterface* peer = pImpl->peer_;

		for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
		{
			switch (packet->data[0])
			{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					pImpl->callback_(true);
					std::cout << "Connected through port" << peer->GetMyBoundAddress().GetPort() << std::endl;
					connecting = false;
					break;
				case ID_CONNECTION_ATTEMPT_FAILED:
					pImpl->callback_(false);
					std::cout << "ID_CONNECTION_ATTEMPT_FAILED" << std::endl;
					connecting = false;
					break;
			}
		}
	}
}