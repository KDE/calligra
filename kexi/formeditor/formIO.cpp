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
#include <qdatetime.h>

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

//typedef QPtrList<QWidget> QWidgetList;

namespace KFormDesigner {

// Helper classes for sorting widgets before saving (because Designer is too stupid to put them in the right order)
/*class HorWidgetList : public QWidgetList
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

class VerWidgetList : public QWidgetList
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
};*/

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

	QDomDocument domDoc;
	saveFormToDom(form, domDoc);

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
FormIO::saveForm(Form *form, QByteArray &dest)
{
	QDomDocument domDoc;
	saveFormToDom(form, domDoc);
	dest = domDoc.toCString();
	return 1;
}

int
FormIO::saveFormToDom(Form *form, QDomDocument &domDoc)
{
	domDoc = QDomDocument("UI");
        QDomElement uiElement = domDoc.createElement("UI");
	domDoc.appendChild(uiElement);
	uiElement.setAttribute("version", "3.1");
	uiElement.setAttribute("stdsetdef", 1);

	QDomElement baseClass = domDoc.createElement("class");
	uiElement.appendChild(baseClass);
	QDomText baseClassV = domDoc.createTextNode("QWidget");
	baseClass.appendChild(baseClassV);

	saveWidget(form->objectTree(), uiElement, domDoc);

	QDomElement layoutDefaults = domDoc.createElement("layoutDefaults");
	layoutDefaults.setAttribute("spacing", QString::number(Form::defaultSpacing()));
	layoutDefaults.setAttribute("margin", QString::number(Form::defaultMargin()));
	uiElement.appendChild(layoutDefaults);

	return 1;
}

int
FormIO::loadFormData(Form *form, QWidget *container, QByteArray &src)
{
	QString errMsg;
	int errLine;
	int errCol;

	QDomDocument inBuf;
	bool parsed = inBuf.setContent(src, false, &errMsg, &errLine, &errCol);

	if(!parsed)
	{
		kdDebug() << "WidgetWatcher::load(): " << errMsg << endl;
		kdDebug() << "WidgetWatcher::load(): line: " << errLine << " col: " << errCol << endl;
		return 0;
	}

	loadFormFromDom(form, container, inBuf);
	return 1;
}

int
FormIO::loadForm(Form *form, QWidget *container, const QString &filename)
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

	loadFormFromDom(form, container, inBuf);
	return 1;
}

int
FormIO::loadFormFromDom(Form *form, QWidget *container, QDomDocument &inBuf)
{
	QDomElement ui = inBuf.namedItem("UI").toElement();
	QDomElement element = ui.namedItem("widget").toElement();
	createToplevelWidget(form, container, element);

	return 1;
}

void
FormIO::prop(QDomElement &parentNode, QDomDocument &parent, const char *name, const QVariant &value, QWidget *w, WidgetLibrary *lib)
{
	// Widget specific properties and attributes ///////////////
	kdDebug() << "FormIO::prop()  saving the property " << name << endl;
	if(w->metaObject()->findProperty(name, true) == -1)
	{
		kdDebug() << "the object doesn't have this property" << endl;
		if(lib)
			lib->saveSpecialProperty(w->className(), name, value, w, parentNode, parent);
		return;
	}

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
			kdDebug() << "the property is enum type " << s << endl;
			type = parent.createElement("enum");
			valueE = parent.createTextNode(s);
			type.appendChild(valueE);
		}
		propertyE.appendChild(type);
		parentNode.appendChild(propertyE);
		return;
	}


	// Saving a "normal" property
	writeVariant(parent, type, valueE, value);

	propertyE.appendChild(type);
	parentNode.appendChild(propertyE);
}

