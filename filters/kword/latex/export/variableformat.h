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

#ifndef __KWORD_VARIABLEFORMAT_H__
#define __KWORD_VARIABLEFORMAT_H__

#include <QString>
#include <QColor>
#include "textzone.h"

enum _EVarType
{
	VAR_DATE,
	VAR_UNUSED,
	VAR_TIME,
	VAR_UNUSED2,
	VAR_PAGE,
	VAR_UNUSED3,
	VAR_CUSTOM,
	VAR_MAILMERGE,
	VAR_FIELD,
	VAR_LINK,
	VAR_NOTE,
	VAR_FOOTNOTE
};

typedef enum _EVarType EVarType;

/***********************************************************************/
/* Class: VariableFormat                                               */
/***********************************************************************/

/**
 * This class holds information formatting the textzone. It may be incorporated
 * in few time in textzone.
 */
class VariableFormat: public TextZone
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

	/* FOOTNOTE */
	QString _numberingtype;
	QString _notetype;
	QString _frameset;
	QString _value;

	/* NOTE */
	QString _note;

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of VariableFormat.
		 *
		 */
		VariableFormat(Para* para): TextZone(para)
		{
			setSize(11);
			setWeight(0);
			setItalic(false);
			setUnderlined(UNDERLINE_NONE);
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

		/* ==== Getters ==== */
		QString      getKey       () const { return _key;     }
		QString      getText      () const { return _text;    }
		EVarType     getType      () const { return _varType; }
		int          getDay       () const { return _day;     }
		int          getMonth     () const { return _month;   }
		int          getYear      () const { return _year;    }
		int          getHour      () const { return _hour;    }
		int          getMinute    () const { return _minute;  }
		int          getSeconde   () const { return _seconde; }
		QString      getNumberingtype() const { return _numberingtype; }
		QString      getNotetype  () const { return _notetype; }
		QString      getFrameset  () const { return _frameset; }
		QString      getValue     () const { return _value;    }
		QString      getNote      () const { return _note;     }
		
		bool         isFix        () const { return (_fix       == true); }

		/* ==== Setters ==== */
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
    void setNumberingtype(const QString nt) { _numberingtype = nt; }
		void setNotetype  (const QString nt)    { _notetype = nt; }
		void setFrameset  (const QString fs)    { _frameset = fs; }
		void setValue     (const QString val)   { _value = val;   }
		void setNote      (const QString note)  { _note  = note;  }

		/* ==== Helpful functions ==== */
		void analyzeFormat(const QDomNode);
		void analyzeDate      (const QDomNode);
		void analyzeTime      (const QDomNode);
		void analyzeFootnote  (const QDomNode);
		void analyzeNote      (const QDomNode);
		void analyzeType      (const QDomNode);
};

#endif /* __KWORD_VARIABLEFORMAT_H__ */
