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

#include "SOAPBodyHandler.h"
#include "SOAPBody.h"
#include "SOAPNamespaces.h"

static const SOAPQName RootTag("root", SOAP_ENC);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPBodyHandler::SOAPBodyHandler(SOAPBody& body)
: m_body(&body)
, m_gotMethod(false)
, m_methodHandler(body.GetMethod())
, m_faultHandler(body.GetFault())
{
	m_paramHandler.SetIgnoreId();
	m_paramHandler.SetIgnoreName();
}

SOAPBodyHandler::~SOAPBodyHandler()
{

}

SOAPParseEventHandler *
SOAPBodyHandler::start(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs)
{
	m_gotMethod = false;
	return this;
}

SOAPParseEventHandler *
SOAPBodyHandler::startElement(SOAPParser& parser, const XML_Char *name, const XML_Char **attrs)
{
	const char *id = 0;
	const char *href = 0;
	const char *root = 0;

	const XML_Char **cattrs = attrs;
	while (*cattrs)
	{
		const char *tag = *cattrs++;
		const char *val = *cattrs++;
		if (sp_strcmp(tag, "id") == 0)
		{
			id = val;
		}
		else if (sp_strcmp(tag, "href") == 0)
		{
			href = val;
		}
		else if (RootTag == tag)
		{
			root = val;
		}
	}

	if (m_gotMethod || root && sp_strcmp(root, "0"))
	{
		SOAPParameter *p = 0;
		if (id)
		{
			p = parser.GetHRefParam(id);
			if (!p)
				throw SOAPException("Body handler: unknown element, id=%s", id);
		}
		else if (href)
		{
			p = parser.GetHRefParam(++href);
			if (!p)
				throw SOAPException("Body handler: unknown element, href=%s", href);
		}
		else if (root)
		{
			p = &m_body->GetMethod().AddParameter(name);
		}

		m_paramHandler.SetParameter(*p);
		return m_paramHandler.start(parser, name, attrs);
	}

	m_gotMethod = true;
	if (sp_strcmp(name, SOAPFaultHandler::start_tag) == 0)
	{
		m_body->SetIsFault(true);
		return m_faultHandler.start(parser, name, attrs);
	}

	m_body->SetIsFault(false);
	return m_methodHandler.start(parser, name, attrs);
}

