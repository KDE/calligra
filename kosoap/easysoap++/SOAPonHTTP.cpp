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

#include "SOAP.h"
#include "SOAPonHTTP.h"
#include "SOAPEnvelope.h"
#include "SOAPSecureSocketImp.h"
#include "SOAPDebugger.h"

#ifndef SOAPUSER_AGENT
#define SOAPUSER_AGENT EASYSOAP_STRING "/" EASYSOAP_VERSION_STRING
#endif // SOAPUSER_AGENT

// read the payload into the buffer.
// can be called multiple times.
// returns 0 if entire payload has been read.

void
SOAPonHTTP::SetError()
{
}

size_t
SOAPonHTTP::Read(char *buffer, size_t buffsize)
{
	return m_http.Read(buffer, buffsize);
}

// send the payload.  can only be called ONCE per
// payload. 
//
size_t
SOAPonHTTP::Write(const SOAPMethod& method, const char *payload, size_t payloadsize)
{
	m_http.BeginPost(m_path);
	m_http.WriteHeader("User-Agent", SOAPUSER_AGENT);
	m_http.WriteHeader("Content-Type", "text/xml; charset=\"UTF-8\"");

	m_http.Write("SOAPAction: \"");
	m_http.Write(method.GetSoapAction());
	m_http.WriteLine("\"");

	int ret = m_http.PostData(payload, payloadsize);
	bool isxml = true;

	const char *contype = m_http.GetHeader("Content-Type");
	if (contype)
		isxml = (sp_strstr(contype, "text/xml") != 0);

	if (ret != 200 && !isxml)
		throw SOAPException("Unexpected return code: %s",
			(const char *)m_http.GetRequestMessage());

	if (!isxml)
		throw SOAPException("Unexpected return Content-Type: %s", contype);

	return ret;
}

const char *
SOAPonHTTP::GetCharset() const
{
	return m_http.GetCharset();
}

void
SOAPHTTPProtocol::ConnectTo(const SOAPUrl& endpoint, const SOAPUrl& proxy)
{
	Close();
	m_endpoint = endpoint;
	m_proxy = proxy;
}

void
SOAPHTTPProtocol::ConnectTo(const SOAPUrl& endpoint)
{
	Close();
	m_endpoint = endpoint;
	m_proxy = SOAPUrl();
}

void
SOAPHTTPProtocol::WriteHostHeader(const SOAPUrl& url)
{
	if (url.PortIsDefault())
		WriteHeader("Host", url.Hostname());
	else
	{
		char buffer[256];
		snprintf(buffer, sizeof(buffer), "%s:%d",
				(const char *)url.Hostname(),
				url.Port());
		WriteHeader("Host", buffer);
	}
}

int
SOAPHTTPProtocol::Get(const char *path)
{
	StartVerb("GET", path);
	WriteLine("");
	int ret = GetReply();
	if (ret == 100)
		ret = GetReply();
	return ret;
}

void
SOAPHTTPProtocol::FlushInput()
{
	char buffer[256];
	while (CanRead())
	{
		size_t read = Read(buffer, sizeof(buffer));
		if (read == 0)
		{
			Close();
		}
	}

	if (m_doclose)
		Close();
}

void
SOAPHTTPProtocol::StartVerb(const char *verb, const char *path)
{
	if (!path)
		throw SOAPException("Invalid NULL path");

	FlushInput();
	SOAPDebugger::Print(1, "\n\nREQUEST:\n");

	if (!Connect())
		throw SOAPSocketException("Unable to make socket connection");
	Write(verb);
	Write(" ");

	if (m_httpproxy)
	{
		Write(m_endpoint.GetBaseString());
		if (*path == '/')
			++path;
		Write(path);
	}
	else
		Write(path);

	WriteLine(" HTTP/1.1");
	WriteHostHeader(m_endpoint);

	if (m_keepAlive)
		WriteHeader("Connection", "Keep-Alive");
}

void
SOAPHTTPProtocol::BeginPost(const char *path)
{

	StartVerb("POST", path);
}

int
SOAPHTTPProtocol::PostData(const char *bytes, int len)
{
	WriteHeader("Content-Length", len);
	WriteLine("");
	Write(bytes, len);
	int ret = GetReply();
	if (ret == 100)
		ret = GetReply();
	return ret;
}

void
SOAPHTTPProtocol::WriteHeader(const char *header, const char *value)
{
	Write(header);
	Write(": ");
	WriteLine(value);
}

void
SOAPHTTPProtocol::WriteHeader(const char *header, int value)
{
	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%d", value);
	WriteHeader(header, buffer);
}

