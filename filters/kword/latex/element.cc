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
#include <iostream.h>
#include <kdebug.h>
#include "element.h"

Element::Element()
{
	_type = ST_AUCUN;
	_name = 0;
	_suivant = 0;
}

void Element::analyse(const Markup * balise_initiale)
{
	//Token *savedToken;
	//Markup *balise = 0;

	// ON A UNE BALISE DE TYPE FRAMESET
	
	// Analyse des paramètres
	printf("ANALYSE DES PARAMETRES D'UNE FRAMESET\n");
	analyse_param(balise_initiale);
	// Analyse des parametres d'une FRAME
	//
	//savedToken = enterTokenChild(balise_initiale);
	/*while((balise = getNextMarkup()) != 0)
	{

		if(strcmp(balise->token.zText, "FRAME")== 0)
		{
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
	}
	setTokenCurrent(savedToken);
	*/
}

void Element::analyse_param(const Markup *balise)
{
	// <FRAMESET frameType="1" frameInfo="0" removable="0" visible="1"
	// name="Supercadre 1">
	Arg *arg;

	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		cout << "param : " << arg->zName << " " << arg->zValue << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			_name= strdup(arg->zValue);
		}
		else if(strcmp(arg->zName, "FRAMETYPE")== 0)
		{
			// A FINIR
			cout << "TYPE : TEXTE" << endl;
			_type = ST_TEXTE;
		}
		else if(strcmp(arg->zName, "FRAMEINFO")== 0)
		{
			cout << "INFO :" << arg->zValue << endl;
			switch(atoi(arg->zValue))
			{
				case 0: _section = SS_CORPS;
					break;
				case 1: _section = SS_ENTETE;
					break;
				case 2: _section = SS_ENTETE;
					break;
				case 3: _section = SS_ENTETE;
					break;
				case 4: _section = SS_ENTETE;
					break;
				case 5: _section = SS_ENTETE;
					break;
				case 6: _section = SS_ENTETE;
					break;
				case 7: _section = SS_ENTETE;
					break;
				default: cerr << "erreur frameinfo inconnue!" << endl;
			}
		}
	}
	cout << "FIN PARAM" << endl;
}

/*void Element::generate(QTextStream &out)
{
	// A priori inutile puisque surcharge par Texte, Image, ...
	//out << "%%%%%%%%%%%%%%%%%%%%%%%%%\n% Nouvelle zone" << endl;
	//fprintf(out, )"%%%%%%%%%%%%%%%%%%%%%%%%%\n% Nouvelle zone\n";
}*/

