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
#include <qtabwidget.h>
#include <qlayout.h>
#include <qobjectlist.h>

#include <kfiledialog.h>
#include <klocale.h>

#include "form.h"
#include "container.h"
#include "objpropbuffer.h"
#include "objecttree.h"
#include "formmanager.h"
#include "widgetlibrary.h"
#include "spacer.h"

#include "formIO.h"

namespace KFormDesigner {

// Helper classes for sorting widgets before saving (because Designer is too stupid to put them in the right order)
class HorWidgetList : public QObjectList
{
	public:
	HorWidgetList() {;}
	virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
	{
		QWidget *w1 = static_cast<QWidget*>(item1);
		QWidget *w2 = static_cast<QWidget*>(item2);

		if(w1->x() < w2->x())
			return -1;
		if(w1->x() > w2->x())
			return 1;
		return 0; // item1 == item2
	}
};

class VerWidgetList : public QObjectList
{
	public:
	VerWidgetList() {;}
	virtual int compareItems(QPtrCollection::Item item1, QPtrCollection::Item item2)
	{
		QWidget *w1 = static_cast<QWidget*>(item1);
		QWidget *w2 = static_cast<QWidget*>(item2);

		if(w1->y() < w2->y())
			return -10;
		if(w1->y() > w2->y())
			return 1;
		return 0; // item1 == item2
	}
};

// FormIO itself

FormIO::FormIO(QObject *parent, const char *name=0)
   : QObject(parent, name)
{
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
	form->setFilename(m_filename);

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
		stream << domDoc.toString(3) << endl;
		file.close();
	}
	
	kdDebug() << domDoc.toString(2) << endl;
	return 1;
}

int
FormIO::loadForm(Form *form, QWidget *parent, const QString &filename)
{
	QString errMsg;
	int errLine;
	int errCol;
	QString m_filename;

	if(filename.isNull())
	{
		m_filename = KFileDialog::getOpenFileName(QString::null, i18n("*.ui|Qt Designer UI files"));
		if(m_filename.isNull())
			return 0;
	}
	
	QFile file(m_filename);
	if(!file.open(IO_ReadOnly))
		return 0;
	QTextStream stream(&file);
	QString text = stream.read();
	 
	QDomDocument inBuf;
	bool parsed = inBuf.setContent(text, false, &errMsg, &errLine, &errCol);

	if(!parsed)
	{
		kdDebug() << "WidgetWatcher::load(): " << errMsg << endl;
		kdDebug() << "WidgetWatcher::load(): line: " << errLine << " col: " << errCol << endl;
		return 0;
	}

	QDomElement ui = inBuf.namedItem("UI").toElement();
	QDomElement element = ui.namedItem("widget").toElement();
	createToplevelWidget(form, parent, element);

	return 1;
}

