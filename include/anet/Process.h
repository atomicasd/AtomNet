#include <memory>
#include <stdio.h>
#pragma once

namespace anet
{
	class Packet;
	class Process
	{
	public:
		Process(int type) : m_type(type) {};
		virtual ~Process() {};

		virtual void Start() = 0;
		virtual void HandlePacket(Packet& packet) = 0;
	private:
		int m_type;
	};
}; 