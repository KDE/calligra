/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include <qmetaobject.h>
#include <qdom.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qcursor.h>
#include <qbuffer.h>
#include <qimage.h>

#include <kfiledialog.h>
#include <klocale.h>

#include "form.h"
#include "objpropbuffer.h"
#include "objecttree.h"
#include "formIO.h"

namespace KFormDesigner {

FormIO::FormIO(QObject *parent, ObjectPropertyBuffer *buffer=0, const char *name=0)
   : QObject(parent, name)
{
	m_buffer = buffer;
	m_count = 0;
}

int
FormIO::saveForm(Form *form, const QString &filename)
{
	QString m_filename;
	if(filename.isNull())
	{
		m_filename = KFileDialog::getSaveFileName(QString::null, i18n("*.ui|Qt Designer UI files"));
		if(m_filename.isNull())
			return 0;
	}
	else
		m_filename = filename;

	if(m_buffer)
		m_buffer->checkModifiedProp();

	QDomDocument domDoc("UI");
        QDomElement uiElement = domDoc.createElement("UI");
	domDoc.appendChild(uiElement);
	uiElement.setAttribute("version", "3.1");
	uiElement.setAttribute("stdsetdef", 1);

	QDomElement baseClass = domDoc.createElement("class");
	uiElement.appendChild(baseClass);
	QDomText baseClassV = domDoc.createTextNode("QWidget");
	baseClass.appendChild(baseClassV);

	saveWidget(form->objectTree(), uiElement, domDoc);
	
	QFile file(m_filename);
	if(file.open(IO_WriteOnly))
	{
	QTextStream stream(&file);
	stream << domDoc.toString() << endl;
	file.close();
	}
	
	kdDebug() << domDoc.toString() << endl;
	return 1;
}

int
FormIO::loadForm(Form *form, const QString &filename)
{
	return 1;
}

QDomElement
FormIO::prop(QDomDocument &parent, const char *name, const QVariant &value)
{
	QDomElement propertyE = parent.createElement("property");
	propertyE.setAttribute("name", name);

	QDomElement type;
	QDomText valueE;

	switch(value.type())
	{
		case QVariant::String:
		{
			type = parent.createElement("string");
			valueE = parent.createTextNode(value.toString());
			type.appendChild(valueE);
			break;
		}	
		case QVariant::CString:
		{
			type = parent.createElement("cstring");
			valueE = parent.createTextNode(value.toString());
			type.appendChild(valueE);
			break;
		}
		case QVariant::Rect:
		{
			type = parent.createElement("rect");
			QDomElement x = parent.createElement("x");
			QDomElement y = parent.createElement("y");
			QDomElement w = parent.createElement("width");
			QDomElement h = parent.createElement("height");
			QDomText valueX = parent.createTextNode(QString::number(value.toRect().x()));
			QDomText valueY = parent.createTextNode(QString::number(value.toRect().y()));
			QDomText valueW = parent.createTextNode(QString::number(value.toRect().width()));
			QDomText valueH = parent.createTextNode(QString::number(value.toRect().height()));

			x.appendChild(valueX);
			y.appendChild(valueY);
			w.appendChild(valueW);
			h.appendChild(valueH);

			type.appendChild(x);
			type.appendChild(y);
			type.appendChild(w);
			type.appendChild(h);
			break;
		}
		case QVariant::Color:
		{
			type = parent.createElement("color");
			QDomElement r = parent.createElement("red");
			QDomElement g = parent.createElement("green");
			QDomElement b = parent.createElement("blue");
			QDomText valueR = parent.createTextNode(QString::number(value.toColor().red()));
			QDomText valueG = parent.createTextNode(QString::number(value.toColor().green()));
			QDomText valueB = parent.createTextNode(QString::number(value.toColor().blue()));

			r.appendChild(valueR);
			g.appendChild(valueG);
			b.appendChild(valueB);

			type.appendChild(r);
			type.appendChild(g);
			type.appendChild(b);
			break;
		}
		case QVariant::Bool:
		{
			type = parent.createElement("bool");
			valueE = parent.createTextNode(QString::number(value.toBool()));
			type.appendChild(valueE);
			break;
		}
		case QVariant::Int:
		case QVariant::UInt:
		{
			type = parent.createElement("number");
			valueE = parent.createTextNode(QString::number(value.toInt()));
			type.appendChild(valueE);
			break;
		}
		case QVariant::Size:
		{
			type = parent.createElement("size");
			QDomElement w = parent.createElement("width");
			QDomElement h = parent.createElement("height");
			QDomText valueW = parent.createTextNode(QString::number(value.toSize().width()));
			QDomText valueH = parent.createTextNode(QString::number(value.toSize().height()));

			w.appendChild(valueW);
			h.appendChild(valueH);

			type.appendChild(w);
			type.appendChild(h);
			break;
		}
		case QVariant::Point:
		{
			type = parent.createElement("point");
			QDomElement x = parent.createElement("x");
			QDomElement y = parent.createElement("y");
			QDomText valueX = parent.createTextNode(QString::number(value.toPoint().x()));
			QDomText valueY = parent.createTextNode(QString::number(value.toPoint().y()));

			x.appendChild(valueX);
			y.appendChild(valueY);

			type.appendChild(x);
			type.appendChild(y);
			break;
		}
		case QVariant::Font:
		{
			type = parent.createElement("font");
			QDomElement f = parent.createElement("family");
			QDomElement p = parent.createElement("pointsize");
			QDomElement w = parent.createElement("weight");
			QDomElement b = parent.createElement("bold");
			QDomElement i = parent.createElement("italic");
			QDomElement u = parent.createElement("underline");
			QDomElement s = parent.createElement("strikeout");
			QDomText valueF = parent.createTextNode(value.toFont().family());
			QDomText valueP = parent.createTextNode(QString::number(value.toFont().pointSize()));
			QDomText valueW = parent.createTextNode(QString::number(value.toFont().weight()));
			QDomText valueB = parent.createTextNode(QString::number(value.toFont().bold()));
			QDomText valueI = parent.createTextNode(QString::number(value.toFont().italic()));
			QDomText valueU = parent.createTextNode(QString::number(value.toFont().underline()));
			QDomText valueS = parent.createTextNode(QString::number(value.toFont().strikeOut()));

			f.appendChild(valueF);
			p.appendChild(valueP);
			w.appendChild(valueW);
			b.appendChild(valueB);
			i.appendChild(valueI);
			u.appendChild(valueU);
			s.appendChild(valueS);

			type.appendChild(f);
			type.appendChild(p);
			type.appendChild(w);
			type.appendChild(b);
			type.appendChild(i);
			type.appendChild(u);
			type.appendChild(s);
			break;
		}
		case QVariant::Cursor:
		{
			type = parent.createElement("cursor");
			valueE = parent.createTextNode(QString::number(value.toCursor().shape()));
			type.appendChild(valueE);
			break;
		}
		case QVariant::SizePolicy:
		{
			type = parent.createElement("hsizetype");
			valueE = parent.createTextNode(QString::number(value.toSizePolicy().horData()));
			type.appendChild(valueE);
			type = parent.createElement("vsizetype");
			valueE = parent.createTextNode(QString::number(value.toSizePolicy().verData()));
			type.appendChild(valueE);
			break;
		}
		case QVariant::Pixmap:
		{
			type = parent.createElement("pixmap");
			valueE = parent.createTextNode(saveImage(parent, value.toPixmap()));
			type.appendChild(valueE);
			break;
		}
		default:
			break;
	}

	propertyE.appendChild(type);
	return propertyE;
}

QDomElement
FormIO::enumProp(QDomDocument &parent, const char *name, const QVariant &value)
{
	QDomElement propertyE = parent.createElement("property");
	propertyE.setAttribute("name", name);

	QDomElement type;
	QDomText valueE;
	
	if(value.type() == QVariant::StringList)
	{
		type = parent.createElement("set");
		valueE = parent.createTextNode(value.toStringList().join("|"));
		type.appendChild(valueE);
	}
	else
	{
		type = parent.createElement("enum");
		valueE = parent.createTextNode(value.toString());
		type.appendChild(valueE);
	}
	
	propertyE.appendChild(type);
	return propertyE;
}

void
FormIO::saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc)
{
	QDomElement tclass = domDoc.createElement("widget");
	tclass.setAttribute("class", item->widget()->className());
	tclass.appendChild(prop(domDoc, "name", item->widget()->property("name")));
	tclass.appendChild(prop(domDoc, "geometry", item->widget()->property("geometry")));

	for(QStringList::Iterator it = item->modifProp()->begin(); it != item->modifProp()->end(); ++it)
	{
		kdDebug() << "FormIO::save(): property: " << *it << endl;
		if((*it != QString("name")) && (*it != QString("geometry")))
		{
			int count = item->widget()->metaObject()->findProperty((*it).latin1(), true);
			const QMetaProperty *meta = item->widget()->metaObject()->property(count, true);
			if(meta->isEnumType())
			{
				if(meta->isSetType())
				tclass.appendChild( enumProp(domDoc, (*it).latin1(), 
				  QStringList::fromStrList(meta->valueToKeys(item->widget()->property(meta->name()).toInt())) ));
				  
				else
				tclass.appendChild( enumProp(domDoc, (*it).latin1(), 
				  meta->valueToKey(item->widget()->property(meta->name()).toInt())) );
			}
			else
			tclass.appendChild(prop(domDoc, (*it).latin1(), item->widget()->property((*it).latin1())));
		}
	}
	parent.appendChild(tclass);
	
	if(!item->children()->isEmpty())
	{
	for(ObjectTreeItem *objIt = item->children()->first(); objIt; objIt = item->children()->next())
		saveWidget(objIt, tclass, domDoc);
	}
}

