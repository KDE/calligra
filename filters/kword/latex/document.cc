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

#include "document.h"
#include "texte.h"

/* CONSTRUCTORS */
Document::Document()
{
}

/* DESTRUCTORS */
Document::~Document()
{
	kdDebug() << "Corps Destructor" << endl;
}


void Document::analyse(const Markup * balise_initiale)
{
	Markup *balise = 0;
		
	setTokenCurrent(balise_initiale->pContent);
	while((balise = getNextMarkup()) != 0)
	{
		kdDebug() << balise << endl;
		kdDebug() << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "FRAMESET")== 0)
		{
			Element *elt = 0;
			kdDebug() <<"ANALYSE D'UNE FRAMESET" << endl;
			switch(getTypeFrameset(balise))
			{
				case ST_AUCUN: 
					kdDebug() << "AUCUN" << endl;
					break;
				case ST_IMAGE:
					kdDebug() << "IMAGE" << endl;
					// elt = new Image;
					// elt->analyse(balise);
					break;
				case ST_TEXTE: 
					kdDebug() << "TEXTE" << endl;
					elt = new Texte;
					elt->analyse(balise);
					break;
				case ST_PARTS:
					break;
				default:
					kdDebug() << "erreur " << elt->getType() << " " << ST_TEXTE << endl;
			}
			
			// 3. Ajouter l'Element dans une des listes
			kdDebug() << "INFO : " << elt->getSection();
			switch(elt->getSection())
			{
				case SS_PIEDS: kdDebug() << "FOOTER" <<endl;
					       _footer.add(elt);
					       break;
				case SS_ENTETE: kdDebug() << " HEADER" << endl;
						_enTete.add(elt);
					break;
				case SS_CORPS: 	_corps.add(elt);
						kdDebug() << " CORPS" << endl;
					break;
				default: kdDebug() << "INCONNU" << endl;
					break;
			}
		}
		kdDebug() << "FIN D'ANALYSE DE FRAMESET" << endl;
	}
	
}

SType Document::getTypeFrameset(const Markup *balise)
{
	Arg*  arg  = 0;
	SType type = ST_AUCUN;

	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "param : " << arg->zName << " " << arg->zValue << endl;
		if(strcmp(arg->zName, "FRAMETYPE")== 0)
		{
			// A FINIR
			kdDebug() << "TYPE : TEXTE" << endl;
			type = ST_TEXTE;
		}
	}
	kdDebug() << "FIN TYPE" << endl;
	return type;
}

void Document::generate(QTextStream &out)
{
	ElementIter iter1;
	ElementIter iter2;
	ElementIter iter3;

	kdDebug() << "DOC. GENERATION." << endl;
	
	// For each header
	if(getHeader()->hasHeader())
	{
		kdDebug() << "header : " << _enTete.getSize() << endl;
		iter1.setList(&_enTete);

		while(!iter1.isTerminate())
		{
			generateTypeHeader(out, iter1.getCourant());
			iter1.next();
		}
	}
	
	// For each footer
	if(getHeader()->hasFooter())
	{
		kdDebug() << "footer : " << _enTete.getSize() << endl;
		iter2.setList(&_footer);

		while(!iter2.isTerminate())
		{
			generateTypeFooter(out, iter2.getCourant());
			iter2.next();
		}
	}
	if(getHeader()->hasHeader() || getHeader()->hasFooter())
		out << "\\pagestyle{fancy}" << endl;

	// Body
	kdDebug() << endl << "body : " << _corps.getSize() << endl;

	out << "\\begin{document}" << endl;
	_corps.getFirst()->generate(out);
	out << "\\end{document}" << endl;
}

void Document::generateTypeHeader(QTextStream &out, Element *header)
{
	out << "\\fancyhead[C";
	switch(header->getInfo())
	{
		case SI_NONE:
		case SI_FIRST:
			break;
		case SI_ODD:
			out << "O";
			break;
		case SI_EVEN:
			out << "E";
			break;
	}
	out << "]{";
	header->generate(out);
	out << "}" << endl;
}

void Document::generateTypeFooter(QTextStream &out, Element *footer)
{
	out << "\\fancyfoot[C";
	switch(footer->getInfo())
	{
		case SI_NONE:
		case SI_FIRST:
			break;
		case SI_ODD:
			out << "O";
			break;
		case SI_EVEN:
			out << "E";
			break;
	}
	out << "]{";
	footer->generate(out);
	out << "}" << endl;
}
