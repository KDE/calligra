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

#include "xml2texgraphparser.h"

Xml2TexgraphParser::Xml2TexgraphParser(QString filename, const char *data): XmlParser(data), _file(filename.latin1())
{
	kdDebug() << filename.latin1() << endl;
	_filename = filename;
	_document.setFileHeader(_fileHeader);
}

void Xml2TexgraphParser::analyse()
{
	Markup* balise     = 0;
	Token*  savedToken = 0;

	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "DOC") == 0)
		{
			kdDebug() << "ENTETE -> DOC" << endl;
			savedToken = enterTokenChild(balise);
			//_header.analyse();

		}
		else if(strcmp(balise->token.zText, "PAPER") == 0)
		{
			kdDebug() <<"ENTETE -> PAPER" << endl;
			_header.analysePaper(balise);
		}
		else if(strcmp(balise->token.zText, "ATTRIBUTES") == 0)
		{
			kdDebug() <<"ENTETE -> ATTRIBUTES" << endl;
			_header.analyseAttributs(balise);
		}
		else if(strcmp(balise->token.zText, "FRAMESETS") == 0)
		{
			kdDebug() <<"ENTETE -> FRAMESETS" << endl;
			_document.analyse(balise);
			kdDebug() <<"ENTETE -> FIN FRAMESETS" << endl;
		}
		else if(strcmp(balise->token.zText, "STYLES") == 0)
		{
			kdDebug() <<"ENTETE -> STYLES" << endl;
			// not implemented
			// _style.analyse(balise);
		}
		else if(strcmp(balise->token.zText, "PIXMAPS") == 0)
		{
			kdDebug() <<"ENTETE -> PIXMAPS" << endl;
			// not implemented
			// _pixmaps.analyse(balise);
		}
		else if(strcmp(balise->token.zText, "SERIALL") == 0)
		{
			kdDebug() <<"ENTETE -> SERIALL" << endl;
			// not implemented
			// _seriall.analyse();
		}
	}
	kdDebug() << "FIN ANALYSE" << endl;
}

void Xml2TexgraphParser::generate()
{
	if(_file.open(IO_WriteOnly))
	{
		kdDebug() << "GENERATION" << endl;
		_out.setDevice(&_file);
		_document.generate(_out);
		_out << getDocument();
	}
	else
		kdDebug() << "Can't use the file ..." << endl;
	_file.close();
}
