
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

#ifndef __KWORD_FORMAT_H__
#define __KWORD_FORMAT_H__

#include <qtextstream.h>
#include <qstring.h>
#include <qcolor.h>
#include "xmlparser.h"

enum _EFormat
{
	EF_ERROR,		/* 0 */
	EF_TEXTZONE,		/* 1 */
	EF_PICTURE,		/* 2 */
	EF_TABULATOR,		/* 3 */
	EF_VARIABLE,		/* 4 */
	EF_FOOTNOTE		/* 5 */
};

typedef enum _EFormat EFormat;

class Para;

class Format: public XmlParser
{
	EFormat _id;
	
	/* USEFULL DATA */
	Para*   _para;

	public:
		Format(Para *para= 0): _id((EFormat) 0), _para(para) {}
		virtual ~Format() {}

		Para*   getPara      () const { return _para; }
		EFormat getFormatType() const { return _id;   }
		//int getPos()
		//int get	
		void setId(int id) { _id = (EFormat) id; }
		void setPara(Para* para) { _para = para; }
		virtual void analyse(const Markup*);
		virtual void generate(QTextStream&) = 0;
};

#endif /* __KWORD_FORMAT_H__ */
