/* A TEXT ZONE IS SOME WORDS WITH A SPECIAL STYLE (ITALIC, ...).
 * IT'S NOT USE FOR TITLE (BUT IN THE FUTURE IT WILL BE USED FOR)
 * OR FOR SPECIAL PARAG.
 */
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

#ifndef __KWORD_VARIABLEZONE_H__
#define __KWORD_VARIABLEZONE_H__

#include <qtextstream.h>
#include <qstring.h>
#include "variableformat.h"

/***********************************************************************/
/* Class: VariableZone                                                     */
/***********************************************************************/

/**
 * This class hold a zone of text with only one formating.
 */
class VariableZone: public VariableFormat
{
	/* TEXT MARKUP */
	QString _texte;

	public:
		/**
		 * Constructors
		 *
		 */

		/**
		 *  Creates a new instance of VariableZone.
		 *
		 * @param Para which is the parent class.
		 */
		VariableZone(Para *para = 0);
		/**
		 *
		 * Creates a new instance of TextZOne.
		 *
		 * @param QStrign the text in this zone.
		 * @param Para which is the parent class.
		 */
		VariableZone(QString, Para *para = 0);

		/* 
		 * Destructor
		 *
		 */
		virtual ~VariableZone();

		/**
		 * Accessors
		 */

		/**
		 * @return Specify if the text must be formated.
		 *
		 */
		bool    useFormat() const;
		QString getTexte()  const { return _texte; }

		/**
		 * Modifiers
		 */

		void setTexte(QString texte) { _texte = texte; }

		/**
		 * Helpfull functions
		 */
		/**
		 * convert a unicode text in latin1 enconding ala latex.
		 */
		QString escapeLatin1(QString);
		
		void analyse(const QDomNode);
		void analyse();
		void generate(QTextStream&, int = 0);
		void generate(QTextStream& out) { generate(out, 0); };

		void generate_format_begin(QTextStream &);
		void generate_format_end(QTextStream &);

	private:
		/**
		 * convert a special character in a markup latex.
		 */
		void convert(QString&, int, const char*);
		void display(QString, QTextStream&, int = 0);
};


//QString convertSpecialChar(int);

#endif /* __KWORD_VARIABLEZONE_H__ */
