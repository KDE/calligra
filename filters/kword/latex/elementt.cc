/*******************************************************/ 
/* ELEMENTT.CC                                         */
/*******************************************************/
/* Implement methods for ElementT object               */
/* CREATION     : 16/12/2000                           */
/* MODIFICATION :                                      */
/*                                                     */
/*******************************************************/
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

/* INCLUDES */
#include <kdebug.h>
#include "elementt.h"

/* PROVATE METHODS         */

/* PROTECTED METHODS       */

/* PUBLIC METHODS          */

/* Constructors            */

/* Destructors             */
ElementT::~ElementT()
{
	kdDebug() << "Destruction d'un elementT" << endl;
	remText();
}

/* Accessors               */

/* Modifiors               */
void ElementT::setText(TextZone* text)
{
	if(_text == 0)
		_text = new TextZone;
	_text = text;	
}

void ElementT::remText()
{
	delete _text;
	_text = 0;
}

void ElementT::setNext(ElementT* next)
{
	_next = next;
}

void ElementT::remNext()
{
	delete _next;
	_next = 0;
}

/* Operators               */
ElementT& ElementT::operator = (const ElementT & elt)
{
	_text = elt.getText();
	_next = elt.getNext();
	return *this;
}
