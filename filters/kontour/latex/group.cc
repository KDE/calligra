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

#include <stdlib.h>		/* for atoi function    */

#include <kdebug.h>

#include "group.h"
#include "rectangle.h"
#include "ellipse.h"
#include "bezier.h"
#include "polyline.h"
#include "polygone.h"
#include "text.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Group::Group()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Group::~Group()
{
	kdDebug() << "Group Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Group::analyse(const QDomNode balise)
{

	/* Get parameters */
	kdDebug() << "BEGIN OF ANALYSE OF A GROUP" << endl;

	Element::analyse(balise);
	for(int index= 0; index < getNbChild(balise); index++)
	{
		Element *elt = 0;
		kdDebug() << getChildName(balise, index) << endl;
		if(getChildName(balise, index).compare("rectangle")== 0)
		{
			elt = new Rectangle;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
		else if(getChildName(balise, index).compare("polyline")== 0)
		{
			elt = new Polyline;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
		else if(getChildName(balise, index).compare("polygon")== 0)
		{
			elt = new Polygone;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
		else if( getChildName(balise, index).compare("ellipse")== 0)
		{
			elt = new Ellipse;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
		else if(getChildName(balise, index).compare("text")== 0)
		{
			elt = new Text;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
		else if(getChildName(balise, index).compare("bezier")== 0)
		{
			elt = new Bezier;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
		else if(getChildName(balise, index).compare("group")== 0)
		{
			elt = new Group;
			elt->analyse(getChild(balise, index));
			_set.append(elt);
		}
	}

	kdDebug() << "END OF ANALYSE OF A GROUP" << endl;
}

/*******************************************/
/* Generate                                */
/*******************************************/
void  Group::generatePSTRICKS(QTextStream& out)
{

}
