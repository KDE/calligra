
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

#ifndef kword_latex_document
#define kword_latex_document

#include "listelement.h"
#include "xmlparse.h"
#include <qfile.h>
#include <qtextstream.h>

enum _EGenerate
{
	LATEX,
	KWORD,
	CONFIG
};

typedef enum _EGenerate EGenerate;

class Document: public XmlParser
{
	ListElement _enTete;
	ListElement _corps;
	EGenerate _generation;

	public:
		Document();

		void  generate(QTextStream&);
		void  analyse(const Markup *);
		SType get_type_frameset(const Markup *);

	private:
};

#endif
