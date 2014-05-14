#include "anet/impl/ServerTCPConnections.h"
#include "anet/PacketInfo.h"

#include <SFML/Network.hpp>

#include <Windows.h>
#include <iostream>
#include <map>
#include <mutex>
#include <atomic>

using namespace anet;

class ServerTCPConnections::Impl
{
public:
	Impl(unsigned short port);
	void Update();

public:

	//Sockets
	sf::TcpListener listener;
	sf::SocketSelector selector;
	std::vector<sf::TcpSocket*> openSockets;

	std::map<short, sf::TcpSocket*> connections_;

	//Connections

	std::atomic<bool> hasNewOpenSockets;
	std::mutex newConMutex;
	std::vector<short> newOpenSockets;

	std::atomic<bool> hasDisconnectedSockets;
	std::mutex dcmutex;
	std::vector<short> disconnectedSockets;

	//Packets

	std::mutex inmutex;
	std::vector<PacketInfo*> incoming[2];
	bool inflag;

	std::mutex outmutex;
	std::vector<PacketInfo*> outgoing[2];
	bool outflag;

public:
	sf::TcpSocket* GetSocket(short id);
	short GetId(sf::TcpSocket* socket);

	short AddSocketConnection(sf::TcpSocket* socket);
	void RemoveSocketConnection(sf::TcpSocket* socket);
};

ServerTCPConnections::Impl::Impl(unsigned short port)
{

	inflag = false;
	outflag = false;

	hasNewOpenSockets = false;
	hasDisconnectedSockets = false;

	listener.listen(port);
	selector.add(listener);
	printf("Started listening on port %hu\n", port);
}

void ServerTCPConnections::Impl::Update()
{
	//Send
	outmutex.lock();
	std::vector<PacketInfo*>* packets = &outgoing[outflag];
	outflag = !outflag;
	outmutex.unlock();
	for(std::vector<PacketInfo*>::iterator it = packets->begin(); it != packets->end(); it++)
	{
		PacketInfo* p = (*it);
		sf::TcpSocket* socket = GetSocket(p->id);
		if(socket)
			socket->send(p->packet.GetData(), p->packet.GetDataSize());
	}

	packets->clear();

	//Receive
	if (selector.wait(sf::milliseconds(1)))
	{
		if (selector.isReady(listener))
		{
			sf::TcpSocket* socket = new sf::TcpSocket();
			if (listener.accept(*socket) == sf::Socket::Done)
			{
				//New socket connection
				openSockets.push_back(socket);
				selector.add(*socket);

				short id = AddSocketConnection(socket);

				newConMutex.lock();
				newOpenSockets.push_back(id);
				hasNewOpenSockets = true;
				newConMutex.unlock();
			}
		}
		else
		{
			for (std::vector<sf::TcpSocket*>::iterator it = openSockets.begin(); it != openSockets.end();)
			{
				sf::TcpSocket& socket = **it;
				if (selector.isReady(socket))
				{
					sf::Packet packet;
					if (socket.receive(packet) == sf::Socket::Done)
					{
						//Incoming packet
						PacketInfo* pinfo = new PacketInfo(packet.getData(), packet.getDataSize());
						pinfo->id = GetId(&socket);

						inmutex.lock();
						incoming[inflag].push_back(pinfo);
						inmutex.unlock();

						++it;
					} else {
						//Disconnected socket
						selector.remove(socket);
						it = openSockets.erase(it);

						short id = GetId(&socket);

						RemoveSocketConnection(&socket);

						dcmutex.lock();
						disconnectedSockets.push_back(id);
						hasDisconnectedSockets = true;
						dcmutex.unlock();

						continue;
					}
				} else {
					++it;
				}
			}
		}
	}
}

sf::TcpSocket* ServerTCPConnections::Impl::GetSocket(short id)
{
	std::map<short, sf::TcpSocket*>::iterator it = connections_.find(id);

	if(it != connections_.end())
		return it->second;

	return NULL;
}

short ServerTCPConnections::Impl::GetId(sf::TcpSocket* socket)
{
	std::map<short, sf::TcpSocket*>::iterator it = connections_.begin();
	for(; it != connections_.end(); it++)
	{
		if(it->second == socket)
			return it->first;
	}

	return -1;
}

short ServerTCPConnections::Impl::AddSocketConnection(sf::TcpSocket* socket)
{
	short id = 0;
	while(true)
	{
		if(connections_.find(id) == connections_.end())
		{
			connections_[id] = socket;
			break;
		}
		id++;
	}

	return id;
}

void ServerTCPConnections::Impl::RemoveSocketConnection(sf::TcpSocket* socket)
{
	std::map<short, sf::TcpSocket*>::iterator it = connections_.begin();
	for(; it != connections_.end(); it++)
	{
		if(it->second == socket)
		{
			connections_.erase(it);
			break;
		}
	}
}

//---

ServerTCPConnections::ServerTCPConnections(unsigned short port)
	: ServerNetworkInterface(port), 
	pImpl(new ServerTCPConnections::Impl(port))
{
	
}

ServerTCPConnections::~ServerTCPConnections()
{

}

void ServerTCPConnections::Run()
{
	pImpl->Run();
}

void ServerTCPConnections::Stop()
{
	pImpl->running = false;
}

void ServerTCPConnections::SendPacket(PacketInfo* pinfo)
{
	pImpl->outmutex.lock();
	pImpl->outgoing[pImpl->outflag].push_back(pinfo);
	pImpl->outmutex.unlock();
}

std::vector<PacketInfo*>* ServerTCPConnections::GetPackets()
{
	pImpl->inmutex.lock();

	std::vector<PacketInfo*>* packets = &pImpl->incoming[pImpl->inflag];
	pImpl->inflag = !pImpl->inflag;

	pImpl->inmutex.unlock();
	return packets;
}

bool ServerTCPConnections::DisconnectedSockets()
{
	std::lock_guard<std::mutex>(pImpl->dcmutex);
	return pImpl->hasDisconnectedSockets;
}

std::vector<short> ServerTCPConnections::GetDisconnectedSockets()
{
	std::lock_guard<std::mutex>(pImpl->dcmutex);

	std::vector<short> vec = pImpl->disconnectedSockets;
	pImpl->disconnectedSockets.clear();
	pImpl->hasDisconnectedSockets = false;

	return vec;
}

bool ServerTCPConnections::NewOpenSockets()
{
	return pImpl->hasNewOpenSockets;
}

std::vector<short> ServerTCPConnections::GetNewOpenSockets()
{
	std::lock_guard<std::mutex>(pImpl->newConMutex);

	std::vector<short> vec = pImpl->newOpenSockets;
	pImpl->newOpenSockets.clear();
	pImpl->hasNewOpenSockets = false;

	return vec;
}