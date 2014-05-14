#pragma once

#include <vector>

namespace anet
{
	class PacketInfo;
	class ServerNetworkInterface
	{
	public:
		ServerNetworkInterface(unsigned short port) {};
		virtual ~ServerNetworkInterface() {};

		virtual void Update() = 0;

		virtual void SendPacket(PacketInfo* pInfo) = 0;
		virtual std::vector<PacketInfo*>* GetPackets() = 0;

		virtual bool DisconnectedSockets() = 0;
		virtual std::vector<short> GetDisconnectedSockets() = 0;

		virtual bool NewOpenSockets() = 0;
		virtual std::vector<short> GetNewOpenSockets() = 0;

	};
};