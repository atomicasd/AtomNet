#include <anet/ClientConnection.h>
#include <anet/impl/IClientNetwork.h>
#include <anet/ConnectionTypes.h>
#include <memory>
#include <assert.h>
#include <map>
using namespace anet;

class ClientConnection::Impl
{
public:
	Impl(IClientNetwork& implementation);

	IClientNetwork& net_;
	char* ip_;
	unsigned short port_;
	bool type_;
	
	void ConnectionResultCallback(bool result);

	std::map<int, Process*> processes_;
};

ClientConnection::Impl::Impl(IClientNetwork& implementation) : 
net_(implementation),
ip_(NULL),
port_(0)
{

}

void ClientConnection::Impl::ConnectionResultCallback(bool connected)
{

}

ClientConnection::ClientConnection(IClientNetwork& implementation)
: pImpl(new Impl(implementation))
{

}

ClientConnection::~ClientConnection()
{
	pImpl->net_.Disconnect();
}

void ClientConnection::SetHost(char* ip, unsigned short port)
{
	pImpl->ip_ = ip;
	pImpl->port_ = port;

	pImpl->net_.SetHost(ip, port);
}

void ClientConnection::Connect()
{
	pImpl->net_.Connect();
}

void ClientConnection::Disconnect()
{
	pImpl->net_.Disconnect();
}

void ClientConnection::AddProcess(Process& process)
{
	pImpl->processes_[process.GetType()] = &process;
}

void ClientConnection::RemoveProcess(int type)
{
	std::map<int, Process*>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		pImpl->processes_.erase(it);
	}
}

void ClientConnection::SendPacket(Packet& packet)
{
	return pImpl->net_.SendPacket(packet);
}

void ClientConnection::Update()
{
	pImpl->net_.ReceivePackets();

	auto packets = pImpl->net_.packets;

	for (auto it = packets.begin(); it != packets.end(); it++)
	{
		int type;
		*it->get() >> type;

		std::map<int, Process*>::iterator proc = pImpl->processes_.find(type);

		if (proc != pImpl->processes_.end())
		{
			proc->second->HandlePacket(*it->get());
		}
	}

	packets.clear();
}