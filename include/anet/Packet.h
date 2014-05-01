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
//	NOTICE: This file is an edited version of Packet.hpp created by Laurent Gomila
//	The purpose of this file is to provide a pimpl interface that functions similar
//	to the original sf::Packet class
//
////////////////////////////////////////////////////////////

#pragma once 

#include <string>
#include <vector>
namespace anet
{
	// 8 bits integer types
    typedef signed   char Int8;
    typedef unsigned char Uint8;

    // 16 bits integer types
    typedef signed   short Int16;
    typedef unsigned short Uint16;

    // 32 bits integer types
    typedef signed   int Int32;
    typedef unsigned int Uint32;

    // 64 bits integer types
    #if defined(_MSC_VER)
        typedef signed   __int64 Int64;
        typedef unsigned __int64 Uint64;
    #else
        typedef signed   long long Int64;
        typedef unsigned long long Uint64;
    #endif

	class Packet
	{
		typedef bool (Packet::*BoolType)(std::size_t);

	public :
		Packet();
		Packet(const void* data, size_t size);

		virtual ~Packet();

		void Append(const void* data, std::size_t sizeInBytes);

		void Clear();

		const void* GetData() const;

		std::size_t GetDataSize() const;

		bool EndOfPacket() const;

	public:

		operator BoolType() const;

		Packet& operator >>(bool&         data);
		Packet& operator >>(Int8&         data);
		Packet& operator >>(Uint8&        data);
		Packet& operator >>(Int16&        data);
		Packet& operator >>(Uint16&       data);
		Packet& operator >>(Int32&        data);
		Packet& operator >>(Uint32&       data);
		Packet& operator >>(float&        data);
		Packet& operator >>(double&       data);
		Packet& operator >>(char*         data);
		Packet& operator >>(std::string&  data);
		Packet& operator >>(wchar_t*      data);
		Packet& operator >>(std::wstring& data);


		Packet& operator <<(bool                data);
		Packet& operator <<(Int8                data);
		Packet& operator <<(Uint8               data);
		Packet& operator <<(Int16               data);
		Packet& operator <<(Uint16              data);
		Packet& operator <<(Int32               data);
		Packet& operator <<(Uint32              data);
		Packet& operator <<(float               data);
		Packet& operator <<(double              data);
		Packet& operator <<(const char*         data);
		Packet& operator <<(const std::string&  data);
		Packet& operator <<(const wchar_t*      data);
		Packet& operator <<(const std::wstring& data);

	private :
		class Impl;
		Impl* pImpl;
};
}