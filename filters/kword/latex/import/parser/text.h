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

#ifndef __LATEXPARSER_TEXT_H__
#define __LATEXPARSER_TEXT_H__

#include "element.h"

#include <qstring.h>

class Text: public Element
{
	public:
		Text();
		Text(const char * text);

		~Text();

		QString getText() const { return _text; }

		void setText(QString text) { _text = text; }

		void print(int tab = 0);

	private:
		QString _text;

};

#endif /* __LATEXPARSER_TEXT_H__ */
