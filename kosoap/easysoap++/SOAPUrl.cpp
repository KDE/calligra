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
#include "SOAPUrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const unsigned int ftp_default_port = 21;
static const unsigned int http_default_port = 80;
static const unsigned int https_default_port = 443;

void
SOAPUrl::Reset()
{
	m_proto = no_proto;

	m_user = "";
	m_password = "";
	m_hostname = "";

	m_port = 0;

	m_path = "";
}

void
SOAPUrl::Set(const char *url)
{
	//
	// urls are of the form:
	// proto://[user[:password]@]hostname[:port]/[path]
	//

	const char * const keepurl = url;

	Reset();
	if (url)
	{
		char buffer[1024];
		char *work;

		const char * const workend = buffer + 1023;
		const char * const urlend = url + sp_strlen(url);

		// parse out the protocol name
		work = buffer;
		while (url != urlend && *url != ':' && *url != '/' && work != workend)
			*work++ = *url++;
		*work = 0;

		// found protocol name
		if (*url++ == ':')
		{
			if (!sp_strcasecmp(buffer, "http"))
			{
				m_proto = http_proto;
				m_port = http_default_port;
			}
			else if (!sp_strcasecmp(buffer, "https"))
			{
				m_proto = https_proto;
				m_port = https_default_port;
			}
			else if (!sp_strcasecmp(buffer, "ftp"))
			{
				m_proto = ftp_proto;
				m_port = ftp_default_port;
			}
			else if (!sp_strcasecmp(buffer, "file"))
			{
				m_proto = file_proto;
				m_port = 0;
			}
			else
			{
				m_proto = unknown_proto;
				throw SOAPException("Unknown protocol: %s", buffer);
			}
			m_base = buffer;
			m_base.Append("://");
		}
		else
		{
			throw SOAPException("Cannot parse url, expecting ':' after protocol name: %s", keepurl);
		}

		// pass over the '//' part
		if (*url++ != '/' || *url++ != '/')
			throw SOAPException("Cannot parse url, expecting '://' after protocol name: %s", keepurl);

		// parse out the [user[:password]@]host[:port] part

		char *colon = 0;
		work = buffer;
		while (url != urlend && *url != '/' && *url != '@' && work != workend)
		{
			if (*url == ':' && !colon)
				colon = work;
			*work++ = *url++;
		}
		*work = 0;

		if (*url == '@')
		{
			// we have 'user:password' part
			m_base.Append(buffer);
			m_base.Append("@");
			++url;

			if (colon)
			{
				// we have both a user and a password;
				*colon++ = 0;
				m_password = colon;
			}
			m_user = buffer;

			// parse through the rest of the url, the 'hostname:port' part
			colon = 0;
			work = buffer;
			while (url != urlend && *url != '/' && work != workend)
			{
				if (*url == ':' && !colon)
					colon = work;
				*work++ = *url++;
			}
			*work = 0;
		}

		if (*url == '/' || *url == 0)
		{
			// we have 'hostname:port' part
			m_base.Append(buffer);
			m_base.Append("/");
			if (colon)
			{
				// we have both a hostname and a port;
				*colon++ = 0;
				m_port = atoi(colon);
			}
			m_hostname = buffer;
		}
		else
		{
			throw SOAPException("Could not parse url: %s", keepurl);
		}

		if (*url)
			m_path = url;
		else
			m_path = "/";

	m_url = keepurl;

	}
}

bool
SOAPUrl::PortIsDefault() const
{
	switch (m_proto)
	{
	case ftp_proto: return m_port == ftp_default_port;
	case http_proto: return m_port == http_default_port;
	case https_proto: return m_port == https_default_port;
	default: return false;
	}
}