QDomElement
FormIO::prop(QDomDocument &parent, const char *name, const QVariant &value, QWidget *w)
{
	// Widget specific properties and attributes ///////////////
	
	// The widget is a page of a QTabWidget
	if(name == QString("title") && (w->parentWidget()->inherits("QWidgetStack")))
	{
		QTabWidget *tab = (QTabWidget*)w->parentWidget()->parentWidget();

		QDomElement propertyE = parent.createElement("attribute");
		propertyE.setAttribute("name", name);
		QDomElement type = parent.createElement("string");
		QDomText valueE = parent.createTextNode(tab->tabLabel(w));

		type.appendChild(valueE);
		propertyE.appendChild(type);
		return propertyE;
	}
	
	// TODO : The widget is a page of a QWidgetStack
	
	// End of widget specific stuff //////////////////////
	


	QDomElement propertyE = parent.createElement("property");
	propertyE.setAttribute("name", name);

	QDomElement type;
	QDomText valueE;

	// Checking if this property is enum or set type
	int count = w->metaObject()->findProperty(name, true);
	const QMetaProperty *meta = w->metaObject()->property(count, true);
	if(meta && meta->isEnumType())
	{
		if(meta->isSetType())
		{
			QStringList list = QStringList::fromStrList(meta->valueToKeys(value.toInt()));
			type = parent.createElement("set");
			valueE = parent.createTextNode(list.join("|"));
			type.appendChild(valueE);
		}
		else
		{
			QString s = meta->valueToKey(value.toInt());
			type = parent.createElement("enum");
			valueE = parent.createTextNode(s);
			type.appendChild(valueE);
		}
		propertyE.appendChild(type);
		return propertyE;
	}
	

	// Saving a "normal" property
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
			type = parent.createElement("sizepolicy");
			QDomElement h = parent.createElement("hsizetype");
			QDomElement v = parent.createElement("vsizetype");
			QDomElement hs = parent.createElement("horstretch");
			QDomElement vs = parent.createElement("verstretch");
			QDomText valueH = parent.createTextNode(QString::number(value.toSizePolicy().horData()));
			QDomText valueV = parent.createTextNode(QString::number(value.toSizePolicy().verData()));
			QDomText valueHS = parent.createTextNode(QString::number(value.toSizePolicy().horStretch()));
			QDomText valueVS = parent.createTextNode(QString::number(value.toSizePolicy().verStretch()));

			h.appendChild(valueH);
			v.appendChild(valueV);
			hs.appendChild(valueHS);
			vs.appendChild(valueVS);
			
			type.appendChild(h);
			type.appendChild(v);
			type.appendChild(hs);
			type.appendChild(vs);
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

QVariant
FormIO::readProp(QDomNode node, QObject *obj, const QString &name)
{
	QDomElement tag = node.toElement();
	QString type = node.toElement().tagName();
	
	if(type == "string" | type == "cstring")
		return tag.text();
	else if(type == "rect")
	{
		QDomElement x = node.namedItem("x").toElement();
		QDomElement y = node.namedItem("y").toElement();
		QDomElement w = node.namedItem("width").toElement();
		QDomElement h = node.namedItem("height").toElement();

		int rx = x.text().toInt();
		int ry = y.text().toInt();
		int rw = w.text().toInt();
		int rh = h.text().toInt();

		return QRect(rx, ry, rw, rh);
	}
	else if(type == "color")
	{
		QDomElement r = node.namedItem("red").toElement();
		QDomElement g = node.namedItem("green").toElement();
		QDomElement b = node.namedItem("blue").toElement();
		
		int red = r.text().toInt();
		int green = g.text().toInt();
		int blue = b.text().toInt();
		
		return QColor(red, green, blue);
	}
	else if(type == "bool")
	{
		return QVariant(tag.text().toInt(), 3); 
	}
	else if(type == "number")
	{
		return tag.text().toInt();
	}
	else if(type == "size")
	{
		QDomElement w = node.namedItem("width").toElement();
		QDomElement h = node.namedItem("height").toElement();
		
		return QSize(w.text().toInt(), h.text().toInt());
	}
	else if(type == "point")
	{
		QDomElement x = node.namedItem("x").toElement();
		QDomElement y = node.namedItem("y").toElement();
		
		return QPoint(x.text().toInt(), y.text().toInt());
	}
	else if(type == "font")
	{
		QDomElement fa = node.namedItem("family").toElement();
		QDomElement p = node.namedItem("pointsize").toElement();
		QDomElement w = node.namedItem("weight").toElement();
		QDomElement b = node.namedItem("bold").toElement();
		QDomElement i = node.namedItem("italic").toElement();
		QDomElement u = node.namedItem("underline").toElement();
		QDomElement s = node.namedItem("strikeout").toElement();
		
		QFont f;
		f.setFamily(fa.text());
		f.setPointSize(p.text().toInt());
		f.setWeight(w.text().toInt());
		f.setBold(b.text().toInt());
		f.setItalic(i.text().toInt());
		f.setUnderline(u.text().toInt());
		f.setStrikeOut(s.text().toInt());
		
		return f;
	}
	else if(type == "cursor")
	{
		return QCursor(tag.text().toInt());
	}
	else if(type == "sizepolicy")
	{
		QDomElement h = node.namedItem("hsizetype").toElement();
		QDomElement v = node.namedItem("vsizetype").toElement();
		QDomElement hs = node.namedItem("horstretch").toElement();
		QDomElement vs = node.namedItem("verstretch").toElement();
		
		QSizePolicy s;
		s.setHorData((QSizePolicy::SizeType)h.text().toInt());
		s.setVerData((QSizePolicy::SizeType)v.text().toInt());
		s.setHorStretch(hs.text().toInt());
		s.setVerStretch(vs.text().toInt());
		return s;
	}
	else if(type == "pixmap")
	{
		return loadImage(tag.ownerDocument(), tag.text());
	}
	else if(type == "enum")
		return tag.text();
	else if(type == "set")
	{
		int count = obj->metaObject()->findProperty(name.latin1(), true);
		const QMetaProperty *meta = obj->metaObject()->property(count, true);
		
		if(meta->isSetType())
		{
			QStrList keys;
			QStringList list = QStringList::split("|", tag.text());
			for(QStringList::iterator it = list.begin(); it != list.end(); ++it)
				keys.append((*it).latin1());
		
			return QVariant(meta->keysToValue(keys));
		}
	}
		return QVariant();
}

void
FormIO::readAttribute(QDomNode node, QObject *obj, const QString &name)
{
	QDomElement tag = node.toElement();
	QWidget *w = (QWidget*)obj; 
	if((name == "title") && (w->parentWidget()->isA("QTabWidget")))
	{
		QTabWidget *tab = (QTabWidget*)w->parentWidget();
		tab->addTab(w, tag.text());
	}
}

void
FormIO::saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc)
{
	if(item->className() == "Spacer")
	{
		Spacer::saveSpacer(item, parent, domDoc);
		return;
	}

	QDomElement tclass = domDoc.createElement("widget");
	tclass.setAttribute("class", item->className());
	tclass.appendChild(prop(domDoc, "name", item->widget()->property("name"), item->widget()));
	tclass.appendChild(prop(domDoc, "geometry", item->widget()->property("geometry"), item->widget()));

	for(QStringList::Iterator it = item->modifProp()->begin(); it != item->modifProp()->end(); ++it)
	{
		if((*it != QString("name")) && (*it != QString("geometry")))
			tclass.appendChild(prop(domDoc, (*it).latin1(), item->widget()->property((*it).latin1()), item->widget()));
	}
	parent.appendChild(tclass);

	// Saving container 's layout if there is one
	QDomElement layout;
	if(item->container())
	{
		QString nodeName;
		switch(item->container()->layoutType())
		{
			case Container::HBox:
			{
				nodeName = "hbox";
				break;
			}
			case Container::VBox:
			{
				nodeName = "vbox";
				break;
			}
			case Container::Grid:
			{
				nodeName = "grid";
				break;
			}
		}
		if(!nodeName.isNull())
		{
			layout = domDoc.createElement(nodeName);
			layout.appendChild(prop(domDoc, "name", "unnamed", item->widget()));
			tclass.appendChild(layout);
		}
	}

	if(!item->children()->isEmpty() && layout.isNull())
	{
		for(ObjectTreeItem *objIt = item->children()->first(); objIt; objIt = item->children()->next())
			saveWidget(objIt, tclass, domDoc);
	}
	else if(!item->children()->isEmpty())
	{
		QObjectList *list;
		if(layout.tagName() == "hbox")
			list = new HorWidgetList();
		else
			list = new VerWidgetList();

		for(ObjectTreeItem *objTree = item->children()->first(); objTree; objTree = item->children()->next())
			list->append((QObject*)(objTree->widget()));
		list->sort();

		QObject *obj;
		QObjectListIt it(*list);
		while ((obj = it.current()) != 0)
		{
			ObjectTreeItem *tree = item->container()->form()->objectTree()->lookup(obj->name());
			saveWidget(tree, layout, domDoc);
			++it;
		}
		delete list;
	}
}

