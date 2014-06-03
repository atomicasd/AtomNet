#pragma once

#include "anet/impl/IServerNetwork.h"
#include <memory>

namespace anet
{
	class PacketInfo;
	class ServerUdp : public IServerNetwork
	{
	public:
		ServerUdp(unsigned short port, unsigned short maxplayers = 32);
		virtual ~ServerUdp();

		void Update();

		void SendPacket(std::shared_ptr<PacketInfo> pInfo);

		std::vector<std::shared_ptr<PacketInfo>>* GetPackets();

		bool HasNewSockets();

		std::vector<unsigned short> GetNewSockets();

		bool HasRemovedSockets();

		std::vector<unsigned short> GetRemovedSockets();

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};