void
FormIO::writeVariant(QDomDocument &parent, QDomElement &type, QDomText &valueE, QVariant value)
{
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
		case QVariant::Time:
		{
			type = parent.createElement("time");
			QDomElement h = parent.createElement("hour");
			QDomElement m = parent.createElement("minute");
			QDomElement s = parent.createElement("second");
			QDomText valueH = parent.createTextNode(QString::number(value.toTime().hour()));
			QDomText valueM = parent.createTextNode(QString::number(value.toTime().minute()));
			QDomText valueS = parent.createTextNode(QString::number(value.toTime().second()));

			h.appendChild(valueH);
			m.appendChild(valueM);
			s.appendChild(valueS);

			type.appendChild(h);
			type.appendChild(m);
			type.appendChild(s);
			break;
		}
		case QVariant::Date:
		{
			type = parent.createElement("date");
			QDomElement y = parent.createElement("year");
			QDomElement m = parent.createElement("month");
			QDomElement d = parent.createElement("day");
			QDomText valueY = parent.createTextNode(QString::number(value.toDate().year()));
			QDomText valueM = parent.createTextNode(QString::number(value.toDate().month()));
			QDomText valueD = parent.createTextNode(QString::number(value.toDate().day()));

			y.appendChild(valueY);
			m.appendChild(valueM);
			d.appendChild(valueD);

			type.appendChild(y);
			type.appendChild(m);
			type.appendChild(d);
			break;
		}
		case QVariant::DateTime:
		{
			type = parent.createElement("datetime");
			QDomElement h = parent.createElement("hour");
			QDomElement m = parent.createElement("minute");
			QDomElement s = parent.createElement("second");
			QDomElement y = parent.createElement("year");
			QDomElement mo = parent.createElement("month");
			QDomElement d = parent.createElement("day");
			QDomText valueH = parent.createTextNode(QString::number(value.toDateTime().time().hour()));
			QDomText valueM = parent.createTextNode(QString::number(value.toDateTime().time().minute()));
			QDomText valueS = parent.createTextNode(QString::number(value.toDateTime().time().second()));
			QDomText valueY = parent.createTextNode(QString::number(value.toDateTime().date().year()));
			QDomText valueMo = parent.createTextNode(QString::number(value.toDateTime().date().month()));
			QDomText valueD = parent.createTextNode(QString::number(value.toDateTime().date().day()));

			h.appendChild(valueH);
			m.appendChild(valueM);
			s.appendChild(valueS);
			y.appendChild(valueY);
			mo.appendChild(valueMo);
			d.appendChild(valueD);

			type.appendChild(h);
			type.appendChild(m);
			type.appendChild(s);
			type.appendChild(y);
			type.appendChild(mo);
			type.appendChild(d);
			break;
		}
		default:
			break;
	}
}

void
FormIO::saveProperty(QDomElement &parentNode, QDomDocument &domDoc, const QString &tagName, const QString &property, const QVariant &value)
{
	QDomElement propertyE = domDoc.createElement(tagName);
	propertyE.setAttribute("name", property);
	QDomElement type;
	QDomText valueE;

	writeVariant(domDoc, type, valueE, value);
	propertyE.appendChild(type);
	parentNode.appendChild(propertyE);
}

QVariant
FormIO::readProp(QDomNode node, QObject *obj, const QString &name)
{
	QDomElement tag = node.toElement();
	QString text = tag.text();
	QString type = tag.tagName();

	if(type == "string" | type == "cstring")
		return text;
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
		return QVariant(text.toInt(), 3);
	}
	else if(type == "number")
	{
		return text.toInt();
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
		return QCursor(text.toInt());
	}
	else if(type == "time")
	{
		QDomElement h = node.namedItem("hour").toElement();
		QDomElement m = node.namedItem("minute").toElement();
		QDomElement s = node.namedItem("second").toElement();

		return QTime(h.text().toInt(), m.text().toInt(), s.text().toInt());
	}
	else if(type == "date")
	{
		QDomElement y = node.namedItem("year").toElement();
		QDomElement m = node.namedItem("month").toElement();
		QDomElement d = node.namedItem("day").toElement();

		return QDate(y.text().toInt(), m.text().toInt(), d.text().toInt());
	}
	else if(type == "datetime")
	{
		QDomElement h = node.namedItem("hour").toElement();
		QDomElement m = node.namedItem("minute").toElement();
		QDomElement s = node.namedItem("second").toElement();
		QDomElement y = node.namedItem("year").toElement();
		QDomElement mo = node.namedItem("month").toElement();
		QDomElement d = node.namedItem("day").toElement();

		QTime t(h.text().toInt(), m.text().toInt(), s.text().toInt());
		QDate da(y.text().toInt(), mo.text().toInt(), d.text().toInt());

		return QDateTime(da, t);
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
		return loadImage(tag.ownerDocument(), text);
	}
	else if(type == "enum")
		return text;
	else if(type == "set")
	{
		int count = obj->metaObject()->findProperty(name.latin1(), true);
		const QMetaProperty *meta = obj->metaObject()->property(count, true);

		if(meta->isSetType())
		{
			QStrList keys;
			QStringList list = QStringList::split("|", text);
			for(QStringList::iterator it = list.begin(); it != list.end(); ++it)
				keys.append((*it).latin1());

			return QVariant(meta->keysToValue(keys));
		}
	}
		return QVariant();
}

