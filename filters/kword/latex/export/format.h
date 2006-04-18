
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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KWORD_FORMAT_H__
#define __KWORD_FORMAT_H__

#include <qtextstream.h>
#include <qstring.h>
#include <qcolor.h>
#include "xmlparser.h"

enum _EFormat
{
	EF_ERROR,		/* 0 */
	EF_TEXTZONE,		/* 1 */
	EF_PICTURE,		/* 2 */
	EF_TABULATOR,		/* 3 */
	EF_VARIABLE,		/* 4 */
	EF_FOOTNOTE,		/* 5 */
	EF_ANCHOR		/* 6 */
};

typedef enum _EFormat EFormat;

class Para;

/***********************************************************************/
/* Class: Format                                                       */
/***********************************************************************/

/**
 * This class describe a format or a part of an element (text zone, picture
 * zone, footnote, ...).
 * An element has a lot of format. For example, a Text has a lot of text zone
 * but can have a picture too.
 */
class Format: public XmlParser
{
	EFormat _id;
	unsigned int _pos;
	unsigned int _taille;		/* Length of the string */

	/* USEFULL DATA */
	Para*   _para;		/* Parent class */

	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of Format.
		 *
		 * @param para the parent class of the format.
		 */
		Format(Para *para= 0): _id((EFormat) 0), _para(para)
		{}

		/* 
		 * Destructor
		 *
		 * Nothing to do
		 */
		virtual ~Format() {}

		/**
		 * Accessors
		 */
		EFormat getId        () const { return _id;   }
		Para*   getPara      () const { return _para; }
		EFormat getFormatType() const { return _id;   }
		unsigned int getPos       () const { return _pos;       }
		unsigned int getLength    () const { return _taille;    }

		/**
		 * Modifiers
		 */
		void setId   (int   id)   { _id   = (EFormat) id; }
		void setPara (Para* para) { _para = para;         }
		void setPos        (const unsigned int pos){ _pos       = pos;  }
		void setLength     (const unsigned int t)  { _taille    = t; }

		/**
		 * Helpfull functions
		 */

		/**
		 * Get informations from a markup tree (only param of a format).
		 */
		virtual void analyse(const QDomNode);

		virtual void generate(QTextStream&) = 0;
		//virtual void generate(QTextStream&, int = 0);

};

#endif /* __KWORD_FORMAT_H__ */
