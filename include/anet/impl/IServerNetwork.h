#pragma once

#include <vector>
#include <memory>
namespace anet
{
	class PacketInfo;
	class IServerNetwork
	{
	public:
		IServerNetwork(unsigned short port) {};
		virtual ~IServerNetwork() {};

		virtual void Update() = 0;

		virtual void SendPacket(std::shared_ptr<PacketInfo> pInfo) = 0;

		virtual std::vector<std::shared_ptr<PacketInfo>>* GetPackets() = 0;

		virtual bool DisconnectedSockets() = 0;

		virtual std::vector<short> GetDisconnectedSockets() = 0;

		virtual bool NewOpenSockets() = 0;

		virtual std::vector<short> GetNewOpenSockets() = 0;

	};
};