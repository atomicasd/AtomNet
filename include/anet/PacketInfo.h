#pragma once

#include <anet/Packet.h>
#include <memory>
namespace anet
{
	class PacketInfo
	{
	public:
		PacketInfo() : packet(new Packet()) {}

		PacketInfo(std::shared_ptr<Packet> p) : packet(p) {}

		PacketInfo(const void* data, size_t size) : 
			packet(new Packet(data, size)) {}

		std::shared_ptr<Packet> packet;
		unsigned short id;
	};
};