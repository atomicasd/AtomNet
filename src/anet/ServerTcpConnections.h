#pragma once

#include <vector>
#include "ServerNetworkInterface.h"
namespace anet
{
	class PacketInfo;
	class ServerTCPConnections  : public ServerNetworkInterface
	{
	public:
		ServerTCPConnections(unsigned short port);
		virtual ~ServerTCPConnections();
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