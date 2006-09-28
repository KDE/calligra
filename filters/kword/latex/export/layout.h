
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

#ifndef __KWORD_LAYOUT_H__
#define __KWORD_LAYOUT_H__

#include <QTextStream>
#include <QString>
#include <kdebug.h>
#include "textformat.h"

enum _EType
{
	TL_NONE,		/* NONE       */
	TL_ARABIC,		/* 1, 2, ...  */
	TL_LLETTER,		/* a, b, ...  */
	TL_CLETTER,		/* A, B, ...  */
	TL_LLNUMBER,		/* i, ii, ... */
	TL_CLNUMBER,		/* I, II, ... */
	TL_CUSTOM_SIMPLE,	/* */
	TL_CUSTOM_COMPLEX,	/* */
	TL_CIRCLE_BULLET,	/* o */
	TL_SQUARE_BULLET,	/* * */
	TL_DISC_BULLET		/* o */
};

enum _EEnv
{
	ENV_NONE,
	ENV_LEFT,
	ENV_RIGHT,
	ENV_CENTER,
	ENV_JUSTIFY	/* default in latex */
};

typedef enum _EType EType;
typedef enum _EEnv EEnv;

/***********************************************************************/
/* Class: Layout                                                       */
/***********************************************************************/

/**
 * This class tells about a paragraph and its contents. For example,
 * a list, a title and the format for the paragraph.
 */
class Layout: public TextFormat
{
	QString      _name;
	QString      _following;
	EType        _counterType;
	int          _counterDepth;
	int          _counterBullet;
	int          _counterStart;
	int          _numberingType;
	EEnv         _env;
	static QString _last_name;
	static EType   _last_counter;
	bool           _useHardBreakAfter;
	bool           _useHardBreak;
	bool           _keepLinesTogether;
	
	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Layout.
		 */
		Layout();

		/* 
		 * Destructor
		 *
		 * Nothing to do.
		 */
		virtual ~Layout() {
			kDebug(30522) << "Destruction of a layout." << endl;
		}

		/**
		 * Accessors
		 */
		bool    isChapter          () const { return (_numberingType == 1); }
		bool    isList             () const { return (_numberingType == 0); }
		bool    isEnum             () const { return (_counterType != TL_NONE); }
		bool    isLastSame         () const { return (_name == _last_name); }
		bool    isNextSame         () const { return (_following == _name); }
		QString getLastName        () const { return _last_name;     }
		EType   getLastCounter     () const { return _last_counter;  }
		EType   getCounterType     () const { return _counterType;   }
		int     getCounterDepth    () const { return _counterDepth;  }
		int     getCounterBullet   () const { return _counterBullet; }
		int     getCounterStart    () const { return _counterStart;  }
		int     getNumberingType   () const { return _numberingType; }
		EEnv    getEnv             () const { return _env;           }
		bool    isHardBreakAfter   () const { return _useHardBreakAfter; }
		bool    isHardBreak        () const { return _useHardBreak;      }
		bool    isKeppLinesTogether() const { return _keepLinesTogether; }

		/**
		 * Modifiers
		 */
		void setName         (QString name) { _name = name; }
		void setLastName     ()             { _last_name   = _name; }
		void setLastCounter  ()             { _last_counter= _counterType; }
		void setFollowing    (QString name) { _following = name; }
		void setCounterType  (int ct)       { _counterType   = (EType) ct; }
		void setCounterDepth (int cd)       { _counterDepth  = cd; }
		void setCounterBullet(int cb)       { _counterBullet = cb; }
		void setCounterStart (int cs)       { _counterStart  = cs; }
		void setNumberingType(int nt)       { _numberingType = nt; }
		void setEnv          (int env)      { _env = (EEnv) env;   }
		void useHardBreakAfter()            { _useHardBreakAfter = true; }
		void useHardBreak     ()            { _useHardBreak      = true; }
		void keepLinesTogether()            { _keepLinesTogether = true; }

		/**
		 * Helpful functions
		 */

		void analyzeLayout  (const QDomNode);

	private:
		void analyzeCounter  (const QDomNode);
		void analyzeFollowing(const QDomNode);
		void analyzeName     (const QDomNode);
		void analyzeEnv      (const QDomNode);
		void analyzeBreakLine(const QDomNode);
};

#endif /* __KWORD_LAYOUT_H__ */
