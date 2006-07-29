/* A TEXT ZONE IS SOME WORDS WITH A SPECIAL STYLE (ITALIC, ...).
 * IT'S NOT USE FOR TITLE (BUT IN THE FUTURE IT WILL BE USED FOR)
 * OR FOR SPECIAL PARAG.
 */
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2002, 2003 Robert JACOLIN
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

#ifndef __KWORD_VARIABLEZONE_H__
#define __KWORD_VARIABLEZONE_H__

#include <qtextstream.h>
#include <qstring.h>
#include "variableformat.h"

/***********************************************************************/
/* Class: VariableZone                                                 */
/***********************************************************************/

/**
 * This class hold a zone of text with only one formating.
 */
class VariableZone: public VariableFormat
{

	public:
		/* ==== Constructors ==== */

		/**
		 *  Creates a new instance of VariableZone.
		 *
		 * @param Para which is the parent class.
		 */
		VariableZone(Para* para);
		
		/**
		 * Creates a new instance of TextZOne.
		 *
		 * @param text the text in this zone.
		 * @param para which is the parent class.
		 */
		VariableZone(QString text, Para* para);

		/* ==== Destructor ==== */
		virtual ~VariableZone();

		/* ==== Getters ==== */

		/* ==== Setters ==== */

		/* ==== Helpfull functions ==== */

		void analyse(const QDomNode);
		void generate(QTextStream&);

};

#endif /* __KWORD_VARIABLEZONE_H__ */
