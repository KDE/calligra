
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

#ifndef kword_latex_header
#define kword_latex_header

#include <qtextstream.h>
#include "xmlparse.h"

enum _TFormat
{
	TF_A3,
	TF_A4,
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

typedef enum _TFormat TFormat;
typedef enum _TUnit TUnit;


class Header
{
	// PAPER
	TFormat _format;
	double _largeur, _hauteur;
	double _margeGauche, _margeDroite, _margeHaut, _margeBas;
	
	// ATTRIBUTES
	TUnit _unite;

	// FOOTNOTEMGR

	public:
		Header();
		void generate(QTextStream &);
		void setPaper(Markup *);
		void setAttributs(Markup *);

	private:
};

#endif
