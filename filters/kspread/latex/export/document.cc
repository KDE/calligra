/* TODO : Manage File problems !
 */
/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2003 Robert JACOLIN
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

#include "document.h"

Document::Document(const KoStore* in, QString fileOut):
		XmlParser(in), _file(fileOut), _in( in )
{
	//kDebug(30522) << fileIn << endl;
	kDebug(30522) << fileOut << endl;
	_filename = fileOut;
	//setFileHeader(_fileHeader);
	//setRoot(&_document);
	Config::instance()->setEmbeded(false);
	//analyze_config(config);
}

Document::~Document()
{

}

void Document::analyze()
{
	QDomNode node;
	node = init();
	kDebug(30522) << "ANALYZE A DOC" << endl;
	_document.analyze(node);
	kDebug(30522) << "END ANALYZE" << endl;
}

void Document::generate()
{
	if(_file.open(QIODevice::WriteOnly))
	{
		kDebug(30522) << "GENERATION" << endl;
		_out.setDevice(&_file);
		_document.generate(_out, !isEmbeded());
		//_out << getDocument();
		_file.close();
	}
	else
		kDebug(30522) << "Can't use the file ..." << endl;
}