void
FormIO::saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc, bool insideGridLayout)
{
	if (!item)
		return;
	bool savedAlignment = false;
	if(item->className() == "Spacer")
	{
		Spacer::saveSpacer(item, parent, domDoc, insideGridLayout);
		return;
	}

	QDomElement tclass = domDoc.createElement("widget");
	if(insideGridLayout)
	{
		tclass.setAttribute("row", item->gridRow());
		tclass.setAttribute("column", item->gridCol());
		if(item->spanMultipleCells())
		{
			tclass.setAttribute("rowspan", item->gridRowSpan());
			tclass.setAttribute("colspan", item->gridColSpan());
		}
	}
	if((item->widget()->isA("HBox")) || (item->widget()->isA("VBox")) || (item->widget()->isA("Grid")))
		tclass.setAttribute("class", "QLayoutWidget");
	else
		tclass.setAttribute("class", item->widget()->className());
	prop(tclass, domDoc, "name", item->widget()->property("name"), item->widget());

	if((parent.tagName() == "widget") || (parent.tagName() == "UI"))
		prop(tclass, domDoc, "geometry", item->widget()->property("geometry"), item->widget());

	WidgetLibrary *lib=0;
	if(item->container())
		lib = item->container()->form()->manager()->lib();
	else
		lib = item->parent()->container()->form()->manager()->lib();

	for(QMap<QString,QVariant>::Iterator it = item->modifProp()->begin(); it != item->modifProp()->end(); ++it)
	{
		QString name = it.key();
		if((name == QString("hAlign")) || (name == QString("vAlign")) || (name == QString("wordbreak")))
		{
			if(!savedAlignment)
			{
				prop(tclass, domDoc, "alignment", item->widget()->property("alignment"), item->widget());
				savedAlignment = true;
			}
		}

		else if((name != "name") && (name != "geometry") && (name != "layout"))
			prop(tclass, domDoc, it.key().latin1(), item->widget()->property(it.key().latin1()), item->widget(), lib);
	}
	parent.appendChild(tclass);

	// Saving container 's layout if there is one
	QDomElement layout;
	if(item->container() && item->container()->layoutType() != Container::NoLayout)
	{
		QString nodeName;
		kdDebug() << "the type of the container layout is " << item->container()->layout() << endl;
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
			default:
				break;
		}
		if(!nodeName.isNull())
		{
			layout = domDoc.createElement(nodeName);
			prop(layout, domDoc, "name", "unnamed", item->widget());
			if(item->modifProp()->contains("layoutMargin"))
				saveProperty(layout, domDoc, "property", "margin", item->container()->layoutMargin());
			if(item->modifProp()->contains("layoutSpacing"))
				saveProperty(layout, domDoc, "property", "spacing", item->container()->layoutSpacing());
			tclass.appendChild(layout);
		}
	}

	if(!item->children()->isEmpty() && layout.isNull())
	{
		for(ObjectTreeItem *objIt = item->children()->first(); objIt; objIt = item->children()->next())
			saveWidget(objIt, tclass, domDoc);
	}
	else if((!item->children()->isEmpty()) && (layout.tagName() == "grid"))
	{
		for(ObjectTreeItem *objIt = item->children()->first(); objIt; objIt = item->children()->next())
			saveWidget(objIt, layout, domDoc, true);
	}
	else if(!item->children()->isEmpty())
	{
		QtWidgetList *list;
		if(layout.tagName() == "hbox")
			list = new HorWidgetList();
		else
			list = new VerWidgetList();

		for(ObjectTreeItem *objTree = item->children()->first(); objTree; objTree = item->children()->next())
			list->append(objTree->widget());
		list->sort();

		for(QWidget *obj = list->first(); obj; obj = list->next())
		{
			ObjectTreeItem *tree = item->container()->form()->objectTree()->lookup(obj->name());
			saveWidget(tree, layout, domDoc);
		}
		delete list;
	}

	addIncludeFile(lib->includeFile(item->widget()->className()), domDoc);
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

	QWidget *w;
	QString classname;

	if(el.tagName() == "spacer")
		classname = "Spacer";
	else if(el.attribute("class") == "QLayoutWidget")
	{
		for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
		{
			QString tagName = n.toElement().tagName();
			if(tagName == "property")
				continue;
			if(tagName == "hbox")
				classname = "HBox";
			else if(tagName == "vbox")
				classname = "VBox";
			else if(tagName == "grid")
				classname = "Grid";
		}
	}
	else
		classname = lib->checkAlternateName(el.attribute("class"));

	if(!parent)
		w = lib->createWidget(classname, container->widget(), wname.latin1(), container);
	else
		w = lib->createWidget(classname, parent, wname.latin1(), container);

	if(!w)  return;

	ObjectTreeItem *tree;
	if (!container->form()->objectTree()->lookup(wname))
	{
		EventEater *eater = new EventEater(w, container);
		tree =  new ObjectTreeItem(lib->displayName(classname), wname, w, eater);
		if(parent)
		{
			ObjectTreeItem *pItem = container->form()->objectTree()->lookup(parent->name());
			if(pItem)
				container->form()->objectTree()->addChild(pItem, tree);
			else
				kdDebug() << "FORMIO :: ERROR no parent widget "  << endl;
		}
		else
			container->form()->objectTree()->addChild(container->tree(), tree);
	}
	else
		tree = container->form()->objectTree()->lookup(wname);

	if(el.parentNode().toElement().tagName() == "grid")
	{
		QGridLayout *layout = (QGridLayout*)container->layout();
		if(!layout)
			kdDebug() << "FormIO::ERROR:: the layout == 0" << endl;
		if(el.hasAttribute("rowspan"))
		{
			layout->addMultiCellWidget(w, el.attribute("row").toInt(), el.attribute("row").toInt() + el.attribute("rowspan").toInt()-1,
			 el.attribute("column").toInt(),  el.attribute("column").toInt() + el.attribute("colspan").toInt()-1);
			 tree->setGridPos(el.attribute("row").toInt(),  el.attribute("column").toInt(), el.attribute("rowspan").toInt(),
			   el.attribute("colspan").toInt());
		}
		else
		{
			layout->addWidget(w, el.attribute("row").toInt(), el.attribute("column").toInt());
			tree->setGridPos(el.attribute("row").toInt(),  el.attribute("column").toInt(), 0, 0);
		}
	}

	readChildNodes(tree, container, lib, el, w);

	if(el.tagName() == "spacer")
		Spacer::loadSpacer(w, el);
	w->show();
}

