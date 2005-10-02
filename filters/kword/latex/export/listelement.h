
/*
** Header file for inclusion with kword_xml2latex.c
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

#ifndef __KWORD_LISTELEMENT_H__
#define __KWORD_LISTELEMENT_H__

#include "element.h"

class ListElement
{
	Element *_start, *_end;
	int _size;

	public:
		ListElement();
		virtual ~ListElement();
		
		void initialiser(Element*);
		void add(Element*);
		Element* getFirst() const { return _start; }
		Element* getLast()  const { return _end;   }
		int      getSize()  const { return _size;  }

	private:
};

class ElementIter {
	Element *_courant;
	
	protected:
	
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	ElementIter()               { _courant = 0;             }
	/// Constructor
	ElementIter(ListElement &l) { _courant = l.getFirst();  }
	/// Constructor
	ElementIter(ListElement *l) { 
		if(l != 0)
			_courant = l->getFirst();
		else
			_courant = 0;
	}
//@}

/**
 * @name Destructors
 */
//@{
	  /// Destructor
	virtual ~ElementIter() { }
//@}

/**
 * @name Accessors
 */
//@{	

	Element* getCourant()  const { return _courant; }
	bool     isTerminate() const { return (_courant == 0); }
//@}

/**
 * @name Modifiors
 */
//@{
	void next()                  { _courant = _courant->getNext(); }
	void setList(ListElement *l) { _courant = l->getFirst();       }
	void setList(ListElement l)  { _courant = l.getFirst();        }
//@}

/**
 * @name Operators
 */
//@{
 //@}
/**
 * @name Functions de delegation
 */
//@{

//@}

};
#endif /* __KWORD_LISTELEMENT_H__ */
