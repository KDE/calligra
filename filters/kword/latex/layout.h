
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

#ifndef __KWORD_LAYOUT_H__
#define __KWORD_LAYOUT_H__

#include <qtextstream.h>
#include <qstring.h>
#include "format.h"

enum _EType
{
	TL_STANDARD,
	TL_ARABIC,		/* 1, 2, ... */
	TL_LLETTER,	/* a, b, ... */
	TL_CLETTER,	/* A, B, ... */
	TL_LLNUMBER,	/* i, ii, ... */
	TL_CLNUMBER,	/* I, II, ... */
	TL_BULLET		/* - */
};

enum _EEnv
{
	ENV_LEFT,
	ENV_RIGHT,
	ENV_CENTER,
	ENV_NONE	/* Parag. always justify in Latex */
};

typedef enum _EType EType;
typedef enum _EEnv EEnv;

class Layout: public Format
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

	public:
		Layout();

		virtual ~Layout() {
			kdDebug() << "Destruction of a layout." << endl;
		}

		bool    isChapter        () const { return (_numberingType == 1); }
		bool    isList           () const { return (_numberingType == 0); }
		bool    isLastSame       () const { return (_name == _last_name); }
		bool    isNextSame       () const { return (_following == _name); }
		//EType getType          () const { return _type;          }
		//EType getTypeFollow    () const { return _follow_type;   }
		QString getLastName      () const { return _last_name;     }
		EType   getLastCounter   () const { return _last_counter;  }
		EType   getCounterType   () const { return _counterType;   }
		int     getCounterDepth  () const { return _counterDepth;  }
		int     getCounterBullet () const { return _counterBullet; }
		int     getCounterStart  () const { return _counterStart;  }
		int     getNumberingType () const { return _numberingType; }
		EEnv    getEnv           () const { return _env;           }


		void setName         (const char * name) { _name = strdup(name); }
		//void setType         (EType type)        { _type = type;         }
		//void setTypeFollow   (EType type)        { _follow_type = type;  }
		void setLastName     ()                  { _last_name   = _name; }
		void setLastCounter  ()                  { _last_counter= _counterType; }
		void setFollowing    (const char * name) { _following = strdup(name); }
		void setCounterType  (int ct) { _counterType   = (EType) ct; }
		void setCounterDepth (int cd) { _counterDepth  = cd; }
		void setCounterBullet(int cb) { _counterBullet = cb; }
		void setCounterStart (int cs) { _counterStart  = cs; }
		void setNumberingType(int nt) { _numberingType = nt; }
		void setEnv          (int env){ _env = (EEnv) env;   }

		void analyseLayout  (const Markup *);

	private:
		void analyseCounter  (const Markup*);
		void analyseFollowing(const Markup*);
		void analyseName     (const Markup*);
		void analyseEnv      (const Markup*);
};

#endif /* __KWORD_LAYOUT_H__ */