void
FormIO::createToplevelWidget(Form *form, QWidget *container, QDomElement &el)
{
	QString wname;
	//QWidget *w;
	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if((n.toElement().tagName() == "property") && (n.toElement().attribute("name") == "name"))
		{
			wname = n.toElement().text();
			break;
		}

	}

//	w = new QWidget(parent, wname.latin1());
//	form->createToplevel(w);
	container->setName(wname.latin1());
	if(form->objectTree())
		form->objectTree()->rename(form->objectTree()->name(), wname);
	form->setInteractiveMode(false);

	readChildNodes(form->objectTree(), form->toplevelContainer(), form->manager()->lib(), el, container);

	form->setInteractiveMode(true);
}

void
FormIO::readChildNodes(ObjectTreeItem *tree, Container *container, WidgetLibrary *lib, const QDomElement &el, QWidget *w)
{
	bool hasGeometryProp = false;
	QString eltag = el.tagName();

	for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QString tag = n.toElement().tagName();
		QDomElement node = n.toElement();

		if((tag == "property") || (tag == "attribute"))
		{
			QString name = node.attribute("name");
			if(name == "geometry")
				hasGeometryProp = true;
			if( ((eltag == "grid") || (eltag == "hbox") || (eltag == "vbox")) &&
			      (name == "name"))
				continue;

			if((name == "margin") && ((eltag == "grid") || (eltag == "hbox") || (eltag == "vbox")))
			{
				int margin = readProp(node.firstChild(), w, name).toInt();
				if(tree->container())
				{
					tree->container()->setLayoutMargin(margin);
					if(tree->container()->layout())
						tree->container()->layout()->setMargin(margin);
				}
			}
			else if((name == "spacing") && ((eltag == "grid") || (eltag == "hbox") || (eltag == "vbox")))
			{
				int spacing = readProp(node.firstChild(), w, name).toInt();
				if(tree->container())
				{
					tree->container()->setLayoutSpacing(spacing);
					if(tree->container()->layout())
						tree->container()->layout()->setSpacing(spacing);
				}
			}
			else if(w->metaObject()->findProperty(name.latin1(), true) == -1)
				lib->readSpecialProperty(w->className(), node, w, tree);
			else
			{
				QVariant val = readProp(node.firstChild(), w, name);
				w->setProperty(name.latin1(), val);
				tree->addModProperty(name, val);
			}
		}
		else if(tag == "widget")
		{
			if(tree->container())
				loadWidget(tree->container(), lib, node);
			else
				loadWidget(container, lib, node, w);
		}
		else if(tag == "spacer")
		{
			loadWidget(container, lib, node, w);
		}
		else if((tag == "vbox") || (tag == "hbox") || (tag == "grid"))
		{
			loadLayout(node, tree);
			readChildNodes(tree, container, lib, node, w);
			if(tag != "grid")
				tree->container()->reloadLayout();
		}
		else
			lib->readSpecialProperty(w->className(), node, w, tree);
	}

	if((!hasGeometryProp) && ((eltag == "widget") || (eltag == "spacer")))
	{
		QString parentTag = el.parentNode().toElement().tagName();
		kdDebug() << "Moving the widget" << w->name() << " by " << tree->parent()->children()->count() << endl;
		if(parentTag == "hbox")
			w->move(w->x() + tree->parent()->children()->count(), w->y());
		else if(parentTag == "vbox")
			w->move(w->x(), w->y() + tree->parent()->children()->count());
	}
}

