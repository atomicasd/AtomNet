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

	class UPnP
	{
	public:
		UPnP();

		UPNPResult Open(unsigned short port, ConnectionType connectionType);
	};
};