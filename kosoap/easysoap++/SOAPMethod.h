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


#if !defined(AFX_SOAPMETHOD_H__B3726AD6_5844_4059_8ECD_36A553A918AD__INCLUDED_)
#define AFX_SOAPMETHOD_H__B3726AD6_5844_4059_8ECD_36A553A918AD__INCLUDED_

#include "SOAP.h"
#include "SOAPParameter.h"

class EASYSOAP_EXPORT SOAPMethod : public SOAPParameter
{
public:
	SOAPMethod() {}
	SOAPMethod(const char *name, const char *ns, const char *soapAction = 0, bool appendName = true);
	virtual ~SOAPMethod();

	size_t GetNumParameters() const
	{
		return GetArray().Size();
	}

	const SOAPString& GetNamespace() const
	{
		return GetName().GetNamespace();
	}

	void SetSoapAction(const char *sa, bool appendName = true);
	const SOAPString& GetSoapAction() const
	{
		return m_action;
	}

	bool WriteSOAPPacket(SOAPPacketWriter& packet) const;
private:

	SOAPString				m_action;
};

#endif // !defined(AFX_SOAPMETHOD_H__B3726AD6_5844_4059_8ECD_36A553A918AD__INCLUDED_)

