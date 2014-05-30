#pragma once

#include "anet/impl/IServerNetwork.h"
#include <memory>

namespace anet
{
	class PacketInfo;
	class ServerTcp  : public IServerNetwork
	{
	public:
		ServerTcp(unsigned short port);
		virtual ~ServerTcp();

		void Update();

		void SendPacket(PacketInfo* pInfo);

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