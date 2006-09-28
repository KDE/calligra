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
	//kDebug(30522) << fileIn.latin1() << endl;
	kDebug(30522) << fileOut.latin1() << endl;
	_filename = fileOut;
	//setFileHeader(_fileHeader);
	setRoot(&_document);
}

void Xml2LatexParser::analyze()
{
	QDomNode node;
	node = init();
	//node = getChild(node, "DOC");
	kDebug(30522) <<"HEADER -> PAPER" << endl;
	FileHeader::instance()->analyzePaper(getChild(node, "PAPER"));
	kDebug(30522) <<"HEADER -> ATTRIBUTES" << endl;
	FileHeader::instance()->analyzeAttributes(getChild(node, "ATTRIBUTES"));
	kDebug(30522) <<"HEADER -> FRAMESETS" << endl;
	_document.analyze(getChild(node, "FRAMESETS"));
	kDebug(30522) <<"HEADER -> END FRAMESETS" << endl;
	//kDebug(30522) <<"HEADER -> STYLES" << endl;
	//
	kDebug(30522) <<"HEADER -> PICTURES" << endl;
	_document.analyzePixmaps(getChild(node, "PICTURES"));
	//kDebug(30522) <<"HEADER -> SERIALL" << endl;
	kDebug(30522) << "END ANALYZE" << endl;
}

void Xml2LatexParser::generate()
{
	if(_file.open(QIODevice::WriteOnly))
	{
		kDebug(30522) << "GENERATION" << endl;
		_out.setDevice(&_file);
		if(!Config::instance()->isEmbeded())
			FileHeader::instance()->generate(_out);
		_document.generate(_out, !Config::instance()->isEmbeded());
		//_out << getDocument();
		_file.close();
	}
	else
		kDebug(30522) << "Can't use the file ..." << endl;
}