void
FormIO::loadLayout(const QDomElement &el, ObjectTreeItem *tree)
{
	if(!tree->container())
		return;

	QString name = el.tagName();
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
		tree->container()->m_layType = Container::Grid;
		int nrow = 1, ncol = 1;
		for(QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
		{
			if(n.toElement().tagName() == "widget")
			{
				int wrow = n.toElement().attribute("row").toInt() + 1;
				if(wrow > nrow)
					nrow = wrow;

				int wcol = n.toElement().attribute("column").toInt() + 1;
				if(wcol > ncol)
					ncol = wcol;
			}
		}
		kdDebug() << "FormIO:: the loaded grid will have " << nrow << " rows and " << ncol << " cols." << endl;
		QGridLayout *layout = new QGridLayout(tree->widget(), nrow, ncol, 10, 2, "grid");
		tree->container()->m_layout = (QLayout*)layout;
	}
}

void
FormIO::addIncludeFile(const QString &include, QDomDocument &domDoc)
{
	if(include.isEmpty())
		return;

	QDomElement includes;
	QDomElement uiEl = domDoc.namedItem("UI").toElement();
	if(uiEl.namedItem("includehints").isNull())
	{
		includes = domDoc.createElement("includehints");
		uiEl.appendChild(includes);
	}
	else
		includes = uiEl.namedItem("includehints").toElement();

	// Check if this include has already been saved, and return if it is the case
	for(QDomNode n = includes.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().text() == include)
			return;
	}

	QDomElement includeHint = domDoc.createElement("includehint");
	includes.appendChild(includeHint);
	QDomText includeText = domDoc.createTextNode(include);
	includeHint.appendChild(includeText);
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
