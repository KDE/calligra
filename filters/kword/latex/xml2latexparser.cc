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

/*#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>*/
#include <iostream.h>

#include <kdebug.h>

#include "xml2latexparser.h"
//#include "xmlparse.h"

Xml2LatexParser::Xml2LatexParser(QString filename, const char *data, const char *charset): XmlParser(data, charset), _file(filename.latin1())
{
	cout << filename.latin1() << endl;
	_filename = filename;
}

void Xml2LatexParser::analyse()
{
	Markup *balise;
	Token *savedToken;

 	// LIT un premier bloc --> c'est une entete
	// _header.setPaper(bloc);
	// LIT un 2e bloc --> encore
	// _header.setAttributs(bloc);
	//...
	// LIT un bloc -->       c'est un FRAMESET
	// _document.addBloc(bloc);
	// Recupere les balise
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "DOC") == 0)
		{
			cout << "ENTETE -> DOC" << endl;
			savedToken = enterTokenChild(balise);
			//_header.analyse();

		}
		else if(strcmp(balise->token.zText, "PAPER") == 0)
		{
			cout <<"ENTETE -> PAPER" << endl;
			_header.setPaper(balise);
		}
		else if(strcmp(balise->token.zText, "ATTRIBUTES") == 0)
		{
			cout <<"ENTETE -> ATTRIBUTES" << endl;
			_header.setAttributs(balise);
		}
		else if(strcmp(balise->token.zText, "FRAMESETS") == 0)
		{
			cout <<"ENTETE -> FRAMESETS" << endl;
			//PrintXml(getTokenCurrent(), 3);
			_document.analyse(balise);
			cout <<"ENTETE -> FIN FRAMESETS" << endl;
		}
		else if(strcmp(balise->token.zText, "STYLES") == 0)
		{
			cout <<"ENTETE -> STYLES" << endl;
			// not implemented
			// _style.analyse(balise);
		}
		else if(strcmp(balise->token.zText, "PIXMAPS") == 0)
		{
			cout <<"ENTETE -> PIXMAPS" << endl;
			// not implemented
			// _pixmaps.analyse(balise);
		}
		else if(strcmp(balise->token.zText, "SERIALL") == 0)
		{
			cout <<"ENTETE -> SERIALL" << endl;
			// not implemented
			// _seriall.analyse();
		}
	}
	cout << "FIN ANALYSE" << endl;
}

void Xml2LatexParser::generate()
{
	if(_file.open(IO_WriteOnly))
	{
		cout << "GENERATION" << endl;
		_out.setDevice(&_file);
		_header.generate(_out);
		_document.generate(_out);
		//_out << getDocument();
		// Parcourir l'entete et le document pour generer le fichier.
	}
	_file.close();
}
