/* 
 * EasySoap++ - A C++ library for SOAP (Simple Object Access Protocol)
 * Copyright (C) 2001 David Crowley; SciTegic, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


//
//TODO:
//  #define or make static const char *'s all the hard coded strings
//

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include "SOAPSocket.h"
#include "SOAPClientSocketImp.h"
#include "SOAPSecureSocketImp.h"

SOAPProtocolBase::SOAPProtocolBase()
	: m_socket(0)
	, m_buff(0)
	, m_buffend(0)
	, m_wpos(0)
	, m_wend(0)
{
}

SOAPProtocolBase::~SOAPProtocolBase()
{
	Close();
}

void
SOAPProtocolBase::Close()
{
	delete m_socket;
	m_socket = 0;
	m_buff = 0;
	m_buffend = 0;
	m_wpos = 0;
	m_wend = 0;
}

void
SOAPProtocolBase::SetSocket(SOAPSocketInterface *socket)
{
	Close();
	m_socket = socket;
	m_wpos = m_wbuff;
	m_wend = m_wpos + sizeof(m_wbuff);
}

bool
SOAPProtocolBase::Connect(const char *host, unsigned int port, bool secure)
{
	Close();

	if (secure)
		m_socket = new SOAPSecureSocketImp();
	else
		m_socket = new SOAPClientSocketImp();

	if (!m_socket)
		throw SOAPMemoryException();

	bool ret = m_socket->Connect(host, port);
	if (ret)
	{
		m_wpos = m_wbuff;
		m_wend = m_wpos + sizeof(m_wbuff);
	}
	return ret;
}

bool
SOAPProtocolBase::Readbuff()
{
	Flush(); // in case we haven't sent everything
	m_buff = 0;
	m_buffend = 0;
	int bytes = m_socket->Read(m_buffer, sizeof(m_buffer));
	if (bytes > 0)
	{
		m_buff = m_buffer;
		m_buffend = m_buffer + bytes;
	}

	return bytes > 0;
}

size_t
SOAPProtocolBase::Read(char *buffer, size_t len)
{
	if (!m_socket)
		throw SOAPSocketException("Protocol doesn't have a socket.");

	Flush(); // in case we haven't sent everything
	if (m_buff != m_buffend)
	{
		size_t numread = 0;
		while (m_buff != m_buffend && numread < len)
		{
			*buffer++ = *m_buff++;
			++numread;
		}
		return numread;
	}
	return m_socket->Read(buffer, len);
}

size_t
SOAPProtocolBase::ReadLine(char *buff, size_t bufflen)
{
	if (!m_socket)
		throw SOAPSocketException("Protocol doesn't have a socket.");

	char *end = buff + bufflen;
	size_t numread = 0;
	char c;
	while (buff != end)
	{
		if (m_buff == m_buffend)
		{
			if (!Readbuff())
				break;
		}
		else
		{
			c = *m_buff++;
			if (c == '\r' || c == '\n')
			{
				if (c == '\r' && (m_buff != m_buffend || Readbuff()) && *m_buff == '\n')
					++m_buff;
				*buff = 0;
				break;
			}
			else
			{
				*buff++ = c;
				++numread;
			}
		}
	}

	return numread;
}

void
SOAPProtocolBase::Flush()
{
	if (m_socket && m_wpos != m_wbuff)
	{
		int bufflen = m_wpos - m_wbuff;
		m_socket->Write(m_wbuff, bufflen);
		m_wpos = m_wbuff;
	}
}

size_t
SOAPProtocolBase::Write(const char *buff, size_t bufflen)
{
	if (!m_socket)
		throw SOAPSocketException("Protocol doesn't have a socket.");

	const char *bend = buff + bufflen;
	while (buff != bend)
	{
		if (m_wpos == m_wend)
			Flush();
		*m_wpos++ = *buff++;
	}
	return bufflen;
}

size_t
SOAPProtocolBase::Write(const char *str)
{
	if (str)
	{
		size_t len = sp_strlen(str);
		return Write(str, len);
	}
	return 0;
}

size_t
SOAPProtocolBase::WriteLine(const char *str)
{
	size_t ret = 0;
	if (str)
		ret = Write(str);
	ret += Write("\r\n", 2);
	return ret;
}
