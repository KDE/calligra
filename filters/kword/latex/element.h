
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

#ifndef __KWORD_ELEMENT_H__
#define __KWORD_ELEMENT_H__

#include <qtextstream.h>
#include "xmlparser.h"

enum SType
{
	ST_NONE,
	ST_TEXT,
	ST_PICTURE,
	ST_PART,
	ST_FORMULA
};

enum SSect
{
	SS_NONE,
	SS_HEADERS,
	SS_FOOTERS,
	SS_BODY,
	SS_FOOTNOTES
};

enum SInfo
{
	SI_NONE,
	SI_FIRST,
	SI_ODD,
	SI_EVEN
};

class Element: public XmlParser
{
	SType _type;
	SSect _section;
	SInfo _hinfo;
	char* _name;
	bool  _removable;
	bool  _visible;

	Element* _suivant;

	public:
		Element();

		virtual ~Element();

		/*virtual bool  hasColor() const = 0;
		virtual bool  hasUline() const = 0;*/
		
		SSect    getSection()  const { return _section;   }
		SType    getType()     const { return _type;      }
		SInfo    getInfo()     const { return _hinfo;     }
		Element* getNext()     const { return _suivant;   }
		bool     isVisible()   const { return _visible;   }
		bool     isRemovable() const { return _removable; }
		
		void setType(SType t)       { _type      = t;   }
		void setSection(SSect s)    { _section   = s;   }
		void setNext(Element *elt)  { _suivant   = elt; }
		void setVisible(bool v)     { _visible   = v;   }
		void setRemovable(bool r)   { _removable = r;   }

		virtual void analyse(const Markup*);
		virtual void generate(QTextStream&) = 0;

	private:
		void analyseParam(const Markup *);
};

#endif /* __KWORD_ELEMENT_H__ */
