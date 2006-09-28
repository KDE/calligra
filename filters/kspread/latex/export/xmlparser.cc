/*
** A program to convert the XML rendered by KOffice into LATEX.
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

#include <kdebug.h>
#include <KoStore.h>

#include "xmlparser.h"
#include "qfile.h"

/* Init static data */
FileHeader* XmlParser::_fileHeader = 0;
Document* XmlParser::_root = 0;
KoStore* XmlParser::_in = NULL;

XmlParser::XmlParser(QString filename): _filename(filename)
{
	QFile f(filename);
	if(!f.open(QIODevice::ReadOnly))
		return;
	if(!_document.setContent(&f))
	{
		f.close();
		return;
	}
	f.close();
	//_eltCurrent = _document.documentElement();
}

XmlParser::XmlParser(QByteArray in)
{
	_document.setContent(in);
}

XmlParser::XmlParser(const KoStore* in)
{
  _in = const_cast<KoStore*>(in);
	if(!_in->open("root"))
	{
		kError(30522) << "Unable to open input file!" << endl;
		return;
	}
	/* input file Reading */
	QByteArray array = _in->read(_in->size());
	_document.setContent(array);
}

XmlParser::XmlParser()
{
}

XmlParser::~XmlParser()
{
	if(_in != NULL)
		_in->close();
}

QDomNode XmlParser::getChild(QDomNode node, QString name)
{
	QDomNode childNode = getChild(node, name, 0);
	kDebug(30522) << childNode.nodeName() << endl;
	return node;
}

bool XmlParser::isChild(QDomNode node, QString name)
{
	if(node.isElement())
		return node.toElement().elementsByTagName(name).count();
	return false;
}

QDomNode XmlParser::getChild(QDomNode node, QString name, int index)
{
	if(node.isElement()) {
		QDomNodeList children = node.toElement().elementsByTagName(name);
		if ( children.count() )
			return children.item(index);
	}
	return QDomNode();
}

QDomNode XmlParser::getChild(QDomNode node, int index)
{
	QDomNodeList children = node.childNodes();
	if ( children.count() )
		return children.item(index);
	return QDomNode();
}

QString XmlParser::getData(QDomNode node, int index)
{
	return getChild(getChild(node, index), 0).nodeValue();
}

QString XmlParser::getData(QDomNode node, QString name)
{
	return getChild(getChild(node, name), 0).nodeValue();
}

int XmlParser::getNbChild(QDomNode node)
{
	return node.childNodes().count();
}

int XmlParser::getNbChild(QDomNode node, QString name)
{
	if(node.isElement())
		return node.toElement().elementsByTagName(name).count();
	return -1;
}

QString  XmlParser::getChildName(QDomNode node, int index)
{
	return node.childNodes().item(index).nodeName();
}

QString  XmlParser::getAttr(QDomNode node, QString name) const
{
	if(node.isElement())
		return node.toElement().attributeNode(name).value();
	return QString();
}
