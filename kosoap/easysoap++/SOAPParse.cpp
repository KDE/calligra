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
#include "SOAPParse.h"
#include "SOAPEnvelopeHandler.h"

#include "SOAPNamespaces.h"

#define BUFF_SIZE 1024

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SOAPParser::SOAPParser()
{
}

SOAPParser::~SOAPParser()
{
}


SOAPEnvelope&
SOAPParser::Parse(SOAPEnvelope& env, SOAPTransport& trans)
{
	SOAPEnvelopeHandler envhandler(env);
	m_handler = &envhandler;

	// make sure our stack is empty
	m_handlerstack.Clear();
	m_hrefmap.Clear();

	InitParser(trans.GetCharset());
	while (1)
	{
		//
		// create a buffer to read the HTTP payload into
		//
		void *buffer = GetParseBuffer(BUFF_SIZE);
		if (!buffer)
			throw SOAPMemoryException();

		//
		// read the HTTP payload
		//
		int read = trans.Read((char *)buffer, BUFF_SIZE);
		if (!ParseBuffer(read))
		{
			throw SOAPException(
				"Error while parsing SOAP XML payload: %s",
				GetErrorMessage());
		}

		if (read != 0 && m_handler->Done())
		{
			ParseBuffer(0);
			break;
		}
	}

	return env;
}

void
SOAPParser::startElement(const XML_Char *name, const XML_Char **attrs)
{
	SOAPParseEventHandler* handler = 0;
	if (m_handlerstack.IsEmpty())
	{
		if (sp_strcmp(name, SOAPEnvelopeHandler::start_tag) == 0)
		{
			handler = m_handler;
		}
		else
		{
			//
			// FIXME:
			// Probably what we should do instead of throw is set
			// a flag that says the response is invalid.  We usually
			// get in here when the HTTP response code is 500 and it
			// gives us back some HTML instead of a SOAP Fault.
			throw SOAPException("Unknown SOAP response tag: %s", name);
		}
	}
	else
	{
		handler = m_handlerstack.Top();
	}

	if (handler)
	{
		m_handlerstack.Push(handler->startElement(*this, name, attrs));
	}
	else
	{
		m_handlerstack.Push(0);
	}
}

void
SOAPParser::characterData(const XML_Char *str, int len)
{
	SOAPParseEventHandler* handler = m_handlerstack.Top();
	if (handler)
		handler->characterData(str, len);
}

void
SOAPParser::endElement(const XML_Char *name)
{
	SOAPParseEventHandler* handler = m_handlerstack.Top();
	if (handler)
		handler->endElement(name);
	m_handlerstack.Pop();
}

void
SOAPParser::startNamespace(const XML_Char *prefix, const XML_Char *uri)
{
	if (prefix)
		m_nsmap[prefix] = uri;
	else
		m_nsmap[""] = uri;
}

void
SOAPParser::endNamespace(const XML_Char *prefix)
{
	if (prefix)
		m_nsmap.Remove(prefix);
	else
		m_nsmap.Remove("");
}

SOAPParameter *
SOAPParser::GetHRefParam(const SOAPString& name)
{
	HRefMap::Iterator i = m_hrefmap.Find(name);
	if (i)
		return *i;
	return 0;
}

void
SOAPParser::SetHRefParam(const SOAPString& name, SOAPParameter *param)
{
	m_hrefmap[name] = param;
}

const char *
SOAPParser::ExpandNamespace(const char *ns) const
{
	NamespaceMap::Iterator i = m_nsmap.Find(ns);
	if (i)
		return i->Str();
	return 0;
}

