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


#if !defined(AFX_SOAPPROXY_H__D489F911_F39E_4C38_A5C0_95CD3D0A9459__INCLUDED_)
#define AFX_SOAPPROXY_H__D489F911_F39E_4C38_A5C0_95CD3D0A9459__INCLUDED_

#include "SOAP.h"

#include "SOAPMethod.h"
#include "SOAPEnvelope.h"
#include "SOAPResponse.h"
#include "SOAPPacketWriter.h"
#include "SOAPParse.h"
#include "SOAPUrl.h"

class SOAPTransport;

class EASYSOAP_EXPORT SOAPProxy  
{
public:
	SOAPProxy() :
		m_transport(0),
		m_deltrans(false)
	{
	}

	SOAPProxy(const SOAPUrl& endpoint) :
		m_transport(0),
		m_deltrans(false)
	{
		SetEndpoint(endpoint);
	}

	SOAPProxy(const SOAPUrl& endpoint, const SOAPUrl& proxy) :
		m_transport(0),
		m_deltrans(false)
	{
		SetEndpoint(endpoint, proxy);
	}

	SOAPProxy(SOAPTransport *transport, bool deltrans = false) :
		m_transport(0),
		m_deltrans(false)
	{
		SetEndpoint(transport, deltrans);
	}

	virtual ~SOAPProxy()
	{
		if (m_deltrans)
			delete m_transport;
	}

	void SetEndpoint(const SOAPUrl& endpoint);

	void SetEndpoint(const SOAPUrl& endpoint, const SOAPUrl& proxy);

	void SetEndpoint(SOAPTransport *trans, bool deltrans = false);

	SOAPMethod& SetMethod(const char *name, const char *ns)
	{
		m_method.SetName(name, ns);
		return m_method;
	}

	const SOAPResponse& Execute()
	{
		return Execute(m_method);
	}

	const SOAPResponse& Execute(SOAPMethod& method);

private:

	SOAPProxy(const SOAPProxy& proxy);
	SOAPProxy& operator=(const SOAPProxy& proxy);

	SOAPMethod			m_method;
	SOAPEnvelope		m_message;
	SOAPResponse		m_response;
	SOAPPacketWriter	m_packet;
	SOAPParser			m_parser;
	SOAPTransport		*m_transport;
	bool				m_deltrans;
};

#endif // !defined(AFX_SOAPPROXY_H__D489F911_F39E_4C38_A5C0_95CD3D0A9459__INCLUDED_)

