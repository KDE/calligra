/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2002 Robert JACOLIN
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

#include <kdebug.h>		/* for kDebug() stream */
#include "key.h"
//Added by qt3to4:
#include <QTextStream>

/*******************************************/
/* Constructor                             */
/*******************************************/
Key::Key(eKeyType type): _type(type)
{
	_name = "";
	_filename = "";
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Key::~Key()
{
	kDebug(30522) << "Destruction of a key." << endl;
}

/*******************************************/
/* Analyze                                 */
/*******************************************/
void Key::analyze(const QDomNode balise)
{
	/* Markup type: Paragraph */

	kDebug(30522) << "**** KEY ****" << endl;
	kDebug(30522) << getAttr(balise, "name") << endl;
	setName(getAttr(balise, "name"));
	setFilename(getAttr(balise, "filename"));
	setHour(getAttr(balise, "hour").toInt());
	setMSec(getAttr(balise, "msec").toInt());
	setDay(getAttr(balise, "day").toInt());
	setMinute(getAttr(balise, "minute").toInt());
	setSecond(getAttr(balise, "second").toInt());
	setMonth(getAttr(balise, "month").toInt());
	setYear(getAttr(balise, "year").toInt());
	kDebug(30522) << "**** END KEY ****" << endl;
}

/*******************************************/
/* Generate                                */
/*******************************************/
/* Generate each text zone with the parag. */
/* markup.                                 */
/*******************************************/
void Key::generate(QTextStream &out)
{

	kDebug(30522) << "  GENERATION KEY" << endl;

	kDebug(30522) << "PARA KEY" << endl;
}
