
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

#ifndef kword_latex_element
#define kword_latex_element

#include <qfile.h>
#include <qtextstream.h>
#include "xmlparser.h"

enum _SType
{
	ST_AUCUN,
	ST_IMAGE,
	ST_TEXTE,
	ST_PARTS
};

typedef enum _SType SType;

enum _SSect
{
	SS_AUCUN,
	SS_ENTETE,
	SS_PIDS,
	SS_CORPS
};

typedef enum _SSect SSect;

class Element: public XmlParser
{
	SType _type;
	SSect _section;
	char *_name;
	bool _removable;
	bool _visible;

	Element *_suivant;

	public:
		Element();

		virtual ~Element() {}
		SSect getSection() const      { return _section; }
		SType getType()    const      { return _type;    }
		Element* getNext() const      { return _suivant; }
		
		void setNext(Element *elt) { _suivant = elt;  }
		
		void analyse(const Markup*);
		//virtual void generate(QTextStream&) {}

	private:
		void analyse_param(const Markup *);
		//void analyse_param_frame(const Markup *);
};

#endif
