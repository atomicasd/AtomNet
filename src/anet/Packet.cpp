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
//
////////////////////////////////////////////////////////////

#pragma comment(lib, "Ws2_32.lib")

#include "anet/Packet.h"

#include <string>
#include <cstring>
#include <cwchar>
#include <WinSock2.h>
using namespace anet;

Packet::Packet() :
data_(new Data())
{
	data_->readPos = 0;
	data_->isValid = true;
}

Packet::Packet(Packet& packet) :
data_(packet.data_)
{

}

Packet::Packet(const void* data, size_t size): Packet()
{
	Append(data, size);
}

Packet::~Packet()
{

}

////////////////////////////////////////////////////////////
void Packet::Append(const void* data, std::size_t sizeInBytes)
{
	if (data && (sizeInBytes > 0))
	{
		std::size_t start = data_->data.size();
		data_->data.resize(start + sizeInBytes);
		std::memcpy(&data_->data[start], data, sizeInBytes);
	}
}


////////////////////////////////////////////////////////////
void Packet::Clear()
{
	data_->data.clear();
	data_->readPos = 0;
	data_->isValid = true;
}


////////////////////////////////////////////////////////////
const void* Packet::GetData() const
{
	return !data_->data.empty() ? &data_->data[0] : NULL;
}


////////////////////////////////////////////////////////////
std::size_t Packet::GetDataSize() const
{
	return data_->data.size();
}


////////////////////////////////////////////////////////////
bool Packet::EndOfPacket() const
{
	return data_->readPos >= data_->data.size();
}


////////////////////////////////////////////////////////////
Packet::operator BoolType() const
{
	return data_->isValid ? &Packet::CheckSize : NULL;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(bool& data)
{
	Uint8 value;
	if (*this >> value)
		data = (value != 0);

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Int8& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const Int8*>(&data_->data[data_->readPos]);
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Uint8& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const Uint8*>(&data_->data[data_->readPos]);
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Int16& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = ntohs(*reinterpret_cast<const Int16*>(&data_->data[data_->readPos]));
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Uint16& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = ntohs(*reinterpret_cast<const Uint16*>(&data_->data[data_->readPos]));
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Int32& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = ntohl(*reinterpret_cast<const Int32*>(&data_->data[data_->readPos]));
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Uint32& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = ntohl(*reinterpret_cast<const Uint32*>(&data_->data[data_->readPos]));
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(float& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const float*>(&data_->data[data_->readPos]);
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(double& data)
{
	if (CheckSize(sizeof(data)))
	{
		data = *reinterpret_cast<const double*>(&data_->data[data_->readPos]);
		data_->readPos += sizeof(data);
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(char* data)
{
	// First extract string length
	Uint32 length = 0;
	*this >> length;

	if ((length > 0) && CheckSize(length))
	{
		// Then extract characters
		std::memcpy(data, &data_->data[data_->readPos], length);
		data[length] = '\0';

		// Update reading position
		data_->readPos += length;
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(std::string& data)
{
	// First extract string length
	Uint32 length = 0;
	*this >> length;

	data.clear();
	if ((length > 0) && CheckSize(length))
	{
		// Then extract characters
		data.assign(&data_->data[data_->readPos], length);

		// Update reading position
		data_->readPos += length;
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(wchar_t* data)
{
	// First extract string length
	Uint32 length = 0;
	*this >> length;

	if ((length > 0) && CheckSize(length * sizeof(Uint32)))
	{
		// Then extract characters
		for (Uint32 i = 0; i < length; ++i)
		{
			Uint32 character = 0;
			*this >> character;
			data[i] = static_cast<wchar_t>(character);
		}
		data[length] = L'\0';
	}

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(std::wstring& data)
{
	// First extract string length
	Uint32 length = 0;
	*this >> length;

	data.clear();
	if ((length > 0) && CheckSize(length * sizeof(Uint32)))
	{
		// Then extract characters
		for (Uint32 i = 0; i < length; ++i)
		{
			Uint32 character = 0;
			*this >> character;
			data += static_cast<wchar_t>(character);
		}
	}

	return *this;
}

////////////////////////////////////////////////////////////
Packet& Packet::operator <<(bool data)
{
	*this << static_cast<Uint8>(data);
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Int8 data)
{
	Append(&data, sizeof(data));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Uint8 data)
{
	Append(&data, sizeof(data));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Int16 data)
{
	Int16 toWrite = htons(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Uint16 data)
{
	Uint16 toWrite = htons(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Int32 data)
{
	Int32 toWrite = htonl(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Uint32 data)
{
	Uint32 toWrite = htonl(data);
	Append(&toWrite, sizeof(toWrite));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(float data)
{
	Append(&data, sizeof(data));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(double data)
{
	Append(&data, sizeof(data));
	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(const char* data)
{
	// First insert string length
	Uint32 length = static_cast<Uint32>(std::strlen(data));
	*this << length;

	// Then insert characters
	Append(data, length * sizeof(char));

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(const std::string& data)
{
	// First insert string length
	Uint32 length = static_cast<Uint32>(data.size());
	*this << length;

	// Then insert characters
	if (length > 0)
		Append(data.c_str(), length * sizeof(std::string::value_type));

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(const wchar_t* data)
{
	// First insert string length
	Uint32 length = static_cast<Uint32>(std::wcslen(data));
	*this << length;

	// Then insert characters
	for (const wchar_t* c = data; *c != L'\0'; ++c)
		*this << static_cast<Uint32>(*c);

	return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(const std::wstring& data)
{
	// First insert string length
	Uint32 length = static_cast<Uint32>(data.size());
	*this << length;

	// Then insert characters
	if (length > 0)
	{
		for (std::wstring::const_iterator c = data.begin(); c != data.end(); ++c)
			*this << static_cast<Uint32>(*c);
	}

	return *this;
}


////////////////////////////////////////////////////////////
bool Packet::CheckSize(std::size_t size)
{
	data_->isValid = data_->isValid && (data_->readPos + size <= data_->data.size());

	return data_->isValid;
}