#pragma once

#include <memory>
#include <anet/ConnectionTypes.h>

namespace anet
{
	typedef enum UPNPResult
	{
		UPNP_NODEVICES,
		UPNP_SUCCESS,
		UPNP_NOIGD
	} UPNPResult;

	class UPNPService
	{
	public:
		UPNPService(unsigned short port, ConnectionType connectionType);

		UPNPResult Open();

	private:
		class Impl;
		std::shared_ptr<Impl> pImpl;
	};
};