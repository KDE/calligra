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


#if !defined(AFX_SIMPLESOCKET_H__10CE9068_BA2E_4BE4_8AE2_D48D2FCA40AF__INCLUDED_)
#define AFX_SIMPLESOCKET_H__10CE9068_BA2E_4BE4_8AE2_D48D2FCA40AF__INCLUDED_

#include "SOAP.h"
#include "SOAPUrl.h"

class EASYSOAP_EXPORT SOAPSocketException : public SOAPException
{
public:
	SOAPSocketException(const SOAPString& what)
		: SOAPException(what) {}
	SOAPSocketException(const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		FormattedMessage(fmt, args);
		va_end(args);
	}
	~SOAPSocketException() {}
};



class SOAPSocketInterface
{
public:
	virtual ~SOAPSocketInterface() {}

	// -1 means blocking
	virtual bool WaitRead(int sec = -1, int usec = 0) = 0;
	virtual bool WaitWrite(int sec = -1, int usec = 0) = 0;
	virtual bool IsOpen() = 0;
	virtual void Close() = 0;
	virtual bool Connect(const char *host, unsigned int port) = 0;
	virtual size_t Read(char *buffer, size_t len) = 0;
	virtual size_t Write(const char *buffer, size_t len) = 0;

protected:
	SOAPSocketInterface() {}
};



class EASYSOAP_EXPORT SOAPProtocolBase
{
private:

	char		m_buffer[1024];
	char		m_wbuff[1460];

	SOAPSocketInterface	*m_socket;
	const char	*m_buff;
	const char	*m_buffend;
	char		*m_wpos;
	const char  *m_wend;

	bool	Readbuff();

protected:
	void	Flush();

public:
	SOAPProtocolBase();
	virtual ~SOAPProtocolBase();

	void SetSocket(SOAPSocketInterface *socket);

	virtual bool CanRead() {return IsOpen() && (m_buff != m_buffend || m_socket->WaitRead(0));}
	virtual bool IsOpen() {return m_socket && m_socket->IsOpen();}
	virtual void Close();
	virtual bool Connect(const char *host, unsigned int port, bool secure = false);

	// read up to len chars
	virtual size_t Read(char *buffer, size_t len);

	// reads up to \r\n but doesn't add it to string
	virtual size_t ReadLine(char *buff, size_t bufflen);

	// write up to terminating null
	virtual size_t Write(const char *buffer);

	// write len chars
	virtual size_t Write(const char *buffer, size_t len);

	// write string plus \r\n
	virtual size_t WriteLine(const char *str = 0);
};

#endif // !defined(AFX_SIMPLESOCKET_H__10CE9068_BA2E_4BE4_8AE2_D48D2FCA40AF__INCLUDED_)

