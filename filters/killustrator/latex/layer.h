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

#ifndef __KILLU_LAYER_H__
#define __KILLU_LAYER_H__

#include <qlist.h>
#include <qtextstream.h>
#include "xmlparser.h"
#include "element.h"

/***********************************************************************/
/* Class: Layer                                                        */
/***********************************************************************/

/**
 * This class hold a whole document with its rectangles, layers, ...
 * It can generate a latex file (with pstricks extension).
 */
class Layer: public XmlParser
{
	QList<Element> _set;	

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Document.
		 */
		Layer();

		/**
		 * Destructor
		 *
		 * Remove the list of headers, footers and the body.
		 */
		virtual ~Layer();

		/**
		 * Accessors
		 */
		void  analyse(const QDomNode);

		void  generatePSTRICKS(QTextStream&);

	private:
};

#endif /* __KILLU_LAYER_H__ */
