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

#ifndef __KILLU_DOCUMENT_H__
#define __KILLU_DOCUMENT_H__

#include <qfile.h>		/* for QFile classe */
#include <qtextstream.h>	/* for QTextStream classe */

#include "header.h"		/* class header file.      */
//#include "layer.h"		/* Layer contains all elements. */
#include "page.h"

enum EGenerate
{
	E_LATEX,
	E_KWORD,
	E_CONFIG
};

enum LatexType
{
	LT_PSTRICKS
};

/***********************************************************************/
/* Class: Document                                                     */
/***********************************************************************/

/**
 * This class hold a whole document with its rectangles, layers, ...
 * It can generate a latex file (with pstricks extension).
 */
class Document: public XmlParser
{
	QFile       _file;
	QTextStream _out;
	QString     _filename;

	/* CHILDREN */
	//Layer  _layer;
	Page   _page;
	Header _header;

	LatexType _latexType;
	int       _version;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Document.
		 */
		Document(QString, QString);

		/**
		 * Destructor
		 *
		 * Remove the list of headers, footers and the body.
		 */
		virtual ~Document();

		/**
		 * Accessors
		 */
		LatexType getLatexType() const { return _latexType; }


		void setLatexType(int lt) { _latexType = (LatexType) lt; }

		void  analyse();
		void  analysePreambule(const QDomNode);
		void  analyseDocument(const QDomNode);

		void  generate(); //QTextStream&);

	private:
};

#endif /* __KILLU_DOCUMENT_H__ */
