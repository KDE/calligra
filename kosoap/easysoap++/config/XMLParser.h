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


#if !defined(AFX_XMLPARSER_H__751545FF_EF84_42BC_9622_A6CE624F1F14__INCLUDED_)
#define AFX_XMLPARSER_H__751545FF_EF84_42BC_9622_A6CE624F1F14__INCLUDED_

class XMLParser
{
public:
	XMLParser();
	virtual ~XMLParser();

	void InitParser(const char *charset = 0);
	void *GetParseBuffer(int size);
	bool ParseBuffer(int size);
	const char *GetErrorMessage();

protected:
	virtual void startElement(const XML_Char *name, const XML_Char **attrs);
	virtual void endElement(const XML_Char *name);
	virtual void characterData(const XML_Char *str, int len);
	virtual void startNamespace(const XML_Char *prefix, const XML_Char *uri);
	virtual void endNamespace(const XML_Char *prefix);

private:
	//
	// the C based XML parser calls through to these methods
	// which pass them on to the C++ interface
	//
	static void _startElement(void *userData, const XML_Char *name, const XML_Char **attrs);
	static void _endElement(void *userData, const XML_Char *name);
	static void _characterData(void *userData, const XML_Char *str, int len);
	static void _startNamespace(void *userData, const XML_Char *prefix, const XML_Char *uri);
	static void _endNamespace(void *userData, const XML_Char *prefix);

	void FreeParser();
	void *m_parser;
};

#endif // !defined(AFX_XMLPARSER_H__751545FF_EF84_42BC_9622_A6CE624F1F14__INCLUDED_)

