#include "anet/Client.h"
#include "anet/Process.h"
#include "anet/Packet.h"
#include <stdio.h>
#include <map>

#include "SFML/Network/Packet.hpp"
#include "Process.h"
using namespace anet;

class Client::Impl
{
public:
	Impl(short id);

	short id_;
	std::map<int, std::shared_ptr<Process>> processes_;
};

Client::Impl::Impl(short id) :
id_(id)
{

}

//---

Client::Client(short id)
{
	pImpl = new Impl(id);
}

Client::~Client()
{
	delete pImpl;
}

void Client::AttachProcess(int processType, std::shared_ptr<Process> process)
{
	pImpl->processes_[processType] = process;
}

void Client::HandlePacket(Packet& packet)
{
	sf::Int8 type;
	packet >> type;
	
	std::map<int, std::shared_ptr<Process>>::iterator it = pImpl->processes_.find(type);

	if (it != pImpl->processes_.end())
	{
		it->second->HandlePacket(packet);
	}
}

short Client::GetId()
{
	return pImpl->id_;
}