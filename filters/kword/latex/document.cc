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

#include "document.h"
#include "texte.h"

Document::Document()
{
}

void Document::analyse(const Markup * balise_initiale)
{
	Markup *balise = 0;
		
	setTokenCurrent(balise_initiale->pContent);
	while((balise = getNextMarkup()) != 0)
	{
		cout << balise << endl;
		cout << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "FRAMESET")== 0)
		{
			//Element *elt = new Element;
			Texte *elt = new Texte;
			cout <<"ANALYSE D'UNE FRAMESET" << endl;
			// 1. Creer un fils suivant le type 
			//elt->analyse(balise);
			//printf("%d\n", elt->getType());
			// 2. Ajouter les parametres specifiques a la frame
			//cout << elt->getType() << endl;
			switch(get_type_frameset(balise))
			{
				case ST_AUCUN:
					cout << "AUCUN" << endl;
					break;
				case ST_IMAGE:
					cout << "IMAGE" << endl;
					// ((Image*) elt)->analyse(balise);
					break;
				case ST_TEXTE: //Texte *elt = new Texte;
					cout << "TEXTE" << endl;
					elt->analyse(balise);
					break;
				case ST_PARTS:
					break;
				default: cerr << "erreur " << elt->getType() << " " << ST_TEXTE << endl;
			}
			
			// 3. Ajouter l'Element dans une des listes
			cout << "INFO : " << elt->getSection();
			switch(elt->getSection())
			{
				case SS_ENTETE: cout << " ENTETE" << endl;
						_enTete.add(elt);
					break;
				case SS_CORPS: 	_corps.add(elt);
						cout << " CORPS" << endl;
					break;
				default: cout << "INCONNU" << endl;
					break;
			}
		}
		cout << "FIN D'ANALYSE DE FRAMESET" << endl;
	}
	
}

SType Document::get_type_frameset(const Markup *balise)
{
	// <FRAMESET frameType="1" frameInfo="0" removable="0" visible="1"
	// name="Supercadre 1">
	Arg *arg;
	SType type = ST_AUCUN;

	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		cout << "param : " << arg->zName << " " << arg->zValue << endl;
		if(strcmp(arg->zName, "FRAMETYPE")== 0)
		{
			// A FINIR
			cout << "TYPE : TEXTE" << endl;
			type = ST_TEXTE;
		}
	}
	cout << "FIN TYPE" << endl;
	return type;
}

void Document::generate(QTextStream &out)
{
	ElementIter iter1(_enTete);
	ElementIter iter2(_corps);

	out << "\\begin{document}" << endl;
	// Pour chaque entete - footnote
	cout << "entete : " << _enTete.getSize() << endl;
	while(!iter1.is_terminate())
	{
		cout << ".";
		//iter1.get_courant()->generate(out);
		cout << "\\";
		iter1.next();
		cout << iter1.get_courant() << endl;
	}

	cout << endl << "corps : " << _corps.getSize() << endl;
	// Pour chaque corps d etexte
	while(!iter2.is_terminate())
	{
		cout << "." << endl;
		if(iter2.get_courant()->getType() == ST_TEXTE)
		{
			((Texte *) iter2.get_courant())->generate(out);
		}
		iter2.next();
	}
	cout << endl;
	out << "\\end{document}" << endl;
}

