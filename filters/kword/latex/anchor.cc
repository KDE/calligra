
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
#include "anchor.h"
#include "element.h"
#include "document.h"

Anchor::Anchor(Para* para): Format(para)
{
}

Anchor::~Anchor()
{
	kdDebug() << "Destruction of an anchor." << endl;
}

void Anchor::analyse(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameters Analyse */
	kdDebug() << "ANALYSE AN ANCHOR" << endl;
	
	/* Children Markups Analyse */
	_type = getAttr(getChild(balise, "ANCHOR"), "type");
	_instance = getAttr(getChild(balise, "ANCHOR"), "instance");
	kdDebug() << "type = " << _type << " instance = " << _instance << endl;
	
	kdDebug() << "END OF AN ANCHOR" << endl;
}

void Anchor::generate(QTextStream &out)
{
	Element *elt = 0;

	kdDebug() << "  GENERATION ANCHOR" << endl;
	/* research the element in all the special elements list
	 * and display it
	 */
	kdDebug() << "anchor : " << _instance << endl;
	if((elt = getRoot()->searchAnchor(_instance)) != 0)
		elt->generate(out);
	kdDebug() << "ANCHOR GENERATED" << endl;
}


