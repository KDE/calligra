
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


#ifndef __KWORD_LISTETEXTZONE_H__
#define __KWORD_LISTETEXTZONE_H__

//#include "textzone.h"
#include "elementt.h"

class TextZone;

class ListeTextZone
{
	QString _texte;

	private:
		ElementT* _first;
		ElementT* _last;
		int       _size;

	protected:
		
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	ListeTextZone();
//@}

/**
 * @name Destructors
 */
//@{
	virtual ~ListeTextZone();
//@}

/**
 * @name Accesseurs
 */
//@{
	TextZone* getFirst   () const { return _first->getText(); }
	ElementT* getFirstElt() const { return _first;            }
	TextZone* getLast    () const { return _last->getText();  }
	bool      isVide     () const { return (_size == 0);      }
	int       getSize    () const { return _size;             }
//@}

/**
 * @name Modifieurs
 */
//@{
	void addLast (TextZone*);
	void addFirst(TextZone*);
	void remLast ();
	void remFirst();
	
//@}

/**
 * @name Operators
 */
//@{
 //@}
/**
 * ^name Fonctions de delegation
 */
//@{
	void vider();         /* Empty the list */

//@}
};

class TextZoneIter {
	ElementT *_courant;
	
	protected:
	
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	TextZoneIter()                 { _courant = 0;                }
	/// Constructor
	TextZoneIter(ListeTextZone& l) { _courant = l.getFirstElt();  }
	/// Constructor
	TextZoneIter(ListeTextZone*);
//@}

/**
 * @name Destructors
 */
//@{
	  /// Destructor
	virtual ~TextZoneIter() { }
//@}

/**
 * @name Accessors
 */
//@{	

	TextZone* getCourant()  const { return _courant->getText(); }
	bool      isTerminate() const { return (_courant == 0);     }
//@}

/**
 * @name Modifiors
 */
//@{
	void next   ()                { _courant = _courant->getNext(); }
	void setList(ListeTextZone*);
//@}

/**
 * @name Operators
 */
//@{
 //@}
/**
 * @name Fonctions de delegation
 */
//@{

//@}

};

#endif /* __KWORD_LISTETEXTZONE_H__ */

