#pragma once

#include <vector>
#include <memory>
namespace anet
{
	class PacketInfo;
	class IServerNetwork
	{
	public:
		IServerNetwork() {};
		virtual ~IServerNetwork() {};

		virtual void Update() = 0;

		virtual void SendPacket(std::shared_ptr<PacketInfo> pInfo) = 0;

		virtual std::vector<std::shared_ptr<PacketInfo>>* GetPackets() = 0;

		virtual bool HasNewSockets() = 0;

		virtual std::vector<unsigned short> GetNewSockets() = 0;

		virtual bool HasRemovedSockets() = 0;

		virtual std::vector<unsigned short> GetRemovedSockets() = 0;
	};
};