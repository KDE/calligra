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

#include <expat.h>
#include "XMLParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XMLParser::XMLParser()
{
	m_parser = 0;
}

XMLParser::~XMLParser()
{
	FreeParser();
}


void
XMLParser::FreeParser()
{
	if (m_parser)
	{
		XML_ParserFree(m_parser);
		m_parser = 0;
	}
}

void
XMLParser::InitParser(const char *encoding)
{
	FreeParser();
	m_parser = XML_ParserCreateNS(encoding, '#');
	XML_SetElementHandler(m_parser,
			XMLParser::_startElement,
			XMLParser::_endElement);

	XML_SetCharacterDataHandler(m_parser,
			XMLParser::_characterData);

	XML_SetStartNamespaceDeclHandler(m_parser,
			XMLParser::_startNamespace);

	XML_SetEndNamespaceDeclHandler(m_parser,
			XMLParser::_endNamespace);

	XML_SetUserData(m_parser, this);
}

void *
XMLParser::GetParseBuffer(int size)
{
	if (m_parser)
		return XML_GetBuffer(m_parser, size);
	return 0;
}

bool
XMLParser::ParseBuffer(int size)
{
	if (m_parser)
		return XML_ParseBuffer(m_parser, size, size == 0) != 0;
	return false;
}

const char *
XMLParser::GetErrorMessage()
{
	if (m_parser)
		return XML_ErrorString(XML_GetErrorCode(m_parser));
	return 0;
}

void
XMLParser::startElement(const XML_Char *name, const XML_Char **attrs)
{
}

void
XMLParser::endElement(const XML_Char *name)
{
}

void
XMLParser::characterData(const XML_Char *str, int len)
{
}

void
XMLParser::startNamespace(const XML_Char *prefix, const XML_Char *uri)
{
}

void
XMLParser::endNamespace(const XML_Char *prefix)
{
}

//
// static methods
//

void
XMLParser::_startElement(void *userData, const XML_Char *name, const XML_Char **attrs)
{
	XMLParser *parser = (XMLParser *)userData;
	parser->startElement(name, attrs);
}

void
XMLParser::_endElement(void *userData, const XML_Char *name)
{
	XMLParser *parser = (XMLParser *)userData;
	parser->endElement(name);
}

void
XMLParser::_characterData(void *userData, const XML_Char *str, int len)
{
	XMLParser *parser = (XMLParser *)userData;
	parser->characterData(str, len);
}

void
XMLParser::_startNamespace(void *userData, const XML_Char *prefix, const XML_Char *uri)
{
	XMLParser *parser = (XMLParser *)userData;
	parser->startNamespace(prefix, uri);
}

void
XMLParser::_endNamespace(void *userData, const XML_Char *prefix)
{
	XMLParser *parser = (XMLParser *)userData;
	parser->endNamespace(prefix);
}


