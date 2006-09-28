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

#include <stdlib.h>

#include <kdebug.h>

#include "fileheader.h"	/* for the use of _header (color and underlined) */
#include "format.h"

/* Get the set of info. about a text format */
void Format::analyze(const QDomNode node)
{
	/* <FORMAT id="1" pos="0" len="17"> */
	setId(getAttr(node, "id").toInt());
	setPos(getAttr(node, "pos").toInt());
	setLength(getAttr(node, "len").toInt());
}

/*void Format::generate(QTextStream&, int)
{
}*/
