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

#ifndef __KWORD_LATEX_IMPORT_PARAGRAPH_H__
#define __KWORD_LATEX_IMPORT_PARAGRAPH_H__

#include <qptrlist.h>

class Element;
class QDomDocument;
class QDomElement;
//class TextZone;

/**
 * The body is a frameset. It contains the body of a document.
 *
 */
class Paragraph
{
	public:
		Paragraph()
		{
		}
		
		~Paragraph() { }

		/**
		 * Analyse the latex tree and keep data.
		 * 
		 * @param elt The root tree which contaisn an entire latex document.
		 * 
		 * @return <code>false/code> if the analyse failed.
		 */
		bool analyse(QPtrList<Element>* elt);

		/**
		 * Generate a kword document.
		 *
		 * @param store The kword document zip file.
		 *
		 * @return <code>true</code> if the generation success.
		 */
		bool generate(QDomElement& paragraph, QDomDocument& doc);

	private:
		/** A paragraph is a list of text zones. */
		//QPtrList<TextZone> _textzones;
};

#endif /* __KWORD_LATEX_IMPORT_PARAGRAPH_H__ */
