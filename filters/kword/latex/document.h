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

#ifndef __KWORD_DOCUMENT_H__
#define __KWORD_DOCUMENT_H__

#include <qlist.h>
#include <qtextstream.h>

//#include "fileheader.h"		/* class header file.      */
#include "listtable.h"		/* list of tables (another kind of list of elements). */

enum EGenerate
{
	E_LATEX,
	E_KWORD,
	E_CONFIG
};

/***********************************************************************/
/* Class: Document                                                     */
/***********************************************************************/

/**
 * This class hold a whole document with its headers, footers, footnotes, endnotes,
 * content, ... It can generate a latex file.
 */
class Document: public XmlParser
{
	QList<Element> _headers;
	QList<Element> _footers;
	QList<Element> _footnotes;
	QList<Element> _formulas;
	QList<Element> _corps;

	ListTable      _tables;
	//QList<Element> _parts;
	EGenerate     _generation;

	public:
		/**
		 * Constructor
		 *
		 * Creates a new instance of Document.
		 */
		Document();

		/**
		 * Destructor
		 *
		 * Remove the list of headers, footers and the body.
		 */
		virtual ~Document();

		/**
		 * Accessors
		 */

		/**
		 * @return the next frame type (header, footer, body, footnote).
		 */
		SType getTypeFrameset(const QDomNode);
		//FileHeader* getFileHeader() const { return _fileHeader; }
		//void setFileHeader(FileHeader *h) { _fileHeader = h; }

		void  analyse(const QDomNode);

		void  generate(QTextStream&, bool);

	private:
		/**
		 * Generate the second part of the preambule
		 */
		void generatePreambule(QTextStream&);

		/**
		 * Generate the header
		 */
		void  generateTypeHeader(QTextStream&, Element*);
		/**
		 * Generate the footer
		 */
		void  generateTypeFooter(QTextStream&, Element*);
};

#endif /* __KWORD_DOCUMENT_H__ */
