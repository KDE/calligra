
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

#ifndef __KWORD_LISTPARA_H__
#define __KWORD_LISTPARA_H__

#include <qstring.h>
#include "xmlparser.h"
#include "listetextzone.h"
#include "layout.h"

class Para: public Layout
{
	QString        _texte;
	ListeTextZone* _liste;
	Para*          _next;
	Para*          _previous;

	public:
		Para();
		virtual ~Para();

		Para* getNext     () const { return _next;     }
		Para* getPrevious () const { return _previous; }
		
		void setNext    (Para *p) { _next     = p;    }
		void setPrevious(Para *p) { _previous = p;    }


		void analyse         (const Markup*);
		void generate        (QTextStream&);
		void generateDebut   (QTextStream&);
		void generateFin     (QTextStream&);

	private:
		void analyseParam    (const Markup *);
		void analyseFormats  (const Markup *);

		void generateTitle(QTextStream&);
};

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
	void setList(ListPara &l) { _courant = l.getFirst();        }
//@}

/**
 * @name Operators
 */
//@{
 //@}
};

#endif /* __KWORD_LISTPARA_H__ */
