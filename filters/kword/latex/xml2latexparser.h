
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

#ifndef __XML2LATEXPARSER_H__
#define __XML2LATEXPARSER_H__

#include <qfile.h>		/* for QFile classe */
#include <qtextstream.h>	/* for QTextStream classe */
#include <qstring.h>		/* for QString classe */
#include "fileheader.h"
#include "document.h"

class Xml2LatexParser : public XmlParser
{
	QFile       _file;
	QTextStream _out;
	QString     _filename;

	FileHeader  _header;
	Document    _document;

	/* Options */
	bool _isEmbeded;

	public:
		Xml2LatexParser(QString, QString);
		Xml2LatexParser(QByteArray, QString, QString);

		virtual ~Xml2LatexParser() {}

		/* Accesors */
		bool isEmbeded() const { return _isEmbeded; }
		void analyse_config(QString);

		void analyse();
		void generate();

	private:

};

#endif /* __XML2LATEXPARSER_H__ */
