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

#include <kdebug.h>		/* for kdDebug stream */
#include "setelements.h"

SetElements::SetElements()
{
	_start = 0;
	_end   = 0;
	_size  = 0;
}

SetElements::~SetElements()
{
	Element *elt = 0;
	kdDebug() << "Destruction of a set of elements" << endl;
	while(_start != 0)
	{
		elt    = _start;
		_start = _start->getNext();
		delete elt;
		_size = _size - 1;
	}
}

void SetElements::init(Element *elt)
{
	_end  = _start = elt;
}

void SetElements::add(Element *elt)
{
	if(_start == 0)
	{
		init(elt);
		_size = 1;
	}
	else
	{
		_end->setNext(elt);
		_end = elt;
		_size = _size + 1;
	}
}

