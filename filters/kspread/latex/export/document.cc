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
	//kdDebug(30522) << fileIn.latin1() << endl;
	kdDebug(30522) << fileOut.latin1() << endl;
	_filename = fileOut;
	//setFileHeader(_fileHeader);
	//setRoot(&_document);
	Config::instance()->setEmbeded(false);
	//analyse_config(config);
}

Document::~Document()
{
	
}

void Document::analyse()
{
	QDomNode balise;
	balise = init();
	kdDebug(30522) << "ANALYSE A DOC" << endl;
	_document.analyse(balise);
	kdDebug(30522) << "END ANALYSE" << endl;
}

void Document::generate()
{
	if(_file.open(IO_WriteOnly))
	{
		kdDebug(30522) << "GENERATION" << endl;
		_out.setDevice(&_file);
		_document.generate(_out, !isEmbeded());
		//_out << getDocument();
		_file.close();
	}
	else
		kdDebug(30522) << "Can't use the file ..." << endl;
}
