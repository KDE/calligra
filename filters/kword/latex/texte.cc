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
#include "texte.h"

Texte::Texte()
{
	_left   = 0;
	_right  = 0;
	_top    = 0;
	_bottom = 0;
	_runaround = false;
}

void Texte::analyse(const Markup * balise_initiale)
{
	Token *savedToken;

	// ON A UNE BALISE DE TYPE FRAMESET INFO = TEXTE, ENTETE CONNUE
	Markup *balise;
	
	// Analyse des paramètres
	//analyse_param_frame(balise_initiale);
	_liste.initialiser(0);
	Element::analyse(balise_initiale);
	// Analyse des balises filles
	cout << "ANALYSE D'UNE FRAME" << endl;
	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "FRAME")== 0)
		{
			analyse_param_frame(balise);
		}
		else if(strcmp(balise->token.zText, "PARAGRAPH")== 0)
		{
			// 1. Creer un paragraphe
			Para *prg = new Para();
			// 2. Ajouter les infos
			prg->analyse(balise);
			// 3. ajouter le parag. dans la liste
			_liste.add(prg);
			cout << "PARA AJOUTE" << endl;
		}
		
	}
	cout << "FIN D'UNE FRAME" << endl;
	setTokenCurrent(savedToken);
}

void Texte::analyse_param_frame(const Markup *balise)
{
	//<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		cout << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "LEFT")== 0)
		{
			_left = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "TOP")== 0)
		{
			_top = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "RIGHT")== 0)
		{
			_right = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "BOTTOM")== 0)
		{
			_bottom = atoi(arg->zValue);
		}
	}
}

void Texte::generate(QTextStream &out)
{
	ParaIter iter(_liste);
	cout << "GENERATION TEXTE" << endl;
	cout << "NB PARA " << _liste.getSize() << endl;
	while(!iter.is_terminate())
	{
		cout <<".\\";
		iter.get_courant()->generate(out);
		iter.next();
		cout << iter.get_courant() << endl;
	}
	out << "%%%%%" << endl;
}

