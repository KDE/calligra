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


#if !defined(AFX_SOAPHEADERHANDLER_H__A673A39C_3468_40A3_A837_690B8A74AAE2__INCLUDED_)
#define AFX_SOAPHEADERHANDLER_H__A673A39C_3468_40A3_A837_690B8A74AAE2__INCLUDED_

#include "SOAPParameterHandler.h"

class SOAPHeader;

class SOAPHeaderHandler : public SOAPParseEventHandler
{
public:

	SOAPHeaderHandler(SOAPHeader&);
	virtual ~SOAPHeaderHandler();

	virtual SOAPParseEventHandler* start(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs);
	virtual SOAPParseEventHandler* startElement(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs);

private:
	SOAPHeader				*m_header;
	SOAPParameterHandler	m_paramHandler;
};

#endif // !defined(AFX_SOAPHEADERHANDLER_H__A673A39C_3468_40A3_A837_690B8A74AAE2__INCLUDED_)