int
SOAPHTTPProtocol::GetReply()
{
	Flush();
	SOAPDebugger::Print(1, "\n\nRESPONSE:\n");
	char buff[2048];
	m_headers.Clear();

	int httpreturn = 500;
	int respver = 0;

	// This should read a line that is something like:
	// HTTP/1.1 200 OK
	if (ReadLine(buff, sizeof(buff)) == 0)
	{
		throw SOAPSocketException("Couldn't read response.");
	}

	const char *vers = sp_strchr(buff, '/');
	if (vers)
	{
		respver += atoi(++vers) * 10;
		if ((vers = sp_strchr(vers, '.')))
			respver += atoi(++vers);
	}

	const char *httpretcode = sp_strchr(buff, ' ');
	if (httpretcode)
	{
		m_httpmsg = ++httpretcode;
		httpreturn = atoi(httpretcode);
		if (httpreturn == 0)
			httpreturn = 500;
	}
	else
		m_httpmsg = buff;

	while (1)
	{
		if (ReadLine(buff, sizeof(buff)) == 0)
			break;

		if (buff[0] == 0)
			break;
		char *c = sp_strchr(buff, ':');
		if (c)
		{
			*c++ = 0;
			while (*c == ' ')
				*c++ = 0;
			m_headers[buff] = c;
		}
		else
		{
			// I don't think we should get here...
		}
	}

	//
	// Save charset info
	//
	// Per some RFC, encoding is us-ascii if it's not
	// specifief in HTTP header.
	m_charset = "US-ASCII";
	const char *contype = GetHeader("Content-Type");
	if (contype)
	{
		const char *charset = charset = sp_strstr(contype, "charset=");
		if (charset)
		{
			charset += 8;
			if (*charset == '\"')
				++charset;
			const char *end = charset;

			while (*end && *end != '\"' && *end != ';' && *end != ' ')
				++end;

			m_charset = "";
			m_charset.Append(charset, end - charset);
		}
	}

	//
	// Get some information so we can close the
	// the connection gracefully if we need to..
	//
	m_canread = GetContentLength();
	m_doclose = false;
	const char *keepalive = GetHeader("Connection");
	if (respver > 10)
	{
		if (keepalive && sp_strcasecmp(keepalive, "Keep-Alive") != 0)
			m_doclose = true;
	}
	else
	{
		if (!keepalive || sp_strcasecmp(keepalive, "Keep-Alive") != 0)
			m_doclose = true;
	}

	if (!m_keepAlive)
		m_doclose = true;

	return httpreturn;
}

const char *
SOAPHTTPProtocol::GetHeader(const char *header) const
{
	HeaderMap::Iterator i = m_headers.Find(header);
	if (i != m_headers.End())
	{
		return (const char *)(*i);
	}
	return 0;
}

int
SOAPHTTPProtocol::GetContentLength() const
{
	const char *header = GetHeader("Content-Length");
	int len = -1;
	if (header)
		len = atoi(header);
	return len;
}

void
SOAPHTTPProtocol::Close()
{
	m_canread = -1;
	m_doclose = false;
	super::Close();
	SOAPDebugger::Print(2, "CLOSED\n");
}

bool
SOAPHTTPProtocol::CanRead()
{
	return m_canread > 0 || super::CanRead();
	//return (m_canread == -1 && super::CanRead()) || m_canread > 0;
}

size_t
SOAPHTTPProtocol::Read(char *buffer, int len)
{
	size_t ret = 0;
	if (m_canread != 0)
	{
		ret = super::Read(buffer, len);
		if (m_canread > 0)
		{
			m_canread -= ret;
			if (m_canread == 0 && m_doclose)
			{
				Close();
			}
		}
	}
	return ret;
}

bool
SOAPHTTPProtocol::Connect()
{
	if (!IsOpen())
	{
		// See if we have to talk through an HTTP proxy
		m_httpproxy = (m_proxy.Protocol() == SOAPUrl::http_proto);

		int port					= m_httpproxy ? m_proxy.Port()		: m_endpoint.Port();
		const char *host			= m_httpproxy ? m_proxy.Hostname()	: m_endpoint.Hostname();
		SOAPUrl::UrlProtocol proto	= m_endpoint.Protocol();

		switch (proto)
		{
		case SOAPUrl::http_proto:
			SOAPProtocolBase::Connect(host, port, false);
			break;
		case SOAPUrl::https_proto:
			{
				SOAPSecureSocketImp *socket = new SOAPSecureSocketImp();
				socket->SOAPClientSocketImp::Connect(host, port);
				SOAPProtocolBase::SetSocket(socket);
				if (m_httpproxy)
				{
					char buffer[1024];
					snprintf(buffer, 1024, "CONNECT %s:%d HTTP/1.0",
							(const char *)m_endpoint.Hostname(),
							m_endpoint.Port());
					WriteLine(buffer);
					WriteHostHeader(m_endpoint);
					WriteLine("");

					if (GetReply() != 200)
						throw SOAPException("Error setting up tunnel through proxy: %s",
							(const char *)m_httpmsg);
					//
					// we turn the proxy flag off because at this point
					// we have a connection which looks like a direct
					// connection.  So we don't have to do anything
					// fancy with the GET/POST commands.
					m_httpproxy = false;
				}
				socket->InitSSL();
			}
			break;
		default:
			throw SOAPSocketException("Can only handle HTTP protocols");
			break;
		}
		return IsOpen();
	}

	return true;
}
