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


#if !defined(AFX_SOAPBODY_H__2ED6E429_A5CB_4FEB_B1B1_19E83A2F8046__INCLUDED_)
#define AFX_SOAPBODY_H__2ED6E429_A5CB_4FEB_B1B1_19E83A2F8046__INCLUDED_

#include "SOAP.h"

#include "SOAPMethod.h"
#include "SOAPFault.h"

class EASYSOAP_EXPORT SOAPBody
{
public:
	SOAPBody();
	virtual ~SOAPBody();

	SOAPMethod&			GetMethod()			{return m_method;}
	const SOAPMethod&	GetMethod() const	{return m_method;}
	void	SetMethod(const SOAPMethod& m)	{m_method = m;}

	SOAPFault&			GetFault()			{return m_fault;}
	const SOAPFault&	GetFault() const	{return m_fault;}

	bool IsFault() const					{return m_isfault;}
	void SetIsFault(bool isfault = true)	{m_isfault = isfault;}
	bool WriteSOAPPacket(SOAPPacketWriter& packet) const;

private:

	static const char *parse_tag;

	SOAPMethod	m_method;
	SOAPFault	m_fault;
	bool		m_isfault;
};

#endif // !defined(AFX_SOAPBODY_H__2ED6E429_A5CB_4FEB_B1B1_19E83A2F8046__INCLUDED_)

