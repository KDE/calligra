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


#if !defined(AFX_SOAPARRAYHANDLER_H__3A318691_8E69_43B1_9733_39FE84CA7565__INCLUDED_)
#define AFX_SOAPARRAYHANDLER_H__3A318691_8E69_43B1_9733_39FE84CA7565__INCLUDED_

#include "SOAP.h"
#include "SOAPParseEventHandler.h"

class SOAPParameterHandler;
class SOAPParameter;

class SOAPArrayHandler : public SOAPParseEventHandler  
{
public:
	SOAPArrayHandler();
	virtual ~SOAPArrayHandler();

	void	SetParameter(SOAPParameter* param)	{m_param = param;}
	void	SetArrayType(const XML_Char* type, const XML_Char *typens);

	virtual SOAPParseEventHandler* start(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs);
	virtual SOAPParseEventHandler* startElement(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs);

private:

	SOAPParameter			*m_param;
	SOAPParameterHandler	*m_paramHandler;
};

#endif // !defined(AFX_SOAPARRAYHANDLER_H__3A318691_8E69_43B1_9733_39FE84CA7565__INCLUDED_)

