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

/*******************************************/
/* Constructor                             */
/*******************************************/
Layout::Layout()
{
	_last_name     =  "STANDARD";
	_last_counter  =  TL_NONE;
	_env           =  ENV_LEFT;
	_counterType   =  TL_NONE;
	_counterDepth  =  0;
	_counterBullet =  0;
	_counterStart  =  0;
	_numberingType = -1;
}

/*******************************************/
/* analyseLAyout                           */
/*******************************************/
void Layout::analyseLayout(const QDomNode balise)
{
	/* Markup type : FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* No parameters for this markup */
	kdDebug() << "ANALYSE OF THE BEGINING OF A LAYOUT" << endl;
	
	/* Analyse children markups */
	for(int index= 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("NAME")== 0)
		{
			kdDebug() << "NAME : " << endl;
			analyseName(getChild(balise, index));
		}
		else if(getChildName(balise, index).compare("FOLLOWING")== 0)
		{
			kdDebug() << "FOLLOWING : " << endl;
			analyseFollowing(getChild(balise, index));
		}
		else if(getChildName(balise, index).compare("FLOW")== 0)
		{
			kdDebug() << "FLOW : " << endl;
			analyseEnv(getChild(balise, index));
		}
		else if(getChildName(balise, index).compare("COUNTER")== 0)
		{
			kdDebug() << "COUNTER : " << endl;
			analyseCounter(getChild(balise, index));
		}
		else if(getChildName(balise, index).compare("FORMAT")== 0)
		{
			kdDebug() << "FORMAT : " << endl;
			Format::analyse(getChild(balise, index));
		}
	}
	kdDebug() << "END OF THE BEGINING OF A LAYOUT" << endl;
}

void Layout::analyseName(const QDomNode balise)
{
	/* <NAME value="times"> */
	kdDebug() << "PARAM" << endl;
	setName(getAttr(balise, "value"));
}

/*******************************************/
/* analyseFollowing                        */
/*******************************************/
/* Get info about folowing. Ununsefull.    */
/*******************************************/
void Layout::analyseFollowing(const QDomNode balise)
{
	/* <FOLLOWING name="times"> */
	kdDebug() << "PARAM" << endl;
	setFollowing(getAttr(balise, "name"));
}

/*******************************************/
/* analyseEnv                              */
/*******************************************/
/* Get informations about environment.     */
/*******************************************/
void Layout::analyseEnv(const QDomNode balise)
{
	/* <FLOW align="0"> */
	// ERROR: Enter first in flow ????
	kdDebug() << "PARAM" << endl;
	if(getAttr(balise,"align").compare("justify")== 0)
		setEnv(ENV_JUSTIFY);
	else if(getAttr(balise, "align").compare("left")== 0)
		setEnv(ENV_LEFT);
	else if(getAttr(balise, "align").compare("right")== 0)
		setEnv(ENV_RIGHT);
	else if(getAttr(balise, "align").compare("center")== 0)
		setEnv(ENV_CENTER);
}

/*******************************************/
/* analyseCounter                          */
/*******************************************/
/* Get all informations about counter.     */
/* If I use a counter, I must include a tex*/
/* package.                                */
/*******************************************/
void Layout::analyseCounter(const QDomNode balise)
{
	/* <COUNTER type="1"> */
	kdDebug() << "PARAM" << endl;
	setCounterType(getAttr(balise, "type").toInt());
	if(getCounterType() > TL_ARABIC && getCounterType() < TL_DISC_BULLET)
	{
		kdDebug() <<  getCounterType() << endl;
		_fileHeader->useEnumerate();
	}
	setCounterDepth(getAttr(balise, "depth").toInt());
	setCounterBullet(getAttr(balise, "bullet").toInt());
	setCounterStart(getAttr(balise, "start").toInt());
	setNumberingType(getAttr(balise, "numberingType").toInt());
}
