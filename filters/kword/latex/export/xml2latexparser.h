/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000, 2002 Robert JACOLIN
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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __XML2LATEXPARSER_H__
#define __XML2LATEXPARSER_H__

#include <QFile>			/* for QFile class */
#include <qtextstream.h>	/* for QTextStream class */
#include <QString>		/* for QString class */
#include "fileheader.h"
#include "document.h"

class Xml2LatexParser : public XmlParser
{
	QFile _file;
	QTextStream _out;
	/** Name of the latex file. */
	//QString  _filename;
	/** The KWord document exported. */
	const KoStore* _in;

	/** KWord document header. */
	//FileHeader  _header;
	/** The root class which contains one kword document. */
	Document    _document;

	public:
		/**
		 * @param in The document from kword.
		 * @param fileOut Output latex filename.
		 * @param config ???
		 */
		Xml2LatexParser(const KoStore* in, QString fileOut, Config* config);

		virtual ~Xml2LatexParser() {}

		void analyse();
		void generate();

};

#endif /* __XML2LATEXPARSER_H__ */
