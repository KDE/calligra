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
void PictureFormat::analysePictureFormat(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">
	 * <IMAGE>
	 * 	<FILENAME value="/my/pictures/directory/filename.png">
	 * </IMAGE>
	 * </FORMAT> */
	
	/* Parameters Analyse */
	analyseParam(balise);
	kdDebug() << "ANALYSE A PICTURE ZONE" << endl;

	/* Children Markups Analyse */
	analyseImage(getChild(balise, "IMAGE"));
	kdDebug() << "END OF A PICTURE ZONE" << endl;
}

/*******************************************/
/* analyseParam                            */
/*******************************************/
/* Get the zone where the format is applied*/
/*******************************************/
void PictureFormat::analyseParam(const QDomNode balise)
{
	/* <FORMAT id="1" pos="0" len="17"> */
	//setId(getAttr(balise, "ID").toInt());
	setPos(getAttr(balise, "POS").toInt());
	setLength(getAttr(balise, "LEN").toInt());
}


/*******************************************/
/* analyseImage                            */
/*******************************************/
/* Get the image filename                  */
/*******************************************/
void PictureFormat::analyseImage(const QDomNode balise)
{
	QDomNode fils;
	
	/* Children Markups Analyse */
	fils = getChild(balise, "FILENAME");
	setFilename(getAttr(fils, "VALUE"));
	//setPathname(arg->zValue);
	_fileHeader->useGraphics();
}
