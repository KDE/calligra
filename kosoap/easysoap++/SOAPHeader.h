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


#if !defined(AFX_SOAPHEADER_H__9717994A_8D5C_42BE_9F48_E0B7990C59E3__INCLUDED_)
#define AFX_SOAPHEADER_H__9717994A_8D5C_42BE_9F48_E0B7990C59E3__INCLUDED_

#include "SOAP.h"

class EASYSOAP_EXPORT SOAPHeader
{
public:
	SOAPHeader();
	virtual ~SOAPHeader();

	typedef SOAPArray<SOAPParameter> Headers;

	void Reset();
	SOAPParameter& AddHeader();
	const Headers& GetHeaders() const;

	bool WriteSOAPPacket(SOAPPacketWriter& packet) const;

private:

	Headers	m_headers;
};

#endif // !defined(AFX_SOAPHEADER_H__9717994A_8D5C_42BE_9F48_E0B7990C59E3__INCLUDED_)