QString
FormIO::saveImage(QDomDocument &domDoc, const QPixmap &pixmap)
{
	QDomNode node = domDoc.elementsByTagName("images").item(0);
	QDomElement images;
	if(node.isNull())
	{
		images = domDoc.createElement("images");
		QDomElement ui = domDoc.elementsByTagName("UI").item(0).toElement();
		ui.appendChild(images);
	}
	else
		images = node.toElement();
	
	QDomElement image = domDoc.createElement("image");
	QString name = "image" + QString::number(m_count);
	image.setAttribute("name", name);
	m_count++;
	
	QImage img = pixmap.convertToImage();
	QByteArray ba;
	QBuffer buf(ba);
	buf.open( IO_WriteOnly | IO_Translate );
	QString format = img.depth() > 1 ? "XPM" : "XBM";
	QImageIO iio( &buf, format.latin1() );
	iio.setImage( img );
	iio.write();
	buf.close();
	QByteArray bazip = qCompress( ba );
	ulong len = bazip.size();
	
	QDomElement data = domDoc.createElement("data");
	data.setAttribute("format", format + ".GZ");
	data.setAttribute("length", ba.size());
	
	static const char hexchars[] = "0123456789abcdef";
	QString content;
	for(int i = 4; i < (int)len; ++i)
	{
	uchar s = (uchar) bazip[i];
	content += hexchars[s >> 4];
	content += hexchars[s & 0x0f];
	}

	QDomText text = domDoc.createTextNode(content);
	data.appendChild(text);
	image.appendChild(data);
	images.appendChild(image);
	
	return name;
}

}


