
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

#ifndef __KWORD_HEADER_H__
#define __KWORD_HEADER_H__

#include <qtextstream.h>
#include "xmlparser.h"

enum _TFormat
{
	TF_A3,
	TF_A4,
	TF_A5,
	TF_USLETTER,
	TF_USLEGAL,
	TF_SCREEN,
	TF_CUSTOM,
	TF_B3,
	TF_USEXECUTIVE
};

enum _TUnit
{
	TU_MM,
	TU_CM,
	TU_PT,
	TU_INCH
};

enum _TOrient
{
	TO_PORTRAIT,
	TO_LANDSCAPE
};

enum _TColonne
{
	TC_1,
	TC_2,
	TC_MORE
};

typedef enum _TFormat TFormat;
typedef enum _TUnit TUnit;
typedef enum _TOrient TOrient;
typedef enum _TColonne TColonne;

class Header: public XmlParser
{
	// PAPER
	TFormat  _format;
	TOrient  _orientation;
	TColonne _colonne;
	double   _largeur,
		 _hauteur;
	double   _margeGauche, 
		 _margeDroite,
		 _margeHaut,
		 _margeBas;
	
	// ATTRIBUTES
	TUnit    _unite;
	bool     _hasHeader;
	bool     _hasFooter;

	// FOOTNOTEMGR

	public:
		Header();

		virtual ~Header();

		TFormat  getFormat     ()       { return _format;      }
		TOrient  getOrientation()       { return _orientation; }
		TColonne getColumns    ()       { return _colonne;     }
		bool  hasHeader        () const { return _hasHeader; }
		bool  hasFooter        () const { return _hasFooter; }

		void setFormat     (TFormat f)  { _format      = f;            }
		void setFormat     (int f)      { _format      = (TFormat) f;  }
		void setOrientation(TOrient o)  { _orientation = o;            }
		void setOrientation(int o)      { _orientation = (TOrient) o;  }
		void setColumns    (TColonne c) { _colonne     = c;            }
		void setColumns    (int c)      { _colonne     = (TColonne) c; }
		void setUnit       (int u)      { _unite       = (TUnit) u;    }

		void analysePaper    (const Markup *);
		void analyseAttributs(const Markup *);

		void generate(QTextStream &);

	private:
};

#endif /* __KWORD_HEADER_H__ */
