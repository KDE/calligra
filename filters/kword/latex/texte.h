
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

enum _SEnv
{
	SE_AUCUN
};

typedef enum _SEnv SEnv;

class Texte: public Element
{
	SEnv      _env;
	ListPara _liste;
	int      _left,
		 _top,
		 _right,
		 _bottom;
	bool     _runaround;

	public:
		Texte();
		virtual ~Texte() {
			kdDebug() << "Destruction d'un txt" << endl; }
		
		void analyse(const Markup*);
		void generate(QTextStream&);

	private:
		void analyseParamFrame(const Markup*);

};

#endif