void
FormIO::loadWidget(Container *container, WidgetLibrary *lib, const QDomElement &el, QWidget *parent)
{
	QString wname;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "name"))
		{
			wname = n.toElement().text();
			break;
		}
		
	}

	if(el.tagName() == "spacer")
		Spacer::loadSpacer(wname, container, lib, el, parent);

	QWidget *w;
	if(!parent)
		w = lib->createWidget(el.attribute("class"), container->widget(), wname.latin1(), container);
	else
		w = lib->createWidget(el.attribute("class"), parent, wname.latin1(), container);
	if(!w)  return;
	
	ObjectTreeItem *tree;
	if (!container->form()->objectTree()->lookup(wname))
	{
		tree =  new ObjectTreeItem(el.attribute("class"), wname, w);
		container->form()->objectTree()->addChild(container->tree(), tree);
	}
	else
		tree = container->form()->objectTree()->lookup(wname);
	
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().tagName() == "property")
		{
			QString name = n.toElement().attribute("name");

			QVariant val = readProp(n.toElement().firstChild(), w, name);
			w->setProperty(name.latin1(), val);
			tree->addModProperty(name);
		}
		if(n.toElement().tagName() == "attribute")
		{
			QString name = n.toElement().attribute("name");
			readAttribute(n.toElement().firstChild(), w, name);
		}
		if(n.toElement().tagName() == "widget")
		{
			if(tree->container())
				loadWidget(tree->container(), lib, n.toElement());
			else
				loadWidget(container, lib, n.toElement(), w);
		}
		if(n.toElement().tagName() == "spacer")
		{
			loadWidget(container, lib, n.toElement(), w);
		}
		if((n.toElement().tagName() == "vbox") || (n.toElement().tagName() == "hbox") || (n.toElement().tagName() == "grid"))
		{
			loadLayout(n.toElement().tagName(), tree);
			for(QDomNode m = n.toElement().firstChild(); !m.isNull(); m = m.nextSibling())
			{
				kdDebug() << "tagname is " << m.toElement().tagName() << endl;
				if(m.toElement().tagName() == "property")
				{
					QString name = m.toElement().attribute("name");
					if(name == "name")
						continue;

					QVariant val = readProp(m.toElement().firstChild(), w, name);
					w->setProperty(name.latin1(), val);
					tree->addModProperty(name);
				}
				if(m.toElement().tagName() == "attribute")
				{
					QString name = m.toElement().attribute("name");
					readAttribute(m.toElement().firstChild(), w, name);
				}
				if(m.toElement().tagName() == "widget")
				{
					if(tree->container())
						loadWidget(tree->container(), lib, m.toElement());
					else
						loadWidget(container, lib, m.toElement(), w);
				}
				if(m.toElement().tagName() == "spacer")
				{
					loadWidget(tree->container(), lib, m.toElement(), w);
				}
			}

			if(tree->container())
				tree->container()->setLayout(tree->container()->layoutType());
		}
	}

	w->show();
}

