
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
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KILLU_SETELEMENTS_H__
#define __KILLU_SETELEMENTS_H__

#include "element.h"

class SetElements
{
	Element *_start, *_end;
	int _size;

	public:
		SetElements();
		virtual ~SetElements();
		
		void     init(Element*);
		void     add(Element*);
		Element* getFirst() const { return _start; }
		Element* getLast()  const { return _end;   }
		int      getSize()  const { return _size;  }

	private:
};

class ElementIter {
	Element *_current;
	
	protected:
	
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	ElementIter()               { _current = 0;             }
	/// Constructor
	ElementIter(SetElements &l) { _current = l.getFirst();  }
	/// Constructor
	ElementIter(SetElements *l) { 
		if(l != 0)
			_current = l->getFirst();
		else
			_current = 0;
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

	Element* getCurrent()  const { return _current; }
	bool     isTerminate() const { return (_current == 0); }
//@}

/**
 * @name Modifiors
 */
//@{
	void next()                  { _current = _current->getNext(); }
	void setList(SetElements *l) { _current = l->getFirst();       }
	void setList(SetElements l)  { _current = l.getFirst();        }
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
#endif /* __KILLU_SETELEMENTS_H__ */
