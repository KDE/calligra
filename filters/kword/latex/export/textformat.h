
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

#ifndef __KWORD_TEXTFORMAT_H__
#define __KWORD_TEXTFORMAT_H__

#include <QString>
#include <QColor>
#include "format.h"

enum _EAlign
{
	EA_NONE,
	EA_SUB,
	EA_SUPER
};

typedef enum _EAlign EAlign;

enum _ETypeUnderline
{
	UNDERLINE_NONE,
	UNDERLINE_SIMPLE,
	UNDERLINE_DOUBLE,
	UNDERLINE_WAVE
};

typedef enum _ETypeUnderline ETypeUnderline;

enum _ETypeLinespacing
{
	LINESPACING_NONE,
	LINESPACING_ONEANDHALF,
	LINESPACING_DOUBLE,
	LINESPACING_CUSTOM,
	LINESPACING_ATLEAST,
	LINESPACING_MULTIPLE
};

typedef enum _ETypeLinespacing ETypeLinespacing;

/***********************************************************************/
/* Class: TextFormat                                                   */
/***********************************************************************/

/**
 * This class holds information formating the textzone. It may be incorporated
 * in few time in textzone.
 */
class TextFormat: public Format
{
	QString      _police;
	unsigned int _size;				/* Size of the police   */
	unsigned int _weight;			/* bold                 */
	bool         _italic;
	ETypeUnderline _underline;
	bool         _strikeout;
	EAlign       _vertalign;
	QColor*      _textcolor;
	QColor*      _backcolor;
	ETypeLinespacing _linespacingType;
	int              _spacingValue;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of TextFormat.
		 *
		 */
		TextFormat(): _weight(0), _italic(false),
				_strikeout(0)
		{
			_textcolor = 0;
			_backcolor = 0;
			_size = Config::instance()->getDefaultFontSize();
			setPos(0);
			setLength(0);
			setUnderlined("0");
		}

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of little zones.
		 *
		 */
		virtual ~TextFormat() {}

		/* ==== Getters ==== */
		unsigned int getSize      () const { return _size;      }
		unsigned int getWeight    () const { return _weight;    }
		EAlign       getAlign     () const { return _vertalign; }
		int          getColorBlue () const;
		int          getColorGreen() const;
		int          getColorRed  () const;
		ETypeUnderline getUnderlineType() const { return _underline; }

		int          getBkColorBlue () const;
		int          getBkColorGreen() const;
		int          getBkColorRed  () const;
		
		bool         isItalic     () const { return (_italic    == true); }
		bool         isUnderlined () const { return (_underline != UNDERLINE_NONE); }
		bool         isStrikeout  () const { return (_strikeout == true); }
		bool         isColor      () const { return (_textcolor != 0);    }
		bool         isBkColored  () const { return (_backcolor != 0);    }

		/* ==== Setters ==== */
		void setSize       (const unsigned int t)  { _size      = t; }
		void setWeight     (const unsigned int w)  { _weight    = w; }
		void setItalic     (bool i)                { _italic    = i; }
		void setUnderlined (ETypeUnderline u)      { _underline = u; }
		void setUnderlined (QString u)
		{
			if(u == "double")
				_underline = UNDERLINE_DOUBLE;
			else if(u == "wave")
				_underline = UNDERLINE_WAVE;
			else if (u == "1")
				_underline = UNDERLINE_SIMPLE;
			else
				_underline = UNDERLINE_NONE;
		}
		void setStrikeout  (bool s)                { _strikeout = s; }
		void setPolice     (QString p)             { _police    = p; }
		void setAlign      (const int a)           { _vertalign = (EAlign) a; }
		void setColor      (const int, const int, const int);
		void setBkColor    (const int, const int, const int);

		/* ==== Helpful functions ==== */
		void analyseFormat    (const QDomNode);
		void analyseParam     (const QDomNode);
		void analyseFont      (const QDomNode);
		void analyseItalic    (const QDomNode);
		void analyseUnderlined(const QDomNode);
		void analyseStrikeout (const QDomNode);
		void analyseWeight    (const QDomNode);
		void analyseAlign     (const QDomNode);
		void analyseColor     (const QDomNode);
		void analyseSize      (const QDomNode);
		void analyseBackgroundColor(const QDomNode);
};

#endif /* __KWORD_TEXTFORMAT_H__ */
