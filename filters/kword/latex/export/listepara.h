/* A PARA IS A TITLE, A SET OF WORDS OR A LIST. TO KNOW ITS TYPE,
 * YOU MUST LOOK AT IN THE LAYOUT CLASS.
 */
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

#ifndef __KWORD_LISTPARA_H__
#define __KWORD_LISTPARA_H__

#include "para.h"

class ListPara: public Para
{
	Para *_start, *_end;
	int _size;

	public:
		ListPara();
		virtual ~ListPara();

		void initialiser(Para*);
		void add(Para*);
		void rem();
		
		Para* getFirst() const { return _start; }
		Para* getLast()  const { return _end;   }
		int   getSize()  const { return _size;  }

		void vider();

	private:
};

class ParaIter {
	Para *_courant;
	
	protected:
	
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	ParaIter()            { _courant = 0;             }
	/// Constructor
	ParaIter(ListPara &l) { _courant = l.getFirst();  }
	/// Constructor
	ParaIter(ListPara *l) { 
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
	virtual ~ParaIter() { }
//@}

/**
 * @name Accessors
 */
//@{	

	Para* getCourant()  const { return _courant;        }
	bool  isTerminate() const { return (_courant == 0); }
//@}

/**
 * @name Modifiors
 */
//@{
	void next   ()            { _courant = _courant->getNext(); }
	void setList(const ListPara &l) { _courant = l.getFirst();        }
//@}

/**
 * @name Operators
 */
//@{
 //@}
};

#endif /* __KWORD_LISTPARA_H__ */
