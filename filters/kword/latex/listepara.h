
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

#ifndef kword_latexpara
#define kword_latexpara

#include "xmlparser.h"
#include "listetextzone.h"

class Para: public XmlParser
{
	char *_texte;
	ListeTextZone *_liste;
	Para *_suivant;

	public:
		Para();
		
		void setNext(Para *p) { _suivant = p;    }
		
		Para* getNext() const { return _suivant; }
		
		void analyse  (const Markup*);
		void generate (QTextStream&);

	private:
		void analyse_param   (const Markup *);
		void analyse_formats (const Markup *);
};

#endif

#ifndef kword_listepara
#define kword_listepara

class ListPara: public Para
{
	Para *_start, *_end;
	int _size;

	public:
		ListPara();
		void initialiser(Para*);
		void add(Para*);
		
		Para* getFirst() const { return _start; }
		Para* getLast()  const { return _end;   }
		int   getSize()  const { return _size;  }

	private:
};

class ParaIter {
	Para *_courant;
	
	protected:
	
	public:
/**
 * @name Constructeurs
 */
//@{
	/// Constructeur par défaut
	ParaIter()            { _courant = 0;             }
	/// Constructeur
	ParaIter(ListPara l)  { _courant = l.getFirst();  }
	/// Constructeur
	ParaIter(ListPara *l) { _courant = l->getFirst(); }
//@}

/**
 * @name Destructeurs
 */
//@{
	  /// Destructeur
	virtual ~ParaIter() { }
//@}

/**
 * @name Accesseurs
 */
//@{	

	Para* get_courant()  const { return _courant; }
	bool  is_terminate() const { return (_courant == 0); }
//@}

/**
 * @name Modifieurs
 */
//@{
	void next() { _courant = _courant->getNext(); }
//@}

/**
 * @name Operateurs
 */
//@{
 //@}
/**
 * @name Fonctions de delegation
 */
//@{

//@}

};

#endif
