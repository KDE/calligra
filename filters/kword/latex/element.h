
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

/* FRAMESET */
enum SType
{
	ST_NONE,
	ST_TEXT,
	ST_PICTURE,
	ST_PART,		/* This last Type mustn't    */
	ST_FORMULA		/* be display where they     */
};

enum SSect
{
	SS_NONE,
	SS_HEADERS,
	SS_FOOTERS,
	SS_BODY,
	SS_FOOTNOTES,
	SS_TABLE
};

enum SInfo
{
	SI_NONE,
	SI_FIRST,
	SI_ODD,
	SI_EVEN
};

/* FRAME */
enum TAround
{
	TA_NONE,
	TA_FRAME,
	TA_TEXT
};

enum TCreate
{
	TC_EXTEND,
	TC_CREATE,
	TC_IGNORE
};

enum TNFrame
{
	TF_RECONNECT,
	TF_NOCREATION,
	TF_COPY
};

enum TSide
{
	TS_ANYSIDE,
	TS_ODDPAGE,
	TS_EVENPAGE
};

class Element: public XmlParser
{
	/* FRAMESET PARAM */
	SType   _type;
	SSect   _section;
	SInfo   _hinfo;
	char*   _name;
	bool    _removable;
	bool    _visible;
	QString _grpMgr;
	int     _row, _col, _rows, _cols;

	Element* _suivant;

	public:
		Element();

		virtual ~Element();

		/*virtual bool  hasColor() const = 0;
		virtual bool  hasUline() const = 0;*/
		
		SSect    getSection () const { return _section;        }
		SType    getType    () const { return _type;           }
		SInfo    getInfo    () const { return _hinfo;          }
		Element* getNext    () const { return _suivant;        }
		bool     isVisible  () const { return _visible;        }
		bool     isRemovable() const { return _removable;      }
		QString  getGrpMgr  () const { return _grpMgr;         }
		int      getRow     () const { return _row;            }
		int      getCol     () const { return _col;            }
		int      getRows    () const { return _rows;           }
		int      getCols    () const { return _cols;           }
		bool     isTable    () const { return (_section == SS_TABLE); }

		void setType(SType t)       { _type      = t;   }
		void setSection(SSect s)    { _section   = s;   }
		void setNext(Element *elt)  { _suivant   = elt; }
		void setVisible(bool v)     { _visible   = v;   }
		void setRemovable(bool r)   { _removable = r;   }
		void setGrpMgr(const char*g){ _grpMgr    = g;   }
		void setRow   (int r)       { _row       = r;   }
		void setCol   (int c)       { _col       = c;   }
		void setRows  (int r)       { _rows      = r;   }
		void setCols  (int c)       { _cols      = c;   }

		virtual void analyse(const Markup*);
		virtual void generate(QTextStream&) = 0;

	private:
		void analyseParam(const Markup *);
};

#endif /* __KWORD_ELEMENT_H__ */
