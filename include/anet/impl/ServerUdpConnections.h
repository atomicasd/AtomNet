#pragma once

#include <vector>
#include "anet/impl/ServerNetworkInterface.h"
namespace anet
{
	class PacketInfo;
	class ServerUDPConnections : public ServerNetworkInterface
	{
	public:
		ServerUDPConnections(unsigned short port);
		virtual ~ServerUDPConnections();

		void Update();

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