#pragma once

#include <anet/Packet.h>

namespace anet
{
	class PacketInfo
	{
	public:
		PacketInfo() : packet() {}
		PacketInfo(const void* data, size_t size) : 
			packet(data, size) {}

		Packet packet;
		short id;
	};
};