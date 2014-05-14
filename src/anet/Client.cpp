#include "anet/Client.h"
#include "anet/Process.h"
#include "anet/Packet.h"
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
};

Client::Impl::Impl()
{

}

//---

Client::Client(short id) :
pImpl(new Impl())
{
	pImpl->id_ = id;
}

Client::~Client()
{

}

void Client::AddProcess(int type, std::shared_ptr<Process> process)
{
	pImpl->processes_[type] = process;
}

void Client::Removeprocess(int type)
{
	std::map<int, std::shared_ptr<Process>>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		pImpl->processes_.erase(it);
	}
}

void Client::HandlePacket(Packet& packet)
{
	anet::Int16 type;
	packet >> type;

	std::map<int, std::shared_ptr<Process>>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		it->second->HandlePacket(packet);
	}
}



short Client::GetId() const
{
	return pImpl->id_;
}