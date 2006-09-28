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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <stdlib.h>	/* for atoi function */

#include <kdebug.h>	/* for kDebug() stream */

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
	_useHardBreakAfter = false;
	_useHardBreak      = false;
	_keepLinesTogether = false;
}

/*******************************************/
/* analyzeLayout                           */
/*******************************************/
void Layout::analyzeLayout(const QDomNode node)
{
	/* Markup: <FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* No parameters for this markup */
	kDebug(30522) << "ANALYSIS OF THE BEGINNING OF A LAYOUT" << endl;
	
	/* Analyze child markups */
	for(int index= 0; index < getNbChild(node); index++)
	{
		if(getChildName(node, index).compare("NAME")== 0)
		{
			kDebug(30522) << "NAME: " << endl;
			analyzeName(getChild(node, index));
		}
		else if(getChildName(node, index).compare("FOLLOWING")== 0)
		{
			kDebug(30522) << "FOLLOWING: " << endl;
			analyzeFollowing(getChild(node, index));
		}
		else if(getChildName(node, index).compare("FLOW")== 0)
		{
			kDebug(30522) << "FLOW: " << endl;
			analyzeEnv(getChild(node, index));
		}
		else if(getChildName(node, index).compare("PAGEBREAKING")== 0)
		{
			kDebug(30522) << "PAGEBREAKING: " << endl;
			analyzeBreakLine(getChild(node, index));
		}
		else if(getChildName(node, index).compare("COUNTER")== 0)
		{
			kDebug(30522) << "COUNTER: " << endl;
			analyzeCounter(getChild(node, index));
		}
		else if(getChildName(node, index).compare("FORMAT")== 0)
		{
			kDebug(30522) << "FORMAT: " << endl;
			analyzeFormat(getChild(node, index));
		}
	}
	kDebug(30522) << "END OF THE BEGINNING OF A LAYOUT" << endl;
}

void Layout::analyzeName(const QDomNode node)
{
	/* <NAME value="times"> */
	kDebug(30522) << "PARAM" << endl;
	setName(getAttr(node, "value"));
}

/*******************************************/
/* analyzeFollowing                        */
/*******************************************/
/* Get info about following.               */
/*******************************************/
void Layout::analyzeFollowing(const QDomNode node)
{
	/* <FOLLOWING name="times"> */
	kDebug(30522) << "PARAM" << endl;
	setFollowing(getAttr(node, "name"));
}

/*******************************************/
/* analyzeEnv                              */
/*******************************************/
/* Get information about environment.      */
/*******************************************/
void Layout::analyzeEnv(const QDomNode node)
{
	/* <FLOW align="0"> */
	// ERROR: Enter first in flow ????
	kDebug(30522) << "PARAM" << endl;
	if(getAttr(node,"align").compare("justify")== 0)
		setEnv(ENV_JUSTIFY);
	else if(getAttr(node, "align").compare("left")== 0)
		setEnv(ENV_LEFT);
	else if(getAttr(node, "align").compare("right")== 0)
		setEnv(ENV_RIGHT);
	else if(getAttr(node, "align").compare("center")== 0)
		setEnv(ENV_CENTER);
}

void Layout::analyzeBreakLine(const QDomNode node)
{
	/* <NAME hardFrameBreakAfter="true"> */
	kDebug(30522) << "PARAM" << endl;
	if(getAttr(node, "linesTogether") != 0)
		keepLinesTogether();
	else if(getAttr(node, "hardFrameBreak") != 0)
		useHardBreak();
	else if(getAttr(node, "hardFrameBreakAfter") != 0)
		useHardBreakAfter();
}

/*******************************************/
/* analyzeCounter                          */
/*******************************************/
/* Get all information about counter.      */
/* If I use a counter, I must include a tex*/
/* package.                                */
/*******************************************/
void Layout::analyzeCounter(const QDomNode node)
{
	/* <COUNTER type="1"> */
	kDebug(30522) << "PARAM" << endl;
	setCounterType(getAttr(node, "type").toInt());
	if(getCounterType() > TL_ARABIC && getCounterType() < TL_DISC_BULLET)
	{
		kDebug(30522) <<  getCounterType() << endl;
		FileHeader::instance()->useEnumerate();
	}
	setCounterDepth(getAttr(node, "depth").toInt());
	setCounterBullet(getAttr(node, "bullet").toInt());
	setCounterStart(getAttr(node, "start").toInt());
	setNumberingType(getAttr(node, "numberingtype").toInt());
}
