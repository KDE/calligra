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
#ifndef __KWORD_ELEMENTT_H__
#define __KWORD_ELEMENTT_H__

#include "textzone.h"

class ElementT
{
	TextZone* _text;
	ElementT* _next;

	private:

	protected:
		
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	ElementT() { 
		_text = 0;
		_next = 0;
	}
	/// Recopy Constructor
	ElementT(ElementT * eltt) {
		_text = eltt->getText();
		_next = eltt->getNext();
	}
//@}

/**
 * @name Destructors
 */
//@{
	  /// Destructor
	virtual ~ElementT();
//@}

/**
 * @name Accessors
 */
//@{
	TextZone* getText () const { return _text;  }
	ElementT* getNext () const { return _next;  }
//@}

/**
 * @name Modifiors
 */
//@{
	void setText (TextZone*);
	void remText ();
	void setNext (ElementT*);
	void remNext ();
//@}

/**
 * @name Operateurs
 */
//@{
	/// Operateur d'affectation
	ElementT& operator = (const ElementT &);
 //@}
/**
 * @name Fonctions de delegation
 */
//@{

//@}
};

#endif /* __KWORD_ELEMENTT_H__ */

