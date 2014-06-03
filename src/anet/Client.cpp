#include <anet/Client.h>
#include <anet/Process.h>
#include <anet/Packet.h>
#include <anet/PacketInfo.h>
#include <anet/ClientConnections.h>
#include <stdio.h>
#include <map>

#include "Process.h"

#include <iostream>

using namespace anet;

class Client::Impl
{
public:
	Impl();

	short id_;
	std::map<int, std::shared_ptr<Process>> processes_;
	ClientConnections* connections_;
};

Client::Impl::Impl()
{

}

//---

Client::Client(short id, ClientConnections* connections) :
pImpl(new Impl())
{
	pImpl->id_ = id;
	pImpl->connections_ = connections;
}

Client::~Client()
{

}

void Client::AddProcess(std::shared_ptr<Process> process)
{
	pImpl->processes_[process->GetType()] = process;
}

void Client::RemoveProcess(int type)
{
	std::map<int, std::shared_ptr<Process>>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		pImpl->processes_.erase(it);
	}
}

void Client::HandlePacket(Packet& packet)
{
	int type;
	packet >> type;

	std::map<int, std::shared_ptr<Process>>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		it->second->HandlePacket(packet);
	}
}

void Client::SendPacket(std::shared_ptr<Packet> packet)
{
	std::shared_ptr<PacketInfo> pInfo(new PacketInfo(packet));

	pInfo->id = pImpl->id_;

	pImpl->connections_->SendPacket(pInfo);
}

short Client::GetId() const
{
	return pImpl->id_;
}