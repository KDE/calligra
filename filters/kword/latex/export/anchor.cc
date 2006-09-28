
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
#include <kdebug.h>		/* for kDebug() stream */
#include "anchor.h"
#include "element.h"
#include "document.h"
//Added by qt3to4:
#include <QTextStream>

Anchor::Anchor(Para* para): Format(para)
{
}

Anchor::~Anchor()
{
	kDebug(30522) << "Destruction of an anchor." << endl;
}

void Anchor::analyze(const QDomNode node)
{
	/* Markup <FORMAT id="1" pos="0" len="17">...</FORMAT> */
	Format::analyze(node);

	/* Parameter analysis */
	kDebug(30522) << "ANALYZE AN ANCHOR" << endl;
	
	/* Child markup analysis */
	_type = getAttr(getChild(node, "ANCHOR"), "type");
	_instance = getAttr(getChild(node, "ANCHOR"), "instance");
	kDebug(30522) << "type = " << _type << " instance = " << _instance << endl;
	
	kDebug(30522) << "END OF AN ANCHOR" << endl;
}

void Anchor::generate(QTextStream &out)
{
	Element *elt = 0;

	kDebug(30522) << "  GENERATION ANCHOR" << endl;
	/* search for the element in all the special element lists
	 * and display it
	 */
	kDebug(30522) << "anchor : " << _instance << endl;
	if((elt = getRoot()->searchAnchor(_instance)) != 0)
		elt->generate(out);
	kDebug(30522) << "ANCHOR GENERATED" << endl;
}


