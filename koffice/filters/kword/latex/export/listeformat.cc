
/* BUGS : latex don't support alpha list with one command !!! the
 * command generated doesn't exist :))))
 */

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

#include <kdebug.h>		/* for kdDebug() stream */
#include "listeformat.h"

/* PRIVATE METHODS         */

/* PROTECTED METHODS       */

/* PUBLIC METHODS          */

/* Constructors            */

/* Destructors             */
FormatElt::~FormatElt()
{
	kdDebug(30522) << "Destruction d'un elementT" << endl;
	remFormat();
}

/* Accessors               */

/* Modifiors               */
void FormatElt::setFormat(Format* format)
{
	_format = format;	
}

void FormatElt::remFormat()
{
	delete _format;
	_format = 0;
}

void FormatElt::setNext(FormatElt* next)
{
	_next = next;
}

void FormatElt::remNext()
{
	delete _next;
	_next = 0;
}

/* Operators               */
FormatElt& FormatElt::operator = (const FormatElt & elt)
{
	_format = elt.getFormat();
	_next   = elt.getNext();
	return *this;
}

//////////////////////////////////////////////////////////
ListeFormat::ListeFormat()
{
	kdDebug(30522) << "Create format list" << endl;
	_first  = 0;
	_end    = 0;
	_size   = 0;
}

ListeFormat::~ListeFormat()
{
	kdDebug(30522) << "Destruction of a list of format" << endl;
	vider();
	kdDebug(30522) << "ok" << endl;
}

void ListeFormat::addLast(Format *elt)
{

	FormatElt *new_last = new FormatElt;

	new_last->setFormat(elt);

	if(_first != 0)
	{
		_end->setNext(new_last);
		_end = new_last;
	}
	else
	{
		/* La liste est vide => _last = _first; */
		_end  = new_last;
		_first = _end;
	}	
	_size = _size + 1;
}

void ListeFormat::addFirst(Format* elt)
{
	FormatElt *new_first = new FormatElt;

	new_first->setFormat(elt);
	new_first->setNext(_first);

	_first = new_first;
	if(_size == 0)
	{
		/* La liste est vide => _last = _first; */
		_end = _first;
	}	
	_size = _size + 1;
}

void ListeFormat::remLast()
{
	FormatElt *new_last = new FormatElt(_first);
	
	for(int index = 1; index< _size - 1; new_last = new_last->getNext())
	{ }
	
	delete _end;
	_end = new_last;
	_size = _size - 1;
}

void ListeFormat::remFirst()
{
	FormatElt *first_saved;

	first_saved = _first->getNext();

	delete _first;
	_first = first_saved;
	_size  = _size - 1;
}

void ListeFormat::vider()
{
	while(_first != 0)
	{
		remFirst();
	}
}

