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


#if !defined(AFX_SOAPFAULT_H__290FC204_F572_4249_8E5E_D52E987FFAD0__INCLUDED_)
#define AFX_SOAPFAULT_H__290FC204_F572_4249_8E5E_D52E987FFAD0__INCLUDED_

#include "SOAP.h"
#include "SOAPParameter.h"

class SOAPFaultHandler;

class EASYSOAP_EXPORT SOAPFault : protected SOAPParameter
{
public:
	friend class SOAPFaultHandler;

	SOAPFault();
	virtual ~SOAPFault();

	void SetFaultCode(const char *faultcode)
	{
		SetFaultValue(faultcode_attr, faultcode);
	}

	void SetFaultString(const char *faultstring)
	{
		SetFaultValue(faultstring_attr, faultstring);
	}

	/*  This is incorrect
	void SetDetail(const char *detail)
	{
		SetFaultValue(faultdetail_attr, detail);
	}
	*/

	void SetFaultActor(const char *faultactor)
	{
		SetFaultValue(faultactor_attr, faultactor);
	}

	const SOAPParameter* GetFaultString() const;
	const SOAPParameter* GetFaultActor() const;
	const SOAPParameter* GetFaultCode() const;
	const SOAPParameter* GetDetail() const;

	bool WriteSOAPPacket(SOAPPacketWriter& packet) const;

private:

	void SetFaultValue(const char *param, const char *value)
	{
		AddParameter(param).SetValue(value);
	}

	static const char *faultcode_attr;
	static const char *faultstring_attr;
	static const char *faultactor_attr;
	static const char *faultdetail_attr;
};


class SOAPFaultException : public SOAPException
{
public:
	SOAPFaultException(const SOAPFault&);
};


#endif // !defined(AFX_SOAPFAULT_H__290FC204_F572_4249_8E5E_D52E987FFAD0__INCLUDED_)

