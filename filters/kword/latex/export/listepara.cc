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
#include "listepara.h"

ListPara::ListPara()
{
	kdDebug(30522) << "Create liste para empty" << endl;
	_start  = 0;
	_end    = 0;
	_size   = 0;
}

ListPara::~ListPara()
{
	kdDebug(30522) << "Destruction of a list of parag" << endl;
	vider();
	kdDebug(30522) << "ok" << endl;
}

void ListPara::initialiser(Para *elt)
{
	kdDebug(30522) << "initialise a list of parag at " << elt << endl;
	_end = _start = elt;
}

void ListPara::add(Para *elt)
{
	if(_start == 0)
	{
		initialiser(elt);
		_size = 1;
	}
	else
	{
		kdDebug(30522) << "add a parag." << endl;
		_end->setNext(elt);
		elt->setPrevious(_end);
		_end  = elt;
		_size = _size + 1;
	}
}

void ListPara::rem()
{
	Para *first_saved = 0;

	first_saved = _start;
	_start      = _start->getNext();
	delete first_saved;
	_size  = _size - 1;
}

void ListPara::vider()
{
	while(_start != 0)
	{
		rem();
	}
}