void
FormIO::createToplevelWidget(Form *form, QWidget *parent, QDomElement &el)
{
	QString wname;
	QWidget *w;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "name"))
		{
			wname = n.toElement().text();
			break;
		}
		
	}

	w = new QWidget(parent, wname.latin1());
	form->createToplevel(w);
	form->setInteractiveMode(false);

	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().tagName() == "property")
		{
			QString name = n.toElement().attribute("name");

			QVariant val = readProp(n.toElement().toElement().firstChild(), w, name);
			w->setProperty(name.latin1(), val);
			form->objectTree()->addModProperty(name);
		}
		if(n.toElement().tagName() == "widget")	
			loadWidget(form->toplevelContainer(), form->manager()->lib(), n.toElement());
	}
	w->show();
	form->setInteractiveMode(true);
}

void
FormIO::loadLayout(const QString &name, ObjectTreeItem *tree)
{
	if(!tree->container())
		return;

	if(name == "hbox")
	{
		tree->container()->setLayout(Container::HBox);
	}
	else if(name == "vbox")
	{
		tree->container()->setLayout(Container::VBox);
	}
	else if(name == "grid")
	{
		tree->container()->setLayout(Container::Grid);
	}
}

QString
FormIO::saveImage(QDomDocument &domDoc, const QPixmap &pixmap)
{
	QDomNode node = domDoc.namedItem("images");
	QDomElement images;
	if(node.isNull())
	{
		images = domDoc.createElement("images");
		QDomElement ui = domDoc.namedItem("UI").toElement();
		ui.appendChild(images);
	}
	else
		images = node.toElement();
	
	int count = images.childNodes().count();
	QDomElement image = domDoc.createElement("image");
	QString name = "image" + QString::number(count);
	image.setAttribute("name", name);
	
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

QPixmap
FormIO::loadImage(QDomDocument domDoc, QString name)
{
	QDomElement images = domDoc.namedItem("UI").namedItem("images").toElement();
	if(images.isNull())
		return 0;

	QDomElement image;
	for(QDomNode n = images.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "image") && (n.toElement().attribute("name") == name))
		{
			image = n.toElement();
			break;
		}
	}

	QPixmap pix;
	QString data = image.namedItem("data").toElement().text();
	const int lengthOffset = 4;
	int baSize = data.length() / 2 + lengthOffset;
	uchar *ba = new uchar[baSize];
	for(int i = lengthOffset; i < baSize; ++i)
	{
		char h = data[2 * (i-lengthOffset)].latin1();
		char l = data[2 * (i-lengthOffset) + 1].latin1();
		uchar r = 0;
		if(h <= '9')
		    r += h - '0';
		else
		    r += h - 'a' + 10;
		r = r << 4;
		if(l <= '9')
		    r += l - '0';
		else
		    r += l - 'a' + 10;
		ba[i] = r;
	}
	
	QString format = image.namedItem("data").toElement().attribute("format", "PNG");
	if((format == "XPM.GZ") || (format == "XBM.GZ"))
	{
		ulong len = image.attribute("length").toULong();
		if(len < data.length() * 5)
		    len = data.length() * 5;
		// qUncompress() expects the first 4 bytes to be the expected length of
		// the uncompressed data
		ba[0] = ( len & 0xff000000 ) >> 24;
		ba[1] = ( len & 0x00ff0000 ) >> 16;
		ba[2] = ( len & 0x0000ff00 ) >> 8;
		ba[3] = ( len & 0x000000ff );
		QByteArray baunzip = qUncompress(ba, baSize);
		pix.loadFromData( (const uchar*)baunzip.data(), baunzip.size(), format.left(format.find('.')).latin1() );
	}
	else
		pix.loadFromData( (const uchar*)ba+lengthOffset, baSize-lengthOffset, format.latin1() );

	delete[] ba;

	return pix;
}

}

#include "formIO.moc"
