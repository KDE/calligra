
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

#ifndef __KWORD_DOCUMENT_H__
#define __KWORD_DOCUMENT_H__

#include "header.h"
#include "listelement.h"
#include <qtextstream.h>

enum _EGenerate
{
	E_LATEX,
	E_KWORD,
	E_CONFIG
};

typedef enum _EGenerate EGenerate;

class Document: public XmlParser
{
	Header*     _header;
	ListElement _enTete;
	ListElement _footer;
	ListElement _corps;
	EGenerate   _generation;

	public:
		Document();

		virtual ~Document();

		SType getTypeFrameset(const Markup *);
		Header* getHeader() const { return _header; }

		void setHeader(Header *h) { _header = h; }

		void  generate(QTextStream&);
		void  analyse(const Markup *);

	private:
		void generateTypeHeader(QTextStream&, Element*);
		void generateTypeFooter(QTextStream&, Element*);
};

#endif /* __KWORD_DOCUMENT_H__ */
