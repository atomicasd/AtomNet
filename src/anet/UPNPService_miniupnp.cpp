#include "UPNPService.h"

#include "miniupnpc.h"
#include "upnpcommands.h"
#include "upnperrors.h"

#include <stdio.h>
#include <stdlib.h>
using namespace anet;

class UPNPService::Impl
{
public:
	Impl();

	unsigned short port_;
	ConnectionType connectionType_;

};

UPNPService::Impl::Impl()
{

}

UPNPService::UPNPService(unsigned short port, ConnectionType connectionType) : pImpl(new Impl())
{

}

UPNPResult UPNPService::Open()
{
	struct UPNPDev * devlist = 0;

	int error;
	devlist = upnpDiscover(3000, 0, 0, 0, 0, &error);

	if (devlist)
	{
		char lanaddr[64];	/* my ip address on the LAN */
		struct UPNPUrls urls;
		struct IGDdatas data;

		int igd = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));

		if (igd != 0)
		{
			char iport[32];
			_itoa_s(pImpl->port_, iport, 10);
			char eport[32];
			_itoa_s(pImpl->port_, eport, 10);

			int r = UPNP_AddPortMapping(urls.controlURL, 
				data.first.servicetype,
				eport, 
				iport, 
				lanaddr, 
				0, 
				(pImpl->connectionType_ == ConnectionType::UDP) ? "UDP" : "TCP",
				0, 
				"0");

			if (r != UPNPCOMMAND_SUCCESS)
			{
				printf("AddPortMapping(%s, %s, %s) failed with code %d (%s)\n", eport, iport, lanaddr, r, strupnperror(r));
			}

			char intPort[6];
			char intClient[16];
			char desc[80];
			char enabled[4];
			char leaseDuration[16];
			r = UPNP_GetSpecificPortMappingEntry(
				urls.controlURL,
				data.first.servicetype,
				eport,
				(pImpl->connectionType_ == ConnectionType::UDP) ? "UDP" : "TCP",
				lanaddr,
				intClient,
				intPort,
				desc,
				enabled,
				leaseDuration);


			if (r != UPNPCOMMAND_SUCCESS)
			{
				printf("GetSpecificPortMappingEntry() failed with code %d (%s)\n",
					r, strupnperror(r));
			}
			else
			{
				return UPNPResult::UPNP_SUCCESS;
			}

		}
		else
		{
			return UPNPResult::UPNP_NOIGD;
		}
	}
	else
	{
		return UPNPResult::UPNP_NODEVICES;
	}

	return UPNPResult::UPNP_SUCCESS;
}