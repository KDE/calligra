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
#include "SOAPBody.h"
#include "SOAPPacketWriter.h"
#include "SOAPNamespaces.h"

const SOAPQName BodyTag("Body", SOAP_ENV);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPBody::SOAPBody()
{
	m_isfault = false;
}

SOAPBody::~SOAPBody()
{

}

bool
SOAPBody::WriteSOAPPacket(SOAPPacketWriter& packet) const
{
	packet.StartTag(BodyTag);

	if (m_isfault)
		m_fault.WriteSOAPPacket(packet);
	else
		m_method.WriteSOAPPacket(packet);

	packet.EndTag(BodyTag);

	return true;
}


