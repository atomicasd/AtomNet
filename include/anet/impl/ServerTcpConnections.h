#pragma once

#include "anet/impl/ServerNetworkInterface.h"
#include <vector>
#include <memory>
namespace anet
{
	class PacketInfo;
	class ServerTCPConnections  : public ServerNetworkInterface
	{
	public:
		ServerTCPConnections(unsigned short port);
		virtual ~ServerTCPConnections();

		void Update();

		void SendPacket(PacketInfo* pInfo);
		std::vector<PacketInfo*>* GetPackets();

		bool DisconnectedSockets();
		std::vector<short> GetDisconnectedSockets();

		bool NewOpenSockets();
		std::vector<short> GetNewOpenSockets();

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};