
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

#ifndef kword_latexformat
#define kword_latexformat

#include <qstring.h>

class Format
{
	QString  _police;
	int      _id;
	int      _pos;
	int      _taille;
	int      _weight;
	bool     _italic;

	public:
		Format(): _id(0), _pos(0), _taille(0), _weight(0), _italic(false) {}

		bool is_italic () { return _italic; }
		
		void set_id     (const int id)  { _id  = id;   }
		void set_pos    (const int pos) { _pos = pos;  }
		void set_taille (const int t)   { _taille = t; }
		void set_weight (const int w)   { _weight = w; }
		void set_italic (const bool i)  { _italic = i; }
		void set_police (const char *p) { _police = p; }

		int  get_pos    () const { return _pos;    }
		int  get_length () const { return _taille; }
		int  get_weight () const { return _weight; }
		bool is_italic  () const { return _italic; }
};

#endif
