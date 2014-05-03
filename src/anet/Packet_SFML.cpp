////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2013 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//



////////////////////////////////////////////////////////////
//
//	NOTICE: This file is an edited version of Packet.cpp created by Laurent Gomila
//	The purpose of this file is to provide a pimpl implementation that functions 
//	as an interface to the original sf::Packet class
//
////////////////////////////////////////////////////////////

#include "anet/Packet.h"
#include <SFML/Network/Packet.hpp>
using namespace anet;

class Packet::Impl
{
public:
	Impl();
	
	sf::Packet packet;
};

Packet::Impl::Impl()
{

}

Packet::Packet()
	: pImpl(new Impl())
{

}

Packet::~Packet()
{
	delete pImpl;
}

Packet::Packet(const void* data, size_t size)
	: pImpl(new Impl())
{
	pImpl->packet.append(data, size);
}

void Packet::Append(const void* data, std::size_t sizeInBytes)
{
	pImpl->packet.append(data, sizeInBytes);
}

void Packet::Clear()
{
	pImpl->packet.clear();
}

const void* Packet::GetData() const
{
	return pImpl->packet.getData();
}

std::size_t Packet::GetDataSize() const
{
	return pImpl->packet.getDataSize();
}

bool Packet::EndOfPacket() const
{
	return pImpl->packet.endOfPacket();
}

Packet& Packet::operator >>(bool& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(Int8& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(Uint8& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(Int16& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(Uint16& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(Int32& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(Uint32& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(float& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(double& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(char* data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(std::string& data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(wchar_t* data)
{
    pImpl->packet >> data;
    return *this;
}

Packet& Packet::operator >>(std::wstring& data)
{
    pImpl->packet >> data;
    return *this;
}



Packet& Packet::operator <<(bool data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(Int8 data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(Uint8 data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(Int16 data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(Uint16 data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(Int32 data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(Uint32 data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(float data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(double data)
{
    pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(const char* data)
{
    pImpl->packet << data;

    return *this;
}

Packet& Packet::operator <<(const std::string& data)
{
	pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(const wchar_t* data)
{
	pImpl->packet << data;
    return *this;
}

Packet& Packet::operator <<(const std::wstring& data)
{
    pImpl->packet << data;
    return *this;
}
