/*
** A program to convert the XML rendered by KOffice into LATEX.
**
** Copyright (C) 2000-2003 Robert JACOLIN
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
//FileHeader* XmlParser::_fileHeader = 0;
Document* XmlParser::_root = 0;
KoStore* XmlParser::_in = NULL;
QString XmlParser::_filename = "";

XmlParser::XmlParser(Config* config, QString filename):_config(config)
	
{
	//_filename = filename;
	QFile f(filename);
	if(!f.open(IO_ReadOnly))
		return;
	if(!_document.setContent(&f))
	{
		f.close();
		return;
	}
	else
		f.close();
	//_eltCurrent = _document.documentElement();
}

XmlParser::XmlParser(Config* config, QByteArray in): _config(config)
{
	_document.setContent(in);
}

XmlParser::XmlParser(Config* config, const KoStore* in): _config(config)
{
  _in = const_cast<KoStore*>(in);
	if(!_in->open("root"))
	{
	        kdError(30503) << "Unable to open input file!" << endl;
	        return;
	}
	/* input file Reading */
	QByteArray array = _in->read(_in->size());
	_document.setContent(array);
	if(!_in->close())
	{
		kdError(30522) << "unable to close input file" << endl;
		return;
	}
}

XmlParser::XmlParser()
{
	_config = Config::instance();
	/*_in = xmlParser.getStorage();
	_fileHeader = xmlParser.getFileHeader();
	_root = xmlParser.getRoot();
	_document.setContent(_in);
	_filename = xmlParser.getFilename();*/
}

XmlParser::~XmlParser()
{
	/*if(_in != NULL)
		_in->close();*/
}

QDomNode XmlParser::getChild(QDomNode balise, QString name)
{
	QDomNode node = getChild(balise, name, 0);
	kdDebug(30522) << node.nodeName() << endl;
	return node;
}

bool XmlParser::isChild(QDomNode balise, QString name)
{
	if(balise.isElement())
		return balise.toElement().elementsByTagName(name).count();
	return false;
}

QDomNode XmlParser::getChild(QDomNode balise, QString name, int index)
{
	if(balise.isElement()) {
		QDomNodeList children = balise.toElement().elementsByTagName(name);
		if ( children.count() )
			return children.item(index);
	}
	return QDomNode();
}

QDomNode XmlParser::getChild(QDomNode balise, int index)
{
	QDomNodeList children = balise.childNodes();
	if ( children.count() )
		return children.item(index);
	return QDomNode();
}

QString XmlParser::getData(QDomNode balise, int index)
{
	return getChild(getChild(balise, index), 0).nodeValue();
}

int XmlParser::getNbChild(QDomNode balise)
{
	return balise.childNodes().count();
}

int XmlParser::getNbChild(QDomNode balise, QString name)
{
	if(balise.isElement())
		return balise.toElement().elementsByTagName(name).count();
	return -1;
}

QString  XmlParser::getChildName(QDomNode balise, int index)
{
	return balise.childNodes().item(index).nodeName();
}

QString  XmlParser::getAttr(QDomNode balise, QString name) const
{
	if(balise.isElement())
		return balise.toElement().attributeNode(name).value();
	return QString();
}
