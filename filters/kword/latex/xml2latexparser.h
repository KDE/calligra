
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

#ifndef xml2latexparser
#define xml2latexparser

#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include "xmlparser.h"
#include "header.h"
#include "document.h"

class Xml2LatexParser : public XmlParser
{
	QFile _file;
	QTextStream _out;
	QString _filename;

	Header _header;
	Document _document;

	public:
		Xml2LatexParser(QString, const char *, const char *);
		void analyse();
		void generate();

	private:

};

#endif
