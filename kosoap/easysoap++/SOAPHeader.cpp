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
#include "SOAPHeader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPHeader::SOAPHeader()
{

}

SOAPHeader::~SOAPHeader()
{

}

SOAPParameter&
SOAPHeader::AddHeader()
{
	return m_headers.Add();
}

const SOAPHeader::Headers&
SOAPHeader::GetHeaders() const
{
	return m_headers;
}

void
SOAPHeader::Reset()
{
	for (Headers::Iterator i = m_headers.Begin(); i != m_headers.End(); ++i)
		i->Reset();
	m_headers.Resize(0);
}

bool
SOAPHeader::WriteSOAPPacket(SOAPPacketWriter& packet) const
{
	// Nothing to do yet...
	return true;
}



