
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

#ifndef kword_latextextzone
#define kword_latextextzone

#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include "xmlparser.h"
#include "format.h"

class TextZone: public XmlParser, public  Format
{
	/*SType _type;
	SSect _section;*/
	QString _texte;	

	public:
		TextZone(const char* c = 0);
		//SSect getSection() { return _section; }
		//SType getType() { return _type; }
		void analyse(const Markup*);
		void generate(QTextStream&);
	private:
		void analyse_param  (const Markup *);
		void analyse_font   (const Markup *);
		void analyse_italic (const Markup *);
		void analyse_weigth (const Markup *);
};

#endif
