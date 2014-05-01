#pragma once
#include <functional>
namespace anet
{
	class HybridSocket
	{
	public:
		HybridSocket(std::function<void(bool)> clientConnectionCallbackResult);
		virtual ~HybridSocket();

		void SetHost(char* ip, unsigned short port);
		void Connect();

	private:
		class Impl;
		Impl* pImpl;
	};
}