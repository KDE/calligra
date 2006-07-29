/* A TEXT ZONE IS SOME WORDS WITH A SPECIAL STYLE (ITALIC, ...).
 * IT'S NOT USE FOR TITLE (BUT IN THE FUTURE IT WILL BE USED FOR)
 * OR FOR SPECIAL PARAG.
 */
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000, 2002 Robert JACOLIN
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

#ifndef __KWORD_LATEX_TEXTZONE_H__
#define __KWORD_LATEX_TEXTZONE_H__

#include <qtextstream.h>
#include <qstring.h>
#include "textformat.h"

/***********************************************************************/
/* Class: TextZone                                                     */
/***********************************************************************/

/**
 * This class hold a zone of text with only one formating.
 */
class TextZone: public TextFormat
{
	/* TEXT MARKUP */
	QString _texte;

	public:
		/* === Constructors ==== */

		/**
		 *  Creates a new instance of TextZone.
		 *
		 * @param para which is the parent class.
		 */
		TextZone(Para *para = 0);
		/**
		 *
		 * Creates a new instance of TextZOne.
		 *
		 * @param text the text in this zone.
		 * @param para which is the parent class.
		 */
		TextZone(QString text, Para *para = 0);

		/* 
		 * Destructor
		 *
		 */
		virtual ~TextZone();

		/* ==== Getters ==== */
		/**
		 * @return Specify if the text must be formated.
		 *
		 */
		bool    useFormat() const;
		QString getTexte()  const { return _texte; }

		/* ==== Setters ==== */
		
		void setTexte(QString texte) { _texte = texte; }

		/* ==== Helpfull functions ==== */

		/**
		 * convert a unicode text in latin1 enconding ala latex.
		 */
		QString escapeLatin1(QString);
		
		void analyse(const QDomNode);
		void analyse();
		virtual void generate(QTextStream&);

		void generate_format_begin(QTextStream &);
		void generate_format_end(QTextStream &);

	protected:
		void display(QString, QTextStream&);

	private:
		/**
		 * convert a special character in a latex command.
		 */
		void convert(QString&, int, const char*);
};


QString convertSpecialChar(int);

#endif /* __KWORD_LATEX_TEXTZONE_H__ */
