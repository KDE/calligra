
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

#ifndef __KILLU_XMLPARSER_H__
#define __KILLU_XMLPARSER_H__

#include "qstring.h"
#include "qdom.h"

class Header;

class XmlParser
{
	QString      _filename;
	QDomDocument _document;
	//const char* _charset;
	//int         _index;
	//QDomNode _eltCurrent;

	protected:
		/* All the inherit class must be have a link with 
		 * the header to specify to use special package
		 */
		static Header *_fileHeader;

	public:
		XmlParser(QString);
		XmlParser();
		virtual ~XmlParser();

		QString     getFilename     () const { return _filename;      }
		QString     getDocument     () const { return _document.toString(); };
		Header*     getFileHeader   () const { return _fileHeader; }
		/*QDomNode getChild(QString);
		QDomNode getChild(QString, int);
		QString  getChildName(int);*/
		QString  getChildName(QDomNode, int);
		QDomNode getChild(QDomNode, QString);
		QDomNode getChild(QDomNode, QString, int);
		QDomNode getChild(QDomNode, int);
		int      getNbChild(QDomNode, QString);
		int      getNbChild(QDomNode);
		/*int      getNbChild(QString);
		int      getNbChild();
		QString  getAttr(QString);*/
		QString  getAttr(QDomNode, QString) const;

		void setFileHeader(Header* h) { _fileHeader = h; }
		QDomNode init() { return _document.documentElement(); }

		/*void analyse(){};
		void generate(){};*/

	protected:

};

#endif /* __KILLU_XMLPARSER_H__ */

