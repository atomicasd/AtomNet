#include "ClientTcp.h"
#include <memory>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>

using namespace anet;

class ClientTcp::Impl
{
public:
	Impl();
	sf::TcpSocket socket_;
	sf::IpAddress ip_;
	unsigned short port_;
	std::function<void(bool)> callback_;
};

ClientTcp::Impl::Impl()
	: ip_(sf::IpAddress::None),
	port_(0)
{

}

ClientTcp::ClientTcp(std::function<void(bool)> clientConnectionCallbackResult) :
pImpl(new Impl()), IClientNetwork()
{
	pImpl->callback_ = clientConnectionCallbackResult;
	pImpl->socket_.setBlocking(false);
}

ClientTcp::~ClientTcp()
{
	delete pImpl;
}

void ClientTcp::SendPacket(std::shared_ptr<Packet> packet)
{
	pImpl->socket_.send(packet->GetData(), packet->GetDataSize());
}

void ClientTcp::ReceivePackets()
{
	sf::Packet packet;

	if(pImpl->socket_.receive(packet) == sf::Socket::Done)
	{
		packets.push_back( std::shared_ptr<Packet>(new Packet(packet.getData(), packet.getDataSize())));
	}
}

void ClientTcp::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;
}

void ClientTcp::Connect()
{
	bool result = pImpl->socket_.connect(pImpl->ip_, pImpl->port_);

	pImpl->callback_(result);
}

void ClientTcp::Disconnect()
{
	pImpl->socket_.disconnect();
}

bool ClientTcp::IsConnected()
{
	return pImpl->socket_.getRemoteAddress() != sf::IpAddress::None;
}
