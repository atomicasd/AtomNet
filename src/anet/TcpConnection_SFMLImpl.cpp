#include "TcpConnection.h"
#include <memory>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>

using namespace anet;

class TcpConnection::Impl
{
public:
	Impl();
	sf::TcpSocket socket_;
	sf::IpAddress ip_;
	unsigned short port_;
	std::function<void(bool)> callback_;
};

TcpConnection::Impl::Impl()
	: ip_(sf::IpAddress::None),
	port_(0)
{
	socket_.setBlocking(false);
}

TcpConnection::TcpConnection(std::function<void(bool)> clientConnectionCallbackResult) :
pImpl(new Impl()), ClientNetworkInterface()
{
	pImpl->callback_ = clientConnectionCallbackResult;
}

TcpConnection::~TcpConnection()
{
	delete pImpl;
}

void TcpConnection::SendPacket(std::shared_ptr<Packet> packet)
{
	pImpl->socket_.send(packet->GetData(), packet->GetDataSize());
}

void TcpConnection::ReceivePackets()
{
	sf::Packet packet;

	if(pImpl->socket_.receive(packet) == sf::Socket::Done)
	{
		packets.push_back( std::shared_ptr<Packet>(new Packet(packet.getData(), packet.getDataSize())));
	}
}

void TcpConnection::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;
}

void TcpConnection::Connect()
{
	if(pImpl->socket_.connect(pImpl->ip_, pImpl->port_))
		pImpl->callback_(true);
	else
		pImpl->callback_(false);
}

void TcpConnection::Disconnect()
{
	pImpl->socket_.disconnect();
}

bool TcpConnection::IsConnected()
{
	return pImpl->socket_.getRemoteAddress() != sf::IpAddress::None;
}
