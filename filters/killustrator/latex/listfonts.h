
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

#ifndef __KILLU_LISTFONTS_H__
#define __KILLU_LISTFONTS_H__

#include "font.h"

class ListFonts
{
	Font *_start, *_end;
	int _size;

	public:
		ListFonts();
		virtual ~ListFonts();
		
		void     init(Font*);
		void     add(Font*);
		Font*    getFirst() const { return _start; }
		Font*    getLast()  const { return _end;   }
		int      getSize()  const { return _size;  }

	private:
};

class FontIter {
	Font *_current;
	
	protected:
	
	public:
/**
 * @name Constructors
 */
//@{
	/// Default Constructor
	FontIter()               { _current = 0;             }
	/// Constructor
	FontIter(ListFonts &l) { _current = l.getFirst();  }
	/// Constructor
	FontIter(ListFonts *l) { 
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
	virtual ~FontIter() { }
//@}

/**
 * @name Accessors
 */
//@{	

	Font* getCurrent()  const { return _current; }
	bool     isTerminate() const { return (_current == 0); }
//@}

/**
 * @name Modifiors
 */
//@{
	void next()                  { _current = _current->getNext(); }
	void setList(ListFonts *l) { _current = l->getFirst();       }
	void setList(ListFonts l)  { _current = l.getFirst();        }
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
#endif /* __KILLU_LISTFONTS_H__ */
