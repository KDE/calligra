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

#include "fileheader.h"		/* for the use of _fileHeader (include graphics package)*/
#include "pictureformat.h"

/*******************************************/
/* analysePictureFormat                    */
/*******************************************/
/* Get the set of info. about a text format*/
/*******************************************/
void PictureFormat::analysePictureFormat(const Markup * balise_initiale)
{
	Token*  savedToken = 0;
	Markup* balise     = 0;

	/* MARKUPS FORMAT id="1" pos="0" len="17">
	 * <IMAGE>
	 * 	<FILENAME value="/my/pictures/directory/filename.png">
	 * </IMAGE>
	 * </FORMAT> */
	
	/* Parameters Analyse */
	analyseParam(balise_initiale);
	kdDebug() << "ANALYSE A PICTURE ZONE" << endl;
	
	/* Children Markups Analyse */
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "IMAGE")== 0)
		{
			kdDebug() << "IMAGE : " << endl;
			analyseImage(balise);
		}
	}
	kdDebug() << "END OF A PICTURE ZONE" << endl;
}

/*******************************************/
/* analyseParam                            */
/*******************************************/
/* Get the zone where the format is applied*/
/*******************************************/
void PictureFormat::analyseParam(const Markup *balise)
{
	/* <FORMAT id="1" pos="0" len="17"> */
	Arg *arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "ID")== 0)
		{
			//setId(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "POS")== 0)
		{
			setPos(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LEN")== 0)
		{
			setLength(atoi(arg->zValue));
		}
	}
}


/*******************************************/
/* analyseImage                            */
/*******************************************/
/* Get the image filename                  */
/*******************************************/
void PictureFormat::analyseImage(const Markup *balise_initiale)
{
	Token*  savedToken = 0;
	Markup* balise     = 0;
	Arg *arg = 0;

	/* Children Markups Analyse */
	savedToken = enterTokenChild(balise_initiale);
	
	while((balise = getNextMarkup()) != NULL)
	{
		if(strcmp(balise->token.zText, "FILENAME")== 0)
		{
			kdDebug() << "FILENAME : " << endl;
			for(arg= balise->pArg; arg; arg= arg->pNext)
			{
				//kdDebug() << "PARAM " << arg->zName << endl;
				if(strcmp(arg->zName, "VALUE")== 0)
				{
					kdDebug() << arg->zValue << endl;
					setFilename(arg->zValue);
					//setPathname(arg->zValue);
					_fileHeader->useGraphics();
				}
			}
		}
	}

}
