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

#include <stdlib.h>
#include <kdebug.h>
#include "element.h"

Element::Element()
{
	_type    = ST_AUCUN;
	_hinfo   = SI_NONE;
	_name    = 0;
	_suivant = 0;
}

Element::~Element()
{
	kdDebug() << "Element Destructor" << endl;
	if(_name != 0)
		delete _name;
}
	
void Element::analyse(const Markup * balise_initiale)
{
	// ANALYSE A FRAMESET MARKUP
	
	// Parameters Analyse
	kdDebug() << "ANALYSE DES PARAMETRES D'UNE FRAMESET (Element)" << endl;
	analyseParam(balise_initiale);
}

void Element::analyseParam(const Markup *balise)
{
	// <FRAMESET frameType="1" frameInfo="0" removable="0" visible="1"
	// name="Supercadre 1">
	Arg *arg;

	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "param : " << arg->zName << " " << arg->zValue << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			_name = strdup(arg->zValue);
		}
		else if(strcmp(arg->zName, "FRAMETYPE")== 0)
		{
			// TO FINISH
			kdDebug() << "TYPE : TEXTE" << endl;
			_type = ST_TEXTE;
		}
		else if(strcmp(arg->zName, "FRAMEINFO")== 0)
		{
			kdDebug() << "INFO :" << arg->zValue << endl;
			switch(atoi(arg->zValue))
			{
				case 0: _section = SS_CORPS;
					break;
				case 1: _section = SS_ENTETE;
					_hinfo   = SI_FIRST;
					break;
				case 2: _section = SS_ENTETE;
					_hinfo   = SI_ODD;
					break;
				case 3: _section = SS_ENTETE;
					_hinfo   = SI_EVEN;
					break;
				case 4: _section = SS_PIEDS;
					_hinfo   = SI_FIRST;
					break;
				case 5: _section = SS_PIEDS;
					_hinfo   = SI_ODD;
					break;
				case 6: _section = SS_PIEDS;
					_hinfo   = SI_EVEN;
					break;
				case 7: _section = SS_ENTETE;
					break;
				default:
					kdDebug() << "error : frameinfo unknown!" << endl;
			}
		}
	}
	kdDebug() << "FIN PARAM" << endl;
}
