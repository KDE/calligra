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

#include "SOAPProxy.h"
#include "SOAPonHTTP.h"

const SOAPResponse&
SOAPProxy::Execute(SOAPMethod& method)
{
	if (!m_transport)
		throw SOAPException("No transport!");

	m_message.GetBody().SetMethod(method);
	m_message.WriteSOAPPacket(m_packet);
	m_transport->Write(method, m_packet.GetBytes(), m_packet.GetLength());
	m_response.SetMethod(method);
	m_parser.Parse(m_response, *m_transport);

	if (m_response.IsFault())
		throw SOAPFaultException(m_response.GetBody().GetFault());

	return m_response;
}

void
SOAPProxy::SetEndpoint(const SOAPUrl& endpoint)
{
	// Assume it's an HTTP protocol.
	// TODO: What we need here is a factory.
	SetEndpoint(new SOAPonHTTP(endpoint), true);
}

void
SOAPProxy::SetEndpoint(const SOAPUrl& endpoint, const SOAPUrl& proxy)
{
	// Assume it's an HTTP protocol.
	// TODO: What we need here is a factory.
	SetEndpoint(new SOAPonHTTP(endpoint, proxy), true);
}

void
SOAPProxy::SetEndpoint(SOAPTransport *trans, bool deltrans)
{
	if (m_deltrans)
		delete m_transport;
	m_transport = trans;
	m_deltrans = deltrans;
}
