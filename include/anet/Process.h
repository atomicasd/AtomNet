#pragma once

namespace anet
{
	class Packet;
	class Process
	{
	public:
		Process(int type) 
		: type_(type) {};

		virtual ~Process() {};

		virtual void HandlePacket(Packet& packet) = 0;

		int GetType()
		{
			return type_;
		}
	private:
		int type_;
	};
}; 