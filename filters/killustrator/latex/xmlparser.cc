/*
** A program to convert the XML rendered by KWord into LATEX.
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

#include <kdebug.h>

#include "xmlparser.h"
#include "qfile.h"

/* Init static data */
Header* XmlParser::_fileHeader = 0;

XmlParser::XmlParser(QString filename):
		_filename(filename)
{
	QFile f(filename);
	if(!f.open(IO_ReadOnly))
		return;
	if(!_document.setContent(&f))
	{
		f.close();
		return;
	}
	f.close();
	//_eltCurrent = _document.documentElement();
}

XmlParser::XmlParser()
{
	//_index = 0;
}

XmlParser::~XmlParser()
{
	//kdDebug() << "Destruction of XmlParser (tree)" << endl;
}

/*QDomNode XmlParser::getChild(QString name)
{
	QDomNode node = getChild(name, 0);
	kdDebug() << node.nodeName() << endl;
	return node;
}*/

QDomNode XmlParser::getChild(QDomNode balise, QString name)
{
	QDomNode node = getChild(balise, name, 0);
	kdDebug() << node.nodeName() << endl;
	return node;
}

/*QDomNode XmlParser::getChild(QString name, int index)
{
	if(_eltCurrent.isElement())
		return _eltCurrent.toElement().elementsByTagName(name).item(index);
	return QDomNode();
}*/

QDomNode XmlParser::getChild(QDomNode balise, QString name, int index)
{
	if(balise.isElement())
		return balise.toElement().elementsByTagName(name).item(index);
	return QDomNode();
}

QDomNode XmlParser::getChild(QDomNode balise, int index)
{
	return balise.childNodes().item(index);
}


/*int XmlParser::getNbChild()
{
	return _eltCurrent.childNodes().count();
}*/

int XmlParser::getNbChild(QDomNode balise)
{
	return balise.childNodes().count();
}


/*int XmlParser::getNbChild(QString name)
{
	if(_eltCurrent.isElement())
		return _eltCurrent.toElement().elementsByTagName(name).count();
	return -1;
}*/

int XmlParser::getNbChild(QDomNode balise, QString name)
{
	if(balise.isElement())
		return balise.toElement().elementsByTagName(name).count();
	return -1;
}

/*QString  XmlParser::getChildName(int index)
{
	return _eltCurrent.childNodes().item(index).nodeName();
}*/

QString  XmlParser::getChildName(QDomNode balise, int index)
{
	return balise.childNodes().item(index).nodeName();
}


/*QString XmlParser::getAttr(QString name)
{
	if(_eltCurrent.isElement())
		return _eltCurrent.toElement().attributeNode(name).value();
	return QString();
}*/

QString  XmlParser::getAttr(QDomNode balise, QString name) const
{
	if(balise.isElement())
		return balise.toElement().attributeNode(name).value();
	return QString();
}
