/* TODO : Manage File problems !
 */
/*
** A program to convert the XML rendered by KWord into LATEX.
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

#include <kdebug.h>

#include "xml2latexparser.h"

Xml2LatexParser::Xml2LatexParser(QString fileIn, QString fileOut):
		XmlParser(fileIn), _file(fileOut)
{
	kdDebug() << fileIn.latin1() << endl;
	kdDebug() << fileOut.latin1() << endl;
	_filename = fileOut;
	setFileHeader(_fileHeader);
	setRoot(&_document);
	_isEmbeded = false;
}

Xml2LatexParser::Xml2LatexParser(QByteArray array, QString fileOut, QString config):
		XmlParser(array), _file(fileOut)
{
	//kdDebug() << fileIn.latin1() << endl;
	kdDebug() << fileOut.latin1() << endl;
	_filename = fileOut;
	setFileHeader(_fileHeader);
	setRoot(&_document);
	_isEmbeded = false;
	analyse_config(config);
}

void Xml2LatexParser::analyse_config(QString config)
{
	kdDebug() << config << endl;
	if(config.contains("EMBEDED") > 0)
		_isEmbeded = true;
	if(config.contains("LATEX") > 0)
		useLatexStyle();
	else if(config.contains("KWORD") > 0)
		useKwordStyle();
	if(config.contains("UNICODE") > 0)
		useUnicodeEnc();
	else if(config.contains("LATIN1") > 0)
		useLatin1Enc();
}

void Xml2LatexParser::analyse()
{
	QDomNode balise;
	balise = init();
	balise = getChild(balise, "DOC");
	kdDebug() <<"ENTETE -> PAPER" << endl;
	kdDebug() << getChildName(balise, 0) << endl;
	_header.analysePaper(getChild(balise, "PAPER"));
	kdDebug() <<"ENTETE -> ATTRIBUTES" << endl;
	_header.analyseAttributs(getChild(balise, "ATTRIBUTES"));
	kdDebug() <<"ENTETE -> FRAMESETS" << endl;
	_document.analyse(getChild(balise, "FRAMESETS"));
	kdDebug() <<"ENTETE -> FIN FRAMESETS" << endl;
	//kdDebug() <<"ENTETE -> STYLES" << endl;
	//kdDebug() <<"ENTETE -> PIXMAPS" << endl;
	//kdDebug() <<"ENTETE -> SERIALL" << endl;
	kdDebug() << "FIN ANALYSE" << endl;
}

void Xml2LatexParser::generate()
{
	if(_file.open(IO_WriteOnly))
	{
		kdDebug() << "GENERATION" << endl;
		_out.setDevice(&_file);
		if(!isEmbeded())
			_header.generate(_out);
		_document.generate(_out, !isEmbeded());
		_out << getDocument();
	}
	else
		kdDebug() << "Can't use the file ..." << endl;
	_file.close();
}
