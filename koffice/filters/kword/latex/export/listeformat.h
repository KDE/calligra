
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


#ifndef __KWORD_LISTEFORMAT_H__
#define __KWORD_LISTEFORMAT_H__

#include "format.h"

class FormatElt
{
	Format*    _format;
	FormatElt* _next;

	private:

	protected:
		
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	FormatElt() { 
		_format = 0;
		_next   = 0;
	}

	/// Recopy Constructor
	FormatElt(FormatElt * eltt) {
		_format = eltt->getFormat();
		_next   = eltt->getNext();
	}
//@}

/**
 * @name Destructors
 */
//@{
	  /// Destructor
	virtual ~FormatElt();
//@}

/**
 * @name Accessors
 */
//@{
	Format*    getFormat () const { return _format;  }
	FormatElt* getNext () const { return _next;    }
//@}

/**
 * @name Modifiors
 */
//@{
	void setFormat (Format*);
	void remFormat ();
	void setNext (FormatElt*);
	void remNext ();
//@}

/**
 * @name Operateurs
 */
//@{
	/// Operateur d'affectation
	FormatElt& operator = (const FormatElt &);
 //@}
/**
 * @name Fonctions de delegation
 */
//@{

//@}
};

class ListeFormat
{
	//QString _texte;

	private:
		FormatElt* _first;
		FormatElt* _end;
		int        _size;

	protected:
		
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	ListeFormat();
//@}

/**
 * @name Destructors
 */
//@{
	virtual ~ListeFormat();
//@}

/**
 * @name Accesseurs
 */
//@{
	Format*    getFirst   () const { return _first->getFormat(); }
	FormatElt* getFirstElt() const { return _first;              }
	Format*    getLast    () const { return _end->getFormat();   }
	bool       isVide     () const { return (_size == 0);        }
	int        getSize    () const { return _size;               }
//@}

/**
 * @name Modifieurs
 */
//@{
	void addLast (Format*);
	void addFirst(Format*);
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

class FormatIter {
	FormatElt *_courant;
	
	protected:
	
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	FormatIter()                 { _courant = 0;                }
	/// Constructor
	FormatIter(ListeFormat& l) { _courant = l.getFirstElt();  }
	/// Constructor
	FormatIter(ListeFormat*);
//@}

/**
 * @name Destructors
 */
//@{
	  /// Destructor
	virtual ~FormatIter() { }
//@}

/**
 * @name Accessors
 */
//@{	

	Format* getCourant()  const { return _courant->getFormat(); }
	bool    isTerminate() const { return (_courant == 0);     }
//@}

/**
 * @name Modifiors
 */
//@{
	void next   ()               { _courant = _courant->getNext(); }
	void setList(ListeFormat* l) { _courant = l->getFirstElt();     }
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

#endif /* __KWORD_LISTEFORMAT_H__ */

