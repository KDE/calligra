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


#if !defined(AFX_SOAPPACKETWRITER_H__DB61E902_B4A0_4AA3_A0F7_479D6295BD06__INCLUDED_)
#define AFX_SOAPPACKETWRITER_H__DB61E902_B4A0_4AA3_A0F7_479D6295BD06__INCLUDED_

#include "SOAPHashMap.h"
#include "SOAPString.h"

class EASYSOAP_EXPORT SOAPPacketWriter  
{
public:
	SOAPPacketWriter();
	virtual ~SOAPPacketWriter();
	void Reset();

	const char * GetBytes();
	unsigned int GetLength();

	void StartTag(const char *tag);
	void StartTag(const SOAPQName& tag, const char *prefix = 0);

	void AddAttr(const char *attr, const char *value);
	void AddAttr(const SOAPQName& attr, const char *value);
	void AddAttr(const SOAPQName& attr, const SOAPQName& value);

	void AddXMLNS(const char *prefix, const char *ns);

	void EndTag(const char *tag);
	void EndTag(const SOAPQName& tag);

	void WriteValue(const char *val);
	const char *GetSymbol(char *buff, const char *prefix);

	void SetNamespace(const char *ns, const char *tag);

	static void SetAddWhiteSpace(bool ws = true);
private:
	void EndStart();
	void Resize();
	void Write(const char *str);
	void WriteEscaped(const char *str);
	void Write(const char *str, int len);

	typedef SOAPHashMap<SOAPString, SOAPString> NamespaceMap;

	bool			m_instart;
	char			*m_buffer;
	char			*m_buffptr;
	const char		*m_buffend;
	unsigned int	m_buffsize;
	unsigned int	m_gensym;
	NamespaceMap	m_nsmap;

	static bool		g_makePretty;
};

#endif // !defined(AFX_SOAPPACKETWRITER_H__DB61E902_B4A0_4AA3_A0F7_479D6295BD06__INCLUDED_)

