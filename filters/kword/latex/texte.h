/* A TEXT IS A SET OF TITLES, A (SET OF) PARAGRAPHS OR LISTS */
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

#ifndef kword_latex_texte
#define kword_latex_texte

#include "element.h"
#include "listepara.h"

class Texte: public Element
{
	/* DATA MARKUP */
	int      _left,
		 _top,
		 _right,
		 _bottom;
	bool     _runaround;

	/* CHILD MARKUP */
	ListPara _liste;

	public:
		Texte();
		virtual ~Texte() {
			kdDebug() << "Destruction of a txt" << endl; }
		
		bool  hasColor();
		bool  hasUline();
		
		void analyse(const Markup*);
		void generate(QTextStream&);

	private:
		void analyseParamFrame(const Markup*);

};

#endif
