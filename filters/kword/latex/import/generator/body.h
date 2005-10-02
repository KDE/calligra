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

#ifndef __KWORD_LATEX_IMPORT_BODY_H__
#define __KWORD_LATEX_IMPORT_BODY_H__

#include <qptrlist.h>
#include "paragraph.h"

class Element;
class QDomDocument;
class QDomElement;

#define NORMALTEXT 0
#define TEXTFRAME 1

/**
 * The body is a frameset. It contains the body of a document.
 *
 */
class Body
{
	public:
		Body()
		{
			_leftMargin = 28;
			_rightMargin = 28;
			_topMargin = 42;
			_bottomMargin = 42;
		}
		
		~Body() { }

		/**
		 * Analyse the latex tree and keep data.
		 * 
		 * @param elt The root tree which contaisn an entire latex document.
		 * 
		 * @return <code>false</code> if the analyse failed.
		 */
		bool analyse(Element* elt);

		/**
		 * Generate a kword document.
		 *
		 * @return <code>true</code> if the generation success.
		 */
		bool generate(QDomElement& framesets, QDomDocument& doc);

	private:
		/** Left margin in pt. */
		int _leftMargin;
		/** Right margin in pt. */
		int _rightMargin;
		/** Top margin in pt. */
		int _topMargin;
		/** Botoom margin in pt. */
		int _bottomMargin;
		/** The body of a document is a list of paragraphs. */
		QPtrList<Paragraph> _paragraphs;
};

#endif /* __KWORD_LATEX_IMPORT_BODY_H__ */
