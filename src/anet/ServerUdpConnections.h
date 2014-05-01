#pragma once

#include <vector>
#include "ServerNetworkInterface.h"
namespace anet
{
	class PacketInfo;
	class ServerUDPConnections : public ServerNetworkInterface
	{
	public:
		ServerUDPConnections(unsigned short port);
		virtual ~ServerUDPConnections();
		void Run();

		void SendPacket(PacketInfo* pInfo);
		std::vector<PacketInfo*>* GetPackets();

		bool DisconnectedSockets();
		std::vector<short> GetDisconnectedSockets();

		bool NewOpenSockets();
		std::vector<short> GetNewOpenSockets();

	private:
		class Impl;
		Impl* pImpl;
	};
};