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
#include "header.h"

Header::Header()
{
}

void Header::setPaper(Markup * balise)
{
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		if(strcmp(arg->zName, "FORMAT")== 0)
		{
			//_format = arg->zValue;
		}
	}
	// Parcours des enfants --> PAPERBORDERS
	
	
}

void Header::setAttributs(Markup *balise)
{
	Arg *arg;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		if(strcmp(arg->zName, "UNIT")== 0)
		{
			//_unit = arg->zValue;
		}
	}
}

void Header::generate(QTextStream &out)
{
	out << "\\documentclass[a4paper,11pt]{article}" << endl;
	//fprintf(out, "\\documentclass[a4paper,11pt]{article}\n");
	// Parcourir l'entete et le document pour generer le fichier.
	// _header.genere(_outputFile);
	// _document.genere(_outputFile);
}

