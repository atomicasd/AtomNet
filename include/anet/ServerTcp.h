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

		bool HasNewSockets();

		std::vector<unsigned short> GetNewSockets();

		bool HasRemovedSockets();

		std::vector<unsigned short> GetRemovedSockets();

	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};
};