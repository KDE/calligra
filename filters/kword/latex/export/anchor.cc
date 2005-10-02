
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

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kdDebug() stream */
#include "anchor.h"
#include "element.h"
#include "document.h"

Anchor::Anchor(Para* para): Format(para)
{
}

Anchor::~Anchor()
{
	kdDebug(30522) << "Destruction of an anchor." << endl;
}

void Anchor::analyse(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT> */
	Format::analyse(balise);

	/* Parameters Analyse */
	kdDebug(30522) << "ANALYSE AN ANCHOR" << endl;
	
	/* Children Markups Analyse */
	_type = getAttr(getChild(balise, "ANCHOR"), "type");
	_instance = getAttr(getChild(balise, "ANCHOR"), "instance");
	kdDebug(30522) << "type = " << _type << " instance = " << _instance << endl;
	
	kdDebug(30522) << "END OF AN ANCHOR" << endl;
}

void Anchor::generate(QTextStream &out)
{
	Element *elt = 0;

	kdDebug(30522) << "  GENERATION ANCHOR" << endl;
	/* research the element in all the special elements list
	 * and display it
	 */
	kdDebug(30522) << "anchor : " << _instance << endl;
	if((elt = getRoot()->searchAnchor(_instance)) != 0)
		elt->generate(out);
	kdDebug(30522) << "ANCHOR GENERATED" << endl;
}


