
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

/***********************************************************************/
/* Class: TextFormat                                                   */
/***********************************************************************/

/**
 * This class hold informations formating the textzone. It may be incorporated
 * in few time in textzone.
 */
class TextFormat: public Format
{
	QString      _police;
	unsigned int _pos;
	unsigned int _taille;		/* Length of the string */
	unsigned int _size;		/* Size of the police   */
	unsigned int _weight;		/* bold                 */
	bool         _italic;
	bool         _underline;
	bool         _strikeout;
	EAlign       _vertalign;
	QColor*      _textcolor;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of TextFormat.
		 *
		 */
		TextFormat(): _pos(0), _taille(0), _size(11), _weight(0), _italic(false),
				_underline(false), _strikeout(0)
		{
			_textcolor = 0;
		}

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of little zones.
		 *
		 */
		virtual ~TextFormat() {}

		/**
		 * Accessors
		 */
		unsigned int getPos       () const { return _pos;       }
		unsigned int getLength    () const { return _taille;    }
		unsigned int getSize      () const { return _size;      }
		unsigned int getWeight    () const { return _weight;    }
		EAlign       getAlign     () const { return _vertalign; }
		int          getColorBlue () const;
		int          getColorGreen() const;
		int          getColorRed  () const;

		bool         isItalic     () const { return (_italic    == true); }
		bool         isUnderlined () const { return (_underline == true); }
		bool         isStrikeout  () const { return (_strikeout == true); }
		bool         isColor      () const { return (_textcolor != 0);    }

		/**
		 * Modifiers
		 */
		void setPos        (const unsigned int pos){ _pos       = pos;  }
		void setLength     (const unsigned int t)  { _taille    = t; }
		void setSize       (const unsigned int t)  { _size      = t; }
		void setWeight     (const unsigned int w)  { _weight    = w; }
		void setItalic     (bool i)                { _italic    = i; }
		void setUnderlined (bool u)                { _underline = u; }
		void setStrikeout  (bool s)                { _strikeout = s; }
		void setPolice     (QString p)             { _police    = p; }
		void setAlign      (const int a)           { _vertalign = (EAlign) a; }
		void setColor      (const int, const int, const int);

		/**
		 * Helpfull functions
		 */
		void analyseTextFormat(const QDomNode);
		void analyseParam     (const QDomNode);
		void analyseFont      (const QDomNode);
		void analyseItalic    (const QDomNode);
		void analyseUnderlined(const QDomNode);
		void analyseStrikeout (const QDomNode);
		void analyseWeight    (const QDomNode);
		void analyseAlign     (const QDomNode);
		void analyseColor     (const QDomNode);
		void analyseSize      (const QDomNode);
};

#endif /* __KWORD_TEXTFORMAT_H__ */
