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


#if !defined(AFX_SOAPURL_H__592D82D9_B93F_431A_BB8D_F658609814C4__INCLUDED_)
#define AFX_SOAPURL_H__592D82D9_B93F_431A_BB8D_F658609814C4__INCLUDED_

#include "SOAP.h"

class EASYSOAP_EXPORT SOAPUrl  
{
public:
	SOAPUrl(const char *url = 0) : m_proto(no_proto), m_port(0) {Set(url);}
	~SOAPUrl() {}

	void Set(const char *url);
	SOAPUrl& operator=(const char *url) {Set(url); return *this;}

	typedef enum {
		no_proto,		// not set
		file_proto,		// file system
		http_proto,		// http
		https_proto,	// secure http
		ftp_proto,		// ftp
		unknown_proto	// unknown, couldn't parse
	} UrlProtocol;

	UrlProtocol	Protocol() const		{return m_proto;}
	const SOAPString& User() const		{return m_user;}
	const SOAPString& Password() const	{return m_password;}
	const SOAPString& Hostname() const	{return m_hostname;}
	unsigned int Port() const			{return m_port;}
	bool PortIsDefault() const;
	const SOAPString& Path() const		{return m_path;}

	const SOAPString& GetString() const		{return m_url;}
	const SOAPString& GetBaseString() const	{return m_base;}

private:
	void		Reset();

	SOAPString		m_url;		// Full URL the user gave us
	SOAPString		m_base;		// Just the base part (no path)

	UrlProtocol		m_proto;	// Enum for the proto
	SOAPString		m_user;		// Username in URL (if any)
	SOAPString		m_password;	// Password in URL (if any)
	SOAPString		m_hostname; // Hostname in URL
	unsigned int	m_port;		// Port given in URL, or default
	SOAPString		m_path;		// The URL Path
};

#endif // !defined(AFX_SOAPURL_H__592D82D9_B93F_431A_BB8D_F658609814C4__INCLUDED_)

