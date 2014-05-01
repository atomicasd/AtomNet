#include "anet/ClientConnection.h"
#include "TcpConnection.h"
#include "UdpConnection.h"
#include "ClientNetworkInterface.h"
#include <anet/ConnectionTypes.h>
#include <memory>
#include <assert.h>
#include <boost/signals2.hpp>
#include <map>
using namespace anet;

//TODO: Create a base class for connections, remove switch cases.

class ClientConnection::Impl
{
public:
	Impl(ConnectionType type);
	std::unique_ptr<ClientNetworkInterface> net_;

	char* ip_;
	unsigned short port_;

	ConnectionType type_;

	boost::signals2::signal<void(bool)> sigClientConnected_;
	boost::signals2::signal<void(bool)> sigClientDisconnected_;

	std::vector<boost::signals2::connection> sigConnections_;

	void CallbackClientConnectionResult(bool result);


};

ClientConnection::Impl::Impl(ConnectionType type)
	: ip_(NULL),
	port_(0),
	type_(type)
{

	switch(type)
	{
	case ConnectionType::TCP:
		net_.reset(new TcpConnection(std::bind1st(std::mem_fun(&ClientConnection::Impl::CallbackClientConnectionResult), this)));
	case ConnectionType::UDP:
		net_.reset(new UdpConnection(std::bind1st(std::mem_fun(&ClientConnection::Impl::CallbackClientConnectionResult), this)));

	default:
		break;
	}
}

void ClientConnection::Impl::CallbackClientConnectionResult(bool connected)
{
	sigClientConnected_(connected);
}

ClientConnection::ClientConnection(ConnectionType type)
	: pImpl(new Impl(type))
{

}

void ClientConnection::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;

	assert(pImpl->net_.get());
	pImpl->net_->SetHost(ip, port);
}

void ClientConnection::Connect()
{
	assert(pImpl->net_.get());
	pImpl->net_->Connect();

}

void ClientConnection::SendPacket(std::shared_ptr<Packet> packet)
{
	assert(pImpl->net_.get());
	return pImpl->net_->SendPacket(packet);
}

void ClientConnection::ReceivePackets()
{
	assert(pImpl->net_.get());
	pImpl->net_->ReceivePackets();
}
void ClientConnection::Disconnect()
{
	assert(pImpl->net_.get());
	pImpl->net_->Disconnect();
}

void ClientConnection::RegisterConnectionCallback(std::function<void(bool)> onConnected)
{
	pImpl->sigConnections_.push_back(pImpl->sigClientConnected_.connect(onConnected));
}

void ClientConnection::RemoveConnectionCallback(std::function<void(bool)> onConnected)
{
	//Todo, Implement a disconnect callback method
}