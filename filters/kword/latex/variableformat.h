
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

#ifndef __KWORD_VARIABLEFORMAT_H__
#define __KWORD_VARIABLEFORMAT_H__

#include <qstring.h>
#include <qcolor.h>
#include "textformat.h"

enum _EVarType
{
	VAR_DATE,
	VAR_UNUSED,
	VAR_TIME,
	VAR_UNUSED2,
	VAR_PAGE,
	VAR_UNUSED3,
	VAR_CUSTOM,
	VAR_SERIALLETTER,
	VAR_FIELD
};

typedef enum _EVarType EVarType;

/***********************************************************************/
/* Class: VariableFormat                                                   */
/***********************************************************************/

/**
 * This class hold informations formating the textzone. It may be incorporated
 * in few time in textzone.
 */
class VariableFormat: public TextFormat
{
	/*QString      _police;
	unsigned int _size;*/			/* Size of the police   */
	//unsigned int _weight;		/* bold                 */
	/*bool         _italic;
	bool         _underline;
	bool         _strikeout;
	EAlign       _vertalign;
	QColor*      _textcolor;*/

	/* VARIABLE */
	QString  _key;
	EVarType _varType;
	QString  _text;

	/* DATE */
	int  _day;
	int  _month;
	int  _year;
	bool _fix;

	/* HOUR */
	int _hour;
	int _minute;
	int _seconde;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of VariableFormat.
		 *
		 */
		VariableFormat()
		{
			setSize(11);
			setWeight(0);
			setItalic(false);
			setUnderlined(false);
			setStrikeout(0);
			setPos(0);
			setLength(0);
		}

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of little zones.
		 *
		 */
		virtual ~VariableFormat() {}

		/**
		 * Accessors
		 */
		/*unsigned int getSize      () const { return _size;      }
		unsigned int getWeight    () const { return _weight;    }
		EAlign       getAlign     () const { return _vertalign; }
		int          getColorBlue () const;
		int          getColorGreen() const;
		int          getColorRed  () const;*/
		QString      getKey       () const { return _key;     }
		QString      getText      () const { return _text;    }
		EVarType     getType      () const { return _varType; }
		int          getDay       () const { return _day;     }
		int          getMonth     () const { return _month;   }
		int          getYear      () const { return _year;    }
		int          getHour      () const { return _hour;    }
		int          getMinute    () const { return _minute;  }
		int          getSeconde   () const { return _seconde; }

		
		bool         isFix        () const { return (_fix       == true); }
		/*bool         isItalic     () const { return (_italic    == true); }
		bool         isUnderlined () const { return (_underline == true); }
		bool         isStrikeout  () const { return (_strikeout == true); }
		bool         isColor      () const { return (_textcolor != 0);    }*/

		/**
		 * Modifiers
		 */
		/*void setSize       (const unsigned int t)  { _size      = t; }
		void setWeight     (const unsigned int w)  { _weight    = w; }
		void setItalic     (bool i)                { _italic    = i; }
		void setUnderlined (bool u)                { _underline = u; }
		void setStrikeout  (bool s)                { _strikeout = s; }
		void setPolice     (QString p)             { _police    = p; }
		void setAlign      (const int a)           { _vertalign = (EAlign) a; }*/
		void setType       (const int t)           { _varType   = (EVarType) t; }
		void setKey        (QString k)             { _key       = k; }
		void setText       (QString t)             { _text      = t; }
		void setFix        (bool f)                { _fix       = f; }
		void setDay        (const int d)           { _day       = d; }
		void setMonth      (const int m)           { _year      = m; }
		void setYear       (const int y)           { _month     = y; }
		void setHour       (const int h)           { _hour      = h; }
		void setMinute     (const int m)           { _minute    = m; }
		void setSeconde    (const int s)           { _seconde   = s; }
		void setColor      (const int, const int, const int);

		/**
		 * Helpfull functions
		 */
		void analyseVariableFormat(const QDomNode);
		/*void analyseParam     (const QDomNode);
		void analyseFont      (const QDomNode);
		void analyseItalic    (const QDomNode);
		void analyseUnderlined(const QDomNode);
		void analyseStrikeout (const QDomNode);
		void analyseWeight    (const QDomNode);
		void analyseAlign     (const QDomNode);
		void analyseColor     (const QDomNode);
		void analyseSize      (const QDomNode);*/
		void analyseDate      (const QDomNode);
		void analyseTime      (const QDomNode);
		void analyseType      (const QDomNode);
};

#endif /* __KWORD_VARIABLEFORMAT_H__ */
