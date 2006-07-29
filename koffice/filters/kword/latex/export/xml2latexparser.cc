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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <kdebug.h>

#include "xml2latexparser.h"

Xml2LatexParser::Xml2LatexParser(const KoStore* in, QString fileOut, Config* config):
		XmlParser(config, in), _file(fileOut), _in( in )
{
	//kdDebug(30522) << fileIn.latin1() << endl;
	kdDebug(30522) << fileOut.latin1() << endl;
	_filename = fileOut;
	//setFileHeader(_fileHeader);
	setRoot(&_document);
}

void Xml2LatexParser::analyse()
{
	QDomNode balise;
	balise = init();
	//balise = getChild(balise, "DOC");
	kdDebug(30522) <<"HEADER -> PAPER" << endl;
	FileHeader::instance()->analysePaper(getChild(balise, "PAPER"));
	kdDebug(30522) <<"HEADER -> ATTRIBUTES" << endl;
	FileHeader::instance()->analyseAttributs(getChild(balise, "ATTRIBUTES"));
	kdDebug(30522) <<"HEADER -> FRAMESETS" << endl;
	_document.analyse(getChild(balise, "FRAMESETS"));
	kdDebug(30522) <<"HEADER -> END FRAMESETS" << endl;
	//kdDebug(30522) <<"HEADER -> STYLES" << endl;
	//
	kdDebug(30522) <<"HEADER -> PICTURES" << endl;
	_document.analysePixmaps(getChild(balise, "PICTURES"));
	//kdDebug(30522) <<"HEADER -> SERIALL" << endl;
	kdDebug(30522) << "END ANALYSE" << endl;
}

void Xml2LatexParser::generate()
{
	if(_file.open(IO_WriteOnly))
	{
		kdDebug(30522) << "GENERATION" << endl;
		_out.setDevice(&_file);
		if(!Config::instance()->isEmbeded())
			FileHeader::instance()->generate(_out);
		_document.generate(_out, !Config::instance()->isEmbeded());
		//_out << getDocument();
		_file.close();
	}
	else
		kdDebug(30522) << "Can't use the file ..." << endl;
}
