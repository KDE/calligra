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

#include "header.h"

Header::Header()
{
	_hasHeader = false;
	_hasFooter = false;
}

Header::~Header()
{
	kdDebug() << "Header Destructor" << endl;
}

void Header::analysePaper(const Markup * balise_initiale)
{
	Markup* balise = 0;
	Arg*    arg    = 0;

	// Get parameters
	for(arg= balise_initiale->pArg; arg; arg= arg->pNext)
	{
		if(strcmp(arg->zName, "FORMAT")== 0)
		{
			setFormat(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "ORIENTATION")== 0)
		{
			setOrientation(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "COLUMNS")== 0)
		{
			if(atoi(arg->zValue) > 2)
				setColumns(TC_MORE);
			else
				setColumns(atoi(arg->zValue) - 1);
		}
	}

	setTokenCurrent(balise_initiale->pContent);
	// Analyse children markups --> PAPERBORDERS
	while((balise = getNextMarkup()) != 0)
	{
		kdDebug() << balise << endl;
		kdDebug() << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "PAPERBORDERS")== 0)
		{
			/* Nothing done now */
		}
	}
	
}

void Header::analyseAttributs(const Markup *balise)
{
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		if(strcmp(arg->zName, "UNIT")== 0)
		{
			setUnit(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "HASHEADER")== 0)
		{
			_hasHeader = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "HASFOOTER")== 0)
		{
			_hasFooter = atoi(arg->zValue);
		}
	}
}

void Header::generate(QTextStream &out)
{
	kdDebug() << "GENERATION OF THE FILE HEADER" << endl;
	out << "\\documentclass";
	switch(getFormat())
	{
		case TF_A3:
			out << "";
			break;
		case TF_A4:
			out << "[a4paper, ";
			break;
		case TF_A5:
			out << "[a5paper, ";
			break;
		case TF_USLETTER:
			out << "[letterpaper, ";
			break;
		case TF_USLEGAL:
			out << "[legalpaper, ";
			break;
		case TF_SCREEN:
			out << "";
			break;
		case TF_CUSTOM:
			out << "";
			break;
		case TF_B3:
			out << "";
			break;
		case TF_USEXECUTIVE:
			out << "[executivepaper, ";
			break;
	}
	if(getOrientation() == TO_LANDSCAPE)
		out << "landscape, ";
	/* To change : will use a special latexcommand to able to
	 * obtain more than one column :))
	 */
	switch(getColumns())
	{
		case TC_1:
			out << "onecolumn, ";
			break;
		case TC_2:
			out << "twocolumn, ";
			break;
		case TC_MORE:
			out << "";
	}
	/* the font and the type of the doc. can not be changed, hmm ? */
	out << "11pt]{article}" << endl;
}

