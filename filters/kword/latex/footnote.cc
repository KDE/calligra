
/* BUGS : latex don't support alpha list with one command !!! the
 * command generated doesn't exist :))))
 */

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
#include "footnote.h"
#include "texte.h"		/* for generate function (catch text footnote) */

Footnote::Footnote(Para* para): Format(para)
{
}

Footnote::~Footnote()
{
	kdDebug() << "Destruction of a footnote." << endl;
}

/* Modifiers */
void Footnote::setSpace (char* new_space)
{
	if(new_space != 0)
		_space = *new_space;
}

void Footnote::setBefore(char* new_before)
{
	if(new_before != 0)
		_before = *new_before;

}

void Footnote::setAfter(char* new_after)
{
	if(new_after != 0)
		_after = *new_after;
}

void Footnote::setRef(char* new_ref)
{
	if(new_ref != 0)
		_ref = new_ref;
}

void Footnote::analyse(const Markup * balise_initiale)
{
	Token*  savedToken = 0;
	Markup* balise     = 0;

	// MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT>
	
	// Parameters Analyse
	//analyseParam(balise_initiale);
	kdDebug() << "ANALYSE A FOOTNOTE" << endl;
	
	// Children Markups Analyse
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "INTERNAL")== 0)
		{
			kdDebug() << "INTERNAL : " << endl;
			analyseInternal(balise);
		}
		else if(strcmp(balise->token.zText, "RANGE")== 0)
		{
			kdDebug() << "RANGE : " << endl;
			analyseRange(balise);
		}
		else if(strcmp(balise->token.zText, "TEXT")== 0)
		{
			kdDebug() << "TEXT : " << endl;
			analyseText(balise);
		}
		else if(strcmp(balise->token.zText, "DESCRIPT")== 0)
		{
			kdDebug() << "DESCRIPT : " << endl;
			analyseDescript(balise);
		}
		else if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			kdDebug() << "SUBFORMAT : " << endl;
			Format::analyse(balise);
		}
	}
	kdDebug() << "END OF FOOTNOTE" << endl;
}

void Footnote::analyseInternal(const Markup * balise_initiale)
{
	Token*  savedToken = 0;
	Markup* balise     = 0;
	Arg*    arg        = 0;

	// MARKUPS <INTERNAL> <PART from="1" to="-1" space="-"/>

	// Children Markups Analyse
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "PART")== 0)
		{
			kdDebug() << "PART : " << endl;

			for(arg= balise->pArg; arg; arg= arg->pNext)
			{
				kdDebug() << "PARAM " << arg->zName << endl;
				if(strcmp(arg->zName, "FROM")== 0)
				{
					setFrom(atoi(arg->zValue));
				}
				else if(strcmp(arg->zName, "TO")== 0)
				{
					setTo(atoi(arg->zValue));
				}
				else if(strcmp(arg->zName, "SPACE")== 0)
				{
					setSpace(arg->zValue);
				}
			}
		}
	}
	setTokenCurrent(savedToken);
}

void Footnote::analyseRange(const Markup * balise)
{
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "START")== 0)
		{
			setStart(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "END")== 0)
		{
			setEnd(atoi(arg->zValue));
		}
	}
}

void Footnote::analyseText(const Markup * balise)
{
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "BEFORE")== 0)
		{
			setBefore(arg->zValue);
		}
		else if(strcmp(arg->zName, "AFTER")== 0)
		{
			setAfter(arg->zValue);
		}
	}
}

void Footnote::analyseDescript(const Markup * balise)
{
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "REF")== 0)
		{
			setRef(arg->zValue);
		}
	}
}

void Footnote::generate(QTextStream &out)
{
	Para *footnote = 0;

	kdDebug() << "  GENERATION FOOTNOTE" << endl;
	// Go to keep the footnote parag.
	// then write it with this format.
	// like this : \,\footnote{the parag. }
	out << "\\,\\footnote{";
	kdDebug() << "footnote : " << _ref << endl;
	if((footnote = getPara()->getTexte()->searchFootnote(_ref)) != 0)
		footnote->generate(out);
	out << "}";
	kdDebug() << "FOOTNOTE GENERATED" << endl;
}


