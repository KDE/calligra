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

#include "SOAPParameterHandler.h"
#include "SOAPArrayHandler.h"
#include "SOAPStructHandler.h"
#include "SOAPNamespaces.h"
#include "SOAPParameter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPParameterHandler::SOAPParameterHandler()
: m_param(0)
, m_structHandler(0)
, m_arrayHandler(0)
, m_setvalue(false)
, m_ignoreId(false)
, m_ignoreName(false)
{
}

SOAPParameterHandler::~SOAPParameterHandler()
{
	delete m_arrayHandler;
	delete m_structHandler;
}

const SOAPQName AttrXsiType1999("type", SOAP_XSI_1999);
const SOAPQName AttrXsiType2001("type", SOAP_XSI_2001);
const SOAPQName AttrXsiNull1999("null", SOAP_XSI_1999);
const SOAPQName AttrXsiNull2001("nil", SOAP_XSI_2001);
const SOAPQName AttrArrayType("arrayType", SOAP_ENC);

SOAPParseEventHandler *
SOAPParameterHandler::start(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs)
{
	if (!m_ignoreName)
		m_param->SetName(name);
	m_param->Reset();
	m_setvalue = true;
	m_str = "";

	const XML_Char *elementType = 0;
	const XML_Char *arrayType = 0;

	const XML_Char **cattrs = attrs;
	while (*cattrs)
	{
		const XML_Char *tag = *cattrs++;
		const XML_Char *val = *cattrs++;

		if (AttrArrayType == tag)
		{
			arrayType = val;
		}
		else if (AttrXsiType2001 == tag || AttrXsiType1999 == tag)
		{
			elementType = val;
			char *sep = sp_strchr(val, ':');
			if (sep)
			{
				*sep = 0;
				const char *typens = parser.ExpandNamespace(val);
				*sep = ':';
				if (typens)
				{
					m_param->SetType(++sep, typens);
				}
				else
				{
					throw SOAPException("Could not resolve typename for element %s: %s", name, val);
				}
			}
			else
			{
				throw SOAPException("Typename is not namespace qualified for element %s: %s", name, val);
			}
		}
		else if (AttrXsiNull2001 == tag || AttrXsiNull1999 == tag)
		{
			if (sp_strcmp(val, "1") == 0 || sp_strcasecmp(val, "true") == 0)
			{
				m_param->SetNull();
				m_setvalue = false;
			}
		}
		else
		{
			m_param->AddAttribute(tag, val);
		}
	}

	if (!elementType)
	{
		m_param->SetType(m_paramType);
	}

	if (arrayType)
	{
		if (!m_arrayHandler)
			m_arrayHandler = new SOAPArrayHandler();
		m_arrayHandler->SetParameter(m_param);
		m_setvalue = false;

		const char *sep = sp_strchr(arrayType, ':');
		const char *typens = 0;
		if (sep)
		{
			char *z = (char *)sep;
			*z = 0;
			typens = parser.ExpandNamespace(arrayType);

			if (!typens)
				throw SOAPException("Could not resolve arrayType for element %s: %s", name, arrayType);

			*z = ':';
			++sep;
		}
		else
		{
			typens = parser.ExpandNamespace("");

			if (!typens)
				throw SOAPException("Array typename is not namespace qualified for element %s: %s", name, arrayType);

			sep = arrayType;
		}

		char *b = sp_strchr(sep, '[');
		if (b)
			*b = 0;
		m_arrayHandler->SetArrayType(sep, typens);
		if (b)
			*b = '[';

		return m_arrayHandler->start(parser, name, attrs);
	}

	return this;
}

SOAPParseEventHandler *
SOAPParameterHandler::startElement(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs)
{
	//
	// If a parameter has an element, then it must
	// be a struct!
	m_setvalue = false;
	if (!m_structHandler)
		m_structHandler = new SOAPStructHandler();
	m_structHandler->SetParameter(m_param);
	return m_structHandler->start(parser, 0, 0)->startElement(parser, name, attrs);
}

void
SOAPParameterHandler::characterData(const XML_Char *str, int len)
{
	if (m_setvalue)
		m_str.Append(str, len);
}

void
SOAPParameterHandler::endElement(const XML_Char *name)
{
	if (m_setvalue)
	{
		m_param->SetNull(false);
		m_param->GetStringRef() = m_str;
	}
}
