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

#include <stdlib.h>	/* for atoi function */

#include <kdebug.h>	/* for kdDebug() stream */

#include "fileheader.h"	/* for _header use */
#include "layout.h"

/* Static Datas */
QString Layout::_last_name;
EType   Layout::_last_counter;

Layout::Layout()
{
	_last_name     =  "STANDARD";
	_last_counter  =  TL_STANDARD;
	_env           =  ENV_LEFT;
	_counterType   =  TL_STANDARD;
	_counterDepth  =  0;
	_counterBullet =  0;
	_counterStart  =  0;
	_numberingType = -1;
}

void Layout::analyseLayout(const Markup * balise_initiale)
{
	Token*  savedToken = 0;
	Markup* balise     = 0;

	// Markup type : FORMAT id="1" pos="0" len="17">...</FORMAT>
	
	// No parameters for this markup
	kdDebug() << "ANALYSE OF THE BEGINING OF A LAYOUT" << endl;
	
	// Analyse children markups
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "NAME")== 0)
		{
			kdDebug() << "NAME : " << endl;
			analyseName(balise);
		}
		else if(strcmp(balise->token.zText, "FOLLOWING")== 0)
		{
			kdDebug() << "FOLLOWING : " << endl;
			analyseFollowing(balise);
		}
		else if(strcmp(balise->token.zText, "FLOW")== 0)
		{
			kdDebug() << "FLOW : " << endl;
			analyseEnv(balise);
		}
		else if(strcmp(balise->token.zText, "COUNTER")== 0)
		{
			kdDebug() << "COUNTER : " << endl;
			analyseCounter(balise);
		}
		/*else if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			kdDebug() << "FORMAT : " << endl;
			Format::analyse(balise);
		}*/
	}
	kdDebug() << "END OF THE BEGINING OF A LAYOUT" << endl;
	setTokenCurrent(savedToken);
}

void Layout::analyseName(const Markup *balise)
{
	//<NAME value="times">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			setName(arg->zValue);
			kdDebug() << arg->zValue << endl;
		}
	}
}

void Layout::analyseFollowing(const Markup *balise)
{
	//<FOLLOWING name="times">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			setFollowing(arg->zValue);
			kdDebug() << arg->zValue << endl;
		}
	}
}

void Layout::analyseEnv(const Markup *balise)
{
	//<FLOW value="0">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "VALUE")== 0)
		{
			setEnv(atoi(arg->zValue));
			kdDebug() << arg->zValue << endl;
		}
	}
}

void Layout::analyseCounter(const Markup *balise)
{
	//<COUNTER type="1">
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "TYPE")== 0)
		{
			setCounterType(atoi(arg->zValue));
			if(getCounterType() > TL_ARABIC && getCounterType() < TL_BULLET)
			{
				kdDebug() <<  getCounterType() << endl;
				_fileHeader->useEnumerate();
			}
		}
		else if(strcmp(arg->zName, "DEPTH")== 0)
		{
			setCounterDepth(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BULLET")== 0)
		{
			setCounterBullet(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "START")== 0)
		{
			setCounterStart(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "NUMBERINGTYPE")== 0)
		{
			setNumberingType(atoi(arg->zValue));
		}
	}
}
