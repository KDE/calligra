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

#ifndef __LATEXPARSER_COMMENT_H__
#define __LATEXPARSER_COMMENT_H__

#include "element.h"

#include <qstring.h>

class Comment: public Element
{
	public:
		Comment();

		Comment(const char* comment);
		~Comment();

		QString getComment() const { return _comment; }

		void setComment(QString comment) { _comment = comment; }

		void print(int tab = 0);

	private:
		QString _comment;

};

#endif /* __LATEXPARSER_COMMENT_H__ */
