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
#include "SOAPEnvelope.h"
#include "SOAPNamespaces.h"
#include "SOAPPacketWriter.h"

const SOAPQName EnvelopeTag("Envelope", SOAP_ENV);
const SOAPQName EncodingAttr("encodingStyle", SOAP_ENV);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPEnvelope::SOAPEnvelope()
{

}

SOAPEnvelope::~SOAPEnvelope()
{

}

bool
SOAPEnvelope::WriteSOAPPacket(SOAPPacketWriter& packet) const
{
	packet.Reset();
	packet.StartTag(EnvelopeTag, "E");

	// TODO: automagically add only the tags we need...
	packet.AddXMLNS("A", SOAP_ENC);
	packet.AddXMLNS("s", SOAP_XSI);
	packet.AddXMLNS("y", SOAP_XSD);
	packet.SetNamespace("___easysoap_default_ns", "y");

	// TODO: allow user to set encoding style
	packet.AddAttr(EncodingAttr, SOAP_ENC);

	m_header.WriteSOAPPacket(packet);
	m_body.WriteSOAPPacket(packet);

	packet.EndTag(EnvelopeTag);

	return true;
}




