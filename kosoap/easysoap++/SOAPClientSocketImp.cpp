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


#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif // _MSC_VER

#include "SOAPClientSocketImp.h"
#include "SOAPDebugger.h"

#if defined (_WIN32)
#if !defined (__MWERKS__)
#include <winsock2.h>
#endif

//
// Initialize Windows socket library
//
static class WinSockInit
{
private:
	bool didinit;
public:
	WinSockInit() : didinit(false)
	{
	}

	void Init()
	{
		if (!didinit)
		{
			WSADATA wsaData;
			// Is version 0x0202 appropriate?
			// I have no idea...
			WSAStartup( 0x0202, &wsaData );
			didinit = true;
		}
	}

	~WinSockInit()
	{
		WSACleanup();
	}
} __winsockinit;

#elif defined(__CYGWIN__)
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#define INVALID_SOCKET 0xFFFFFFFF
#define SOCKET_ERROR 0xFFFFFFFF
#define closesocket close

#else // not _WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <unistd.h>

#define INVALID_SOCKET 0xFFFFFFFF
#define SOCKET_ERROR -1
#define closesocket close

#endif // _WIN32

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPClientSocketImp::SOAPClientSocketImp()
: m_socket(INVALID_SOCKET)
{
#if defined (_WIN32)
	__winsockinit.Init();
#endif;
}

SOAPClientSocketImp::~SOAPClientSocketImp()
{
	Close();
}

void
SOAPClientSocketImp::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}

	m_socket = INVALID_SOCKET;
}


bool
SOAPClientSocketImp::WaitRead(int sec, int usec)
{
	struct timeval tv;
	fd_set rset, eset;

	FD_ZERO(&rset);
	FD_SET(m_socket, &rset);
	FD_ZERO(&eset);
	FD_SET(m_socket, &eset);

	tv.tv_sec = sec;
	tv.tv_usec = usec;

	int ret = select(m_socket+1, &rset, 0, &eset, sec == -1 ? 0 : &tv);
	int rsetresult = FD_ISSET(m_socket, &rset);
	int esetresult = FD_ISSET(m_socket, &eset);
	SOAPDebugger::Print(3, "read select() return: %d\n", ret);
	SOAPDebugger::Print(4, "write select() wset: %d\n", rsetresult);
	SOAPDebugger::Print(4, "write select() eset: %d\n", esetresult);
	if (ret == (int)SOCKET_ERROR)
		throw SOAPException("WaitRead select error");

	return rsetresult != 0;
}

bool
SOAPClientSocketImp::WaitWrite(int sec, int usec)
{
	struct timeval tv;
	fd_set wset, eset;

	FD_ZERO(&eset);
	FD_SET(m_socket, &eset);
	FD_ZERO(&wset);
	FD_SET(m_socket, &wset);

	tv.tv_sec = sec;
	tv.tv_usec = usec;

	int ret = select(m_socket+1, 0, &wset, &eset, sec == -1 ? 0 : &tv);
	int wsetresult = FD_ISSET(m_socket, &wset);
	int esetresult = FD_ISSET(m_socket, &eset);
	SOAPDebugger::Print(3, "write select() return: %d\n", ret);
	SOAPDebugger::Print(4, "write select() wset: %d\n", wsetresult);
	SOAPDebugger::Print(4, "write select() eset: %d\n", esetresult);
	if (ret == (int)SOCKET_ERROR)
		throw SOAPException("WaitWrite select error");

	return wsetresult != 0;
}

bool
SOAPClientSocketImp::IsOpen()
{
	return m_socket != INVALID_SOCKET;
}


bool
SOAPClientSocketImp::Connect(const char *server, unsigned int port)
{
	Close();

	//
	// TODO: Abstract this away into other class/methods
	//
	m_socket = 0;
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		throw SOAPSocketException("Error creating socket");

	struct sockaddr_in sockAddr;
	sp_memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(m_socket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == (int)SOCKET_ERROR)
		throw SOAPSocketException("Error binding socket");

	sp_memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(server);
	sockAddr.sin_port = htons((u_short)port);
	if (sockAddr.sin_addr.s_addr == (unsigned int)-1)
	{
		struct hostent *lphost = gethostbyname(server);
		if (lphost != NULL)
		{
			sockAddr.sin_addr.s_addr = ((struct in_addr *)lphost->h_addr)->s_addr;
		}
		else
		{
			Close();
			throw SOAPSocketException("Could not resolve host name: %s", server);
		}
	}

	if (connect(m_socket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == (int)SOCKET_ERROR)
	{
		Close();
		throw SOAPSocketException("Connection refused to host: %s:%d", server, port);
	}

	int nodelay = 1;
	struct protoent *tcpproto = getprotobyname("tcp");
	if (!tcpproto)
	{
		throw SOAPSocketException("Could not get TCP proto struct.");
	}

	if (setsockopt(m_socket, tcpproto->p_proto, TCP_NODELAY, (const char *)&nodelay, sizeof(nodelay)) == -1)
	{
		throw SOAPSocketException("Could not set TCP_NODELAY");
	}

	return true;
}

size_t
SOAPClientSocketImp::Read(char *buff, size_t bufflen)
{
	int bytes = 0;
	if (bufflen > 0)
	{
		*buff = 0;
		bytes = recv(m_socket, buff, bufflen, 0);
		SOAPDebugger::Print(2, "RECV: %d bytes\n", bytes);
		if (bytes == 0)
		{
			Close(); // other side dropped the connection
		}
		else if (bytes == (int)SOCKET_ERROR)
		{
			Close();
			throw SOAPSocketException("Error reading socket");
		}
		SOAPDebugger::Write(1, buff, bytes);
	}
	return bytes;
}

size_t
SOAPClientSocketImp::Write(const char *buff, size_t bufflen)
{
	int bytes = 0;
	if (bufflen > 0)
	{
		bytes = send(m_socket, buff, bufflen, 0);
		SOAPDebugger::Print(2, "SEND: %d bytes\n", bytes);
		if (bytes == (int)SOCKET_ERROR)
		{
			Close();
			throw SOAPSocketException("Error writing to socket");
		}
		else if (bytes != (int)bufflen)
		{
			Close();
			throw SOAPSocketException("Error writing to socket, "
					"tried to write %d bytes, wrote %d",
					bufflen, bytes);
		}
		SOAPDebugger::Write(1, buff, bytes);
	}
	return bytes;
}

