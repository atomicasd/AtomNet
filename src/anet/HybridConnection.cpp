#include "anet/HybridConnection.h"

#include <memory>
#include <assert.h>
#include <boost/signals2.hpp>
using namespace anet;

class HybridConnection::Impl
{
public:
	Impl();

	char* ip_;
	unsigned short port_;

	std::function<void(bool)> callback_;

	void CallbackClientConnectionResult(bool result);
};

HybridConnection::Impl::Impl() 
{

}

HybridConnection::HybridConnection(std::function<void(bool)> onServerConnectedResult) :
	pImpl(new Impl())
{
	pImpl->callback_ = onServerConnectedResult;
}