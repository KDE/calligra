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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <kdebug.h>
#include <iostream.h>
#include "listepara.h"

ListPara::ListPara()
{
	cout << "create liste para vide" << endl;
	_start = 0;
	_end   = 0;
	_size  = 0;
}

void ListPara::initialiser(Para *elt)
{
	cout << "initialisation liste para a " << elt << endl;
	_end = _start = elt;
}

void ListPara::add(Para *elt)
{
	if(_start == 0)
	{
		initialiser(elt);
		_size = 1;
	}
	else
	{
		cout << "ajout d'un parag." << endl;
		_end->setNext(elt);
		_end = elt;
		_size = _size + 1;
	}
}


Para::Para()
{
	_texte   = 0;
	_liste   = 0;
	_suivant = 0;
}

void Para::analyse(const Markup * balise_initiale)
{
	Token *savedToken;
	Token *p;

	// ON A UNE BALISE DE TYPE PARAGRAPH
	Markup *balise;
	
	// Analyse des paramètres
	//analyse_param(balise_initiale);
	cout << "ANALYSE D'UN PARAGRAPHE" << endl;
	
	// Analyse des balises filles
	savedToken = enterTokenChild(balise_initiale);
	//PrintXml(savedToken, 2);
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "TEXT")== 0)
		{
			//_texte = balise->pContent->zText;
			//PrintXml(balise->pContent, 2);
			for(p = balise->pContent; p; p = p->pNext)
			{
				if(_texte == 0)
					_texte = strdup(p->zText);
				else
					strcat(_texte, p->zText);
			}
			cout << "TEXTE : " << _texte << endl;
			// 1. Creer un fils suivant le type 
			// 2. elt.analyse(balise);
			// 3. Ajouter l'Element dans une des listes
			// switch(elt.getSection())
			// {
			// 	case TS_ENTETE: _enTete.add(elt);
			// 		break;
			// 	case TS_CORPS: _corps.add(elt);
			// 		break;
			// 	default: 
			// }
		}
		else if(strcmp(balise->token.zText, "FORMATS")== 0)
		{
			// IMPORTANT ==> police + style
			cout << "FORMATS" << endl;
			analyse_formats(balise);
			
		}
		else if(strcmp(balise->token.zText, "LAYOUT")== 0)
		{
		}
	}
	cout << "FIN DE PARAGRAPHE" << endl;
	setTokenCurrent(savedToken);
}

void Para::analyse_formats(const Markup *balise_initiale)
{
	Token *savedToken = 0;
	Markup *balise = 0;

	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != NULL)
	{
		TextZone *texte = new TextZone(_texte);
		cout << "FORMAT" << endl;
		if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			texte->analyse(balise);
			cout << "ok" << endl;
			if(_liste == 0)
				_liste = new ListeTextZone;
			_liste->add_last(texte);
		}
		cout << "FIN FORMAT" << endl;
	}
	setTokenCurrent(savedToken);
	cout << "FIN FORMATS" << endl;
}

/*void Para::analyse_param(const Markup *balise)
{
	
}*/

void Para::generate(QTextStream &out)
{
	//fprintf(out, "\\begin{document}\n");
	// Parcourir les elements pour generer le fichier.
	// Pour chaque entete - footnote
	// _entete.genere(_outputFile);
	// Pour chaque corps de texte
	// _element.genere(_outputFile);
	cout << "  GENERATION PARA" << endl;
	if(_liste != 0)
	{
		TextZoneIter iter(_liste);
		cout << "  NB ZONE : " << _liste->get_size() << endl;
		while(!iter.is_terminate())
		{
			iter.get_courant()->generate(out);
			iter.next();
		}
	}
	out << endl;
	cout << "PARA GENERATED" << endl;
	//out << _texte << endl;
	//fprintf(out, "%s\n", _texte);
}

