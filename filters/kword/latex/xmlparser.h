
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_XMLPARSER_H__
#define __KWORD_XMLPARSER_H__

#include "xmlparse.h"

class XmlParser
{
	const char* _document;
	const char* _charset;
	int         _index;
	Token*      _arbreXml;
	Token*      _tokenCurrent;
	Token*      _childCurrent;

	public:
		XmlParser(const char *, const char *);
		XmlParser();
		virtual ~XmlParser();

		const char *getDocument() const;
		Token* getTokenCurrent()  const { return _tokenCurrent; }

		void   setTokenCurrent (Token * t)     { _tokenCurrent = t; }
		void   nextToken       ()              { _tokenCurrent = _tokenCurrent->pNext; };
		Token* enterTokenChild(const Markup*);
		
		void analyse(){};
		void generate(){};

	protected:
		Markup* getNextMarkup();
		Token*  getNextChild ();

};

#endif /* __KWORD_XMLPARSER_H__ */
