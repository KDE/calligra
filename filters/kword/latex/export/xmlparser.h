
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000-2002 Robert JACOLIN
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

#ifndef __LATEX_XMLPARSER_H__
#define __LATEX_XMLPARSER_H__

#include "qstring.h"
#include "qdom.h"
#include "config.h"

class FileHeader;
class Document;
class KoStore;

/**
 * This class supply some convenients methods to access to a DOM tree.
 * TODO: implement singleton.
 * TODO: remove config in constructors, static variables.
 * @author robert jacolin
 */
class XmlParser
{
		/** The Koffice app document stored in a XML DOM Tree. */
	QDomDocument _document;
	/** The koffice document (maindoc, picture, ...). */
	static KoStore* _in;
	/** Configuration */
	Config *_config;

	protected:
		/* All the inherit class must be have a link with
		 * the header to specify to use special package
		 */
		//static FileHeader *_fileHeader;
		static Document   *_root;
		/** Latex output file */
		static QString _filename;

	public:
		XmlParser(Config*, QString);
    XmlParser(Config*, QByteArray);	/* deprecated */
		XmlParser(Config*, const KoStore*);
		XmlParser();
		virtual ~XmlParser();

		QString     getFilename     () const { return _filename;            }
		QString     getDocument     () const { return _document.toString(); }
		Document*   getRoot         () const { return _root;                }
		//FileHeader* getFileHeader   () const { return _fileHeader; }
		KoStore*    getStorage      () const { return _in; }
		QString     getChildName(QDomNode, int);
		QDomNode    getChild(QDomNode, QString);
		QDomNode    getChild(QDomNode, QString, int);
		QDomNode    getChild(QDomNode, int);
		QString     getData(QDomNode, int);
		int         getNbChild(QDomNode, QString);
		int         getNbChild(QDomNode);
		QString     getAttr(QDomNode, QString) const;
		bool        isChild(QDomNode, QString);

		//void setFileHeader(FileHeader* h) { _fileHeader = h; }
		void setRoot      (Document*   r) { _root       = r; }

		QDomNode init() { return _document.documentElement(); }

};

#endif /* __LATEX_XMLPARSER_H__ */

