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

#include <kdebug.h>		/* for kdDebug() stream */

#include "pixmap.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Pixmap::Pixmap()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Pixmap::~Pixmap()
{
	kdDebug() << "Destruction of a pixmap" << endl;
}
void Pixmap::setKeepAspectRatio(const QString ratio)
{
	if(ratio == "true")
		_keepAspectRatio = true;
	else
		_keepAspectRatio = false;
}

/*******************************************/
/* analyse                                 */
/*******************************************/
/* Get informations about a pixmap stored  */
/* in the tared file.                      */
/*******************************************/
void Pixmap::analyse(const QDomNode balise)
{
	/* MARKUP TYPE : FRAMESET INFO = TEXTE, ENTETE CONNUE */

	/* Parameters Analyse */
	Element::analyse(balise);

	kdDebug() << "FRAME ANALYSE (Pixmap)" << endl;

	/* Chlidren markups Analyse */
	for(int index = 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("FRAME")== 0)
		{
			analyseParamFrame(balise);
		}
		else if(getChildName(balise, index).compare("PICTURE")== 0)
		{
			getPixmap(getChild(balise, "PICTURE"));
		}

	}
	kdDebug() << "END OF A FRAME" << endl;
}

/*******************************************/
/* getPixmap                               */
/*******************************************/
/* Get informations about a pixmap stored  */
/* in the tared file.                      */
/*******************************************/
void Pixmap::getPixmap(const QDomNode balise_initiale)
{
	kdDebug() << "PIXMAP" << endl;
	setKeepAspectRatio(getAttr(balise_initiale, "keepAspectRatio"));
	QDomNode balise = getChild(balise_initiale, "KEY");
	setKey(getAttr(balise, "filename"));
	setFilename(getAttr(balise, "name"));
	QString file = getFilename();
	int pos = file.findRev('.');
	file.truncate(pos);
	setFilenamePS(file + ".eps");
	kdDebug() << "PS : " << getFilenamePS() << file << endl;
	kdDebug() << "END PIXMAP" << endl;
}

/*******************************************/
/* analyseParamFrame                       */
/*******************************************/
void Pixmap::analyseParamFrame(const QDomNode balise)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/

	_left = getAttr(balise, "left").toInt();
	_top = getAttr(balise, "top").toInt();
	_right = getAttr(balise, "right").toInt();
	_bottom = getAttr(balise, "bottom").toInt();
	setRunAround(getAttr(balise, "runaround").toInt());
	setAroundGap(getAttr(balise, "runaroundGap").toInt());
	setAutoCreate(getAttr(balise, "autoCreateNewFrame").toInt());
	setNewFrame(getAttr(balise, "newFrameBehaviour").toInt());
	setSheetSide(getAttr(balise, "sheetside").toInt());
}

/*******************************************/
/* generate                                */
/*******************************************/
/* Generate the text formated (if needed). */
/*******************************************/
void Pixmap::generate(QTextStream &out)
{

	//generate_format_begin(out);

	//convert(getFilename());
	out << "\\includegraphics{" << getFilenamePS()<< "}" << endl;

	//generate_format_end(out);
}

