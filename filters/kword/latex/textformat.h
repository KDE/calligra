
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

#ifndef __KWORD_TEXTFORMAT_H__
#define __KWORD_TEXTFORMAT_H__

#include <qstring.h>
#include <qcolor.h>
#include "format.h"

enum _EAlign
{
	EA_NONE,
	EA_SUB,
	EA_SUPER
};

typedef enum _EAlign EAlign;

class TextFormat: public Format
{
	QString  _police;
	int      _pos;
	int      _taille;	/* Length of the string */
	int      _size;		/* Size of the police */
	int      _weight;
	bool     _italic;
	bool     _underline;
	EAlign   _vertalign;
	QColor*  _textcolor;

	public:
		TextFormat(): _pos(0), _taille(0), _size(11), _weight(0), _italic(false), _underline(false)
		{
			_textcolor = 0;
		}
		virtual ~TextFormat() {}

		int    getPos       () const { return _pos;       }
		int    getLength    () const { return _taille;    }
		int    getSize      () const { return _size;      }
		int    getWeight    () const { return _weight;    }
		bool   isItalic     () const { return _italic;    }
		bool   isUnderlined () const { return _underline; }
		EAlign getAlign     () const { return _vertalign; }
		bool   isColor      () const { return (_textcolor!= 0); }
		int    getColorBlue () const;
		int    getColorGreen() const;
		int    getColorRed  () const;

		//void setId         (const int id)  { _id        = id;   }
		void setPos        (const int pos) { _pos       = pos;  }
		void setTaille     (const int t)   { _taille    = t; }
		void setSize       (const int t)   { _size      = t; }
		void setWeight     (const int w)   { _weight    = w; }
		void setItalic     (const bool i)  { _italic    = i; }
		void setUnderlined (const bool u)  { _underline = u; }
		void setPolice     (const char *p) { _police    = p; }
		void setAlign      (const int a)   { _vertalign = (EAlign) a; }
		void setColor (const int, const int, const int);

		void analyseTextFormat(const Markup*);
		void analyseParam     (const Markup*);
		void analyseFont      (const Markup*);
		void analyseItalic    (const Markup*);
		void analyseUnderlined(const Markup*);
		void analyseWeigth    (const Markup*);
		void analyseAlign     (const Markup*);
		void analyseColor     (const Markup*);
};

#endif /* __KWORD_TEXTFORMAT_H__ */
