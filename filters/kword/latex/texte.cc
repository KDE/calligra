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

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kdDebug() stream */
#include "texte.h"

Texte::Texte()
{
	_left      = 0;
	_right     = 0;
	_top       = 0;
	_bottom    = 0;
	_env       = SE_AUCUN;
	_runaround = false;

	setType(ST_TEXTE);
	setSection(SS_CORPS);
}

void Texte::analyse(const Markup * balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	// MARKUP TYPE : FRAMESET INFO = TEXTE, ENTETE CONNUE
	
	// Parameters Analyse
	Element::analyse(balise_initiale);

	kdDebug() << "ANALYSE D'UNE FRAME (Texte)" << endl;

	// Chlidren markups Analyse
	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "FRAME")== 0)
		{
			analyseParamFrame(balise);
		}
		else if(strcmp(balise->token.zText, "PARAGRAPH")== 0)
		{
			// 1. Create a paragraph :
			Para *prg = new Para;
			// 2. Add the informations :
			prg->analyse(balise);
			// 3. add this parag. in the list
			_liste.add(prg);
			kdDebug() << "PARA AJOUTE" << endl;
		}
		
	}
	kdDebug() << "FIN D'UNE FRAME" << endl;
}

void Texte::analyseParamFrame(const Markup *balise)
{
	//<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />
	Arg *arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
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
	ParaIter iter;
	kdDebug() << "TEXT GENERATION" << endl;
	kdDebug() << "NB PARA " << _liste.getSize() << endl;
	iter.setList(_liste);
	while(!iter.isTerminate())
	{
		iter.getCourant()->generate(out);
		iter.next();
		kdDebug() << iter.getCourant() << endl;
	}
}

