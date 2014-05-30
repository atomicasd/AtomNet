#pragma once

#include "anet/impl/IServerNetwork.h"
#include <memory>

namespace anet
{
	class PacketInfo;
	class ServerUdp : public IServerNetwork
	{
	public:
		ServerUdp(unsigned short port);
		virtual ~ServerUdp();

		void Update();

		void SendPacket(std::shared_ptr<PacketInfo> pInfo);

		std::vector<std::shared_ptr<PacketInfo>>* GetPackets();

		bool DisconnectedSockets();

		std::vector<short> GetDisconnectedSockets();

		bool NewOpenSockets();

		std::vector<short> GetNewOpenSockets();

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};