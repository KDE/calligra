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

/*******************************************/
/* Constructor                             */
/*******************************************/
Element::Element()
{
	_type      = ST_NONE;
	_hinfo     = SI_NONE;
	_section   = SS_NONE;
	_name      = 0;
	_removable = false;
	_visible   = true;
	_row       = 0;
	_col       = 0;
	_rows      = 0;
	_cols      = 0;
	setGrpMgr("");
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Element::~Element()
{
	kdDebug() << "Element Destructor" << endl;
	if(_name != 0)
		delete _name;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Element::analyse(const Markup * balise_initiale)
{
	/* ANALYSE A FRAMESET MARKUP */
	
	/* Parameters Analyse */
	kdDebug() << "FRAMESET PARAMETERS ANALYSE (Element)" << endl;
	analyseParam(balise_initiale);
}

/*******************************************/
/* AnalyseParam                            */
/*******************************************/
void Element::analyseParam(const Markup *balise)
{
	/* <FRAMESET frameType="1" frameInfo="0" removable="0" visible="1"
	 * name="Supercadre 1"> */
	Arg *arg = 0;

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
			switch(atoi(arg->zValue))
			{
				case 0: _type = ST_NONE;
					break;
				case 1: _type = ST_TEXT;
					break;
				case 2: _type = ST_PICTURE;
					break;
				case 3: _type = ST_PART;
					break;
				case 4: _type = ST_FORMULA;
					break;
				default:
					_type = ST_NONE;
					kdDebug() << "error : frameinfo unknown!" << endl;
			}
		}
		else if(strcmp(arg->zName, "FRAMEINFO")== 0 && _section == SS_NONE)
		{
			/* If _section != NONE, it's a table. */
			switch(atoi(arg->zValue))
			{
				case 0: _section = SS_BODY;
					break;
				case 1: _section = SS_HEADERS;
					_hinfo   = SI_FIRST;
					break;
				case 2: _section = SS_HEADERS;
					_hinfo   = SI_ODD;
					break;
				case 3: _section = SS_HEADERS;
					_hinfo   = SI_EVEN;
					break;
				case 4: _section = SS_FOOTERS;
					_hinfo   = SI_FIRST;
					break;
				case 5: _section = SS_FOOTERS;
					_hinfo   = SI_ODD;
					break;
				case 6: _section = SS_FOOTERS;
					_hinfo   = SI_EVEN;
					break;
				case 7: _section = SS_FOOTNOTES;
					break;
				default:
					_section = SS_NONE;
					kdDebug() << "error : frameinfo unknown!" << endl;
			}
		}
		else if(strcmp(arg->zName, "REMOVABLE") == 0)
		{
			setRemovable(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "VISIBLE") == 0)
		{
			setVisible(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "GRPMGR") == 0)
		{
			/* It's a table !! */
			_section = SS_TABLE;
			setGrpMgr(arg->zValue);
		}
		else if(strcmp(arg->zName, "ROW") == 0)
		{
			setRow(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "COL") == 0)
		{
			setCol(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "ROWS") == 0)
		{
			setRows(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "COLS") == 0)
		{
			setCols(atoi(arg->zValue));
		}
	}
	kdDebug() << "FIN PARAM" << endl;
}
