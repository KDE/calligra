/* This file is part of the KDE project
 * Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __LATEXPARSER_ELEMENT_H__
#define __LATEXPARSER_ELEMENT_H__

#include <qstring.h>

class Element
{
	public:
		Element();

		virtual ~Element();

		/* Type of commands */
		enum EType
		{
			LATEX_NONE,
			LATEX_ENV,
			LATEX_MATH,
			LATEX_COMMAND
		};

		/* getters */
		EType getType() const { return _type; }
		virtual QString getName() const { return ""; }

		/* setters */
		void setType(EType t) { _type = t; }

		/* useful methods */
		//void addParams(QPtrList<Param> params) { }
		//void addGroups(QPtrList<QPtrList<Element> >* elts) {  }
		virtual void print(int tab = 0) = 0;

	private:
		EType _type;

};

#endif /* __LATEXPARSER_ELEMENT_H__ */

