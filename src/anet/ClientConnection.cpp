#include "anet/ClientConnection.h"
#include "anet/impl/IClientNetwork.h"
#include <anet/ConnectionTypes.h>
#include <memory>
#include <assert.h>
#include <boost/signals2.hpp>
#include <map>
using namespace anet;

class ClientConnection::Impl
{
public:
	Impl();

	std::shared_ptr<IClientNetwork> net_;

	char* ip_;
	unsigned short port_;
	ConnectionType type_;

	boost::signals2::signal<void(bool)> sigClientConnected_;
	boost::signals2::signal<void(bool)> sigClientDisconnected_;
	std::vector<boost::signals2::connection> sigConnections_;

	void ConnectionResultCallback(bool result);

	std::map<int, std::shared_ptr<Process>> processes_;
};

ClientConnection::Impl::Impl()
	: ip_(NULL),
	port_(0)
{

}

void ClientConnection::Impl::ConnectionResultCallback(bool connected)
{
	sigClientConnected_(connected);
}

ClientConnection::ClientConnection(std::shared_ptr<IClientNetwork> implementation)
	: pImpl(new Impl())
{
	pImpl->net_ = implementation;
}

ClientConnection::~ClientConnection()
{
	pImpl->net_->Disconnect();
}

void ClientConnection::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;

	pImpl->net_->SetHost(ip, port);
}

void ClientConnection::Connect()
{
	pImpl->net_->Connect();
}

void ClientConnection::Disconnect()
{
	pImpl->net_->Disconnect();
}

void ClientConnection::AddProcess(int type, std::shared_ptr<Process> process)
{
	pImpl->processes_[type] = process;
}

void ClientConnection::RemoveProcess(int type)
{
	std::map<int, std::shared_ptr<Process>>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		pImpl->processes_.erase(it);
	}
}

void ClientConnection::SendPacket(std::shared_ptr<Packet> packet)
{
	return pImpl->net_->SendPacket(packet);
}

void ClientConnection::ReceivePackets()
{
	pImpl->net_->ReceivePackets();
}

void ClientConnection::RegisterConnectionCallback(std::function<void(bool)> onConnected)
{
	pImpl->sigConnections_.push_back(pImpl->sigClientConnected_.connect(onConnected));
}

void ClientConnection::RemoveConnectionCallback(std::function<void(bool)> onConnected)
{
	//Todo, Implement a disconnect callback method
}