
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

#ifndef __KWORD_TEXTZONE_H__
#define __KWORD_TEXTZONE_H__

#include <qtextstream.h>
#include <qstring.h>
#include "format.h"

class TextZone: public  Format
{

	QString _texte;
	bool    _useformat;
	
	public:
		TextZone();
		TextZone(QString);
		virtual ~TextZone();

		void useFormat   () { _useformat = true;  }
		void notUseFormat() { _useformat = false; }

		void analyse(const Markup*);
		void generate(QTextStream&);
		void generate_format_begin(QTextStream &);
		void generate_format_end(QTextStream &);

	private:

};

#endif /* __KWORD_TEXTZONE_H__ */
