/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qdom.h>

#include <kdebug.h>

#include "propertybuffer.h"
#include "eventbuffer.h"
#include "widgetwatcher.h"
#include "widgetcontainer.h"
#include "widgetprovider.h"

namespace KFormEditor {

WidgetWatcher::WidgetWatcher(QObject *parent, PropertyBuffer *b, EventBuffer *eb, const char *name)
 : QObject(parent, name),
   QMap<QString, QObject *>()
{
	if(b)
	{
		connect(b, SIGNAL(nameChanged(QObject *, const char *)),
		 this, SLOT(slotNameChanged(QObject *, const char *)));

		m_buffer = b;
	}

	m_eb = eb;
}

QString
WidgetWatcher::genName(const QString &base)
{
		int count = m_nameCounter[base];
		m_nameCounter.insert(base, count + 1);
		return QString(base) + QString::number(count + 1);
}

QString
WidgetWatcher::genName(QObject *o)
{
	return genName(o->className());
}

void
WidgetWatcher::slotNameChanged(QObject *o, const char *newN)
{
	kdDebug() << "WidgetWatcher::slotNameChanged() " << o << "=>" << newN << endl;
//	if(property == name)
}

QByteArray
WidgetWatcher::store(WidgetContainer *parentC)
{
	kdDebug() << "WidgetWatcher::store(): " << m_buffer->count() << " items" << endl;


	QDomDocument domDoc("UI");
        QDomElement uiElement = domDoc.createElement("UI");
	uiElement.setAttribute("version", "3.1");
	uiElement.setAttribute("stdsetdef", 1);

	QDomElement baseClass = domDoc.createElement("class");
	uiElement.appendChild(baseClass);
	QDomText baseClassV = domDoc.createTextNode("QWidget");
	baseClass.appendChild(baseClassV);

	QDomElement parent = domDoc.createElement("widget");
	parent.setAttribute("class", "QWidget");
	uiElement.appendChild(parent);

	parent.appendChild(prop(&domDoc, "name", QVariant(parentC->className())));
	parent.appendChild(prop(&domDoc, "geometry", parentC->property("geometry")));
	parent.appendChild(prop(&domDoc, "dataSource", parentC->property("dataSource")));


	for(QMapIterator<QString, QObject *> it = begin(); it != end(); it++)
	{
		QDomElement tclass = domDoc.createElement("widget");
		tclass.setAttribute("class", it.data()->className());
		tclass.appendChild(prop(&domDoc, "name", QVariant(it.data()->property("name"))));
		tclass.appendChild(prop(&domDoc, "geometry", QVariant(it.data()->property("geometry"))));

		PropertyBufferItem *i;
		for(i = m_buffer->first(); i; i = m_buffer->next())
		{
			kdDebug() << "WidgetWatcher::store(): property: " << i->name() << endl;
			if(i->object() == it.data())
			{
				if(i->name() != "name" && i->name() != "geometry")
					tclass.appendChild(prop(&domDoc, i->name(), i->value()));
			}
		}

		parent.appendChild(tclass);
	}

	QDomElement connections = domDoc.createElement("connections");
	uiElement.appendChild(connections);

	EventBufferItem *it;
	for(it = m_eb->first(); it; it = m_eb->next())
	{
		QDomElement connection = domDoc.createElement("connection");
		connection.setAttribute("fake", "true");
		connections.appendChild(connection);

		QDomElement csender = domDoc.createElement("sender");
		QDomText tsender= domDoc.createTextNode(it->sender());
		csender.appendChild(tsender);

		QDomElement csignal = domDoc.createElement("signal");
		QDomText tsignal= domDoc.createTextNode(it->signal());
		csignal.appendChild(tsignal);

		QDomElement creceiver = domDoc.createElement("receiver");
		QDomText treceiver= domDoc.createTextNode(it->receiver());
		creceiver.appendChild(treceiver);

		QDomElement cslot = domDoc.createElement("slot");
		QDomText tslot = domDoc.createTextNode(it->slot());
		cslot.appendChild(tslot);

		connection.appendChild(csender);
		connection.appendChild(csignal);
		connection.appendChild(creceiver);
		connection.appendChild(cslot);
	}

	domDoc.appendChild(uiElement);
	QByteArray data = domDoc.toCString();
	data.resize(data.size() - 1);

	return data;
}

QDomElement
WidgetWatcher::prop(QDomDocument *parent, const QString &name, const QVariant &value)
{
	QDomElement propertyE = parent->createElement("property");
	propertyE.setAttribute("name", name);

	QDomElement type;
	QDomText valueE;

	switch(value.type())
	{
		case QVariant::String:
			type = parent->createElement("string");
			valueE = parent->createTextNode(value.toString());
			type.appendChild(valueE);
			break;

		case QVariant::CString:
			type = parent->createElement("cstring");
			valueE = parent->createTextNode(value.toString());
			type.appendChild(valueE);
			break;

		case QVariant::Rect:
		{
			type = parent->createElement("rect");
			QDomElement x = parent->createElement("x");
			QDomElement y = parent->createElement("y");
			QDomElement w = parent->createElement("width");
			QDomElement h = parent->createElement("height");
			QDomText valueX = parent->createTextNode(QString::number(value.toRect().x()));
			QDomText valueY = parent->createTextNode(QString::number(value.toRect().y()));
			QDomText valueW = parent->createTextNode(QString::number(value.toRect().width()));
			QDomText valueH = parent->createTextNode(QString::number(value.toRect().height()));

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
		default:
			break;
	}

	propertyE.appendChild(type);
	return propertyE;

//	QDomElement type
}

void
WidgetWatcher::load(WidgetContainer *p, WidgetProvider *wp, const QByteArray &data)
{
	kdDebug() << "WidgetWatcher::load(): loading... " << data.size() << " bytes" << endl;
	QString errMsg;
	int errLine;
	int errCol;

	QDomDocument inBuf;
	bool parsed = inBuf.setContent(data, false, &errMsg, &errLine, &errCol);

	if(!parsed)
	{
		kdDebug() << "WidgetWatcher::load(): " << errMsg << endl;
		kdDebug() << "WidgetWatcher::load(): line: " << errLine << " col: " << errCol << endl;
		return;
	}

	QDomElement ui = inBuf.namedItem("UI").toElement();
	QDomElement element = ui.namedItem("widget").toElement();
	setUpWidget(p, wp, element, true);
	for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement w = n.namedItem("widget").toElement();
		kdDebug() << "WidgetWatcher::load(): tag " << n.toElement().tagName() << endl;
		kdDebug() << "WidgetWatcher::load(): attr " << n.toElement().attribute("class", "<NONE>") << endl;

		kdDebug() << "WidgetWatcher::load(): finding details..." << endl;
		setUpWidget(p, wp, n.toElement());
	}

	QDomElement connections = ui.namedItem("connections").toElement();
	for(QDomNode n = connections.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement connection = n.toElement();

		QString sender   = connection.namedItem("sender").toElement().text();
		QString signal   = connection.namedItem("signal").toElement().text();
		QString receiver = connection.namedItem("receiver").toElement().text();
		QString slot     = connection.namedItem("slot").toElement().text();

		kdDebug() << "WidgetWatcher::load(): tag =" << connection.namedItem("sender").toElement().tagName() << endl;
		kdDebug() << "WidgetWatcher::load(): sender =" << sender << endl;

		QObject *o = find(sender).data();
		kdDebug() << "WidgetWatcher::load(): count: " << count() << endl;
		EventBufferItem *eb = new EventBufferItem(sender, receiver, signal, slot, true, o);
		m_eb->insertEvent(eb);
	}
}

void
WidgetWatcher::setUpWidget(WidgetContainer *p, WidgetProvider *w, const QDomElement &d, bool po)
{
	QWidget *item = w->create(d.attribute("class"), p, "null");
	kdDebug() << "WidgetWatcher::setUpWidget(): item: " << item << endl;

	if(po)
		item = p;

	if(!item)
		return;

	p->addInteractive(item);

	for(QDomNode n = d.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		if(n.toElement().tagName() == "property")
		{
			QString name = n.toElement().attribute("name");
			kdDebug() << "WidgetWatcher::setUpWidget(): pname: " << name << endl;

			QDomNode sub = n.toElement().toElement().firstChild();
			QDomElement tag = sub.toElement();
			QString type = sub.toElement().tagName();

			if(type == "string")
			{
				m_buffer->changeProperty(item, name.latin1(), QVariant(tag.text()));
				item->setProperty(name.latin1(), QVariant(tag.text()));
			}
			else if(type == "cstring")
			{
				m_buffer->changeProperty(item, name.latin1(), QVariant(tag.text()));
				item->setProperty(name.latin1(), QVariant(tag.text()));
			}
			else if(type == "rect")
			{
				QDomElement x = sub.namedItem("x").toElement();
				QDomElement y = sub.namedItem("y").toElement();
				QDomElement w = sub.namedItem("width").toElement();
				QDomElement h = sub.namedItem("height").toElement();

				int rx = x.text().toInt();
				int ry = y.text().toInt();
				int rw = w.text().toInt();
				int rh = h.text().toInt();

				QRect rect(rx, ry, rw, rh);
				m_buffer->changeProperty(item, name.latin1(), QVariant(rect));
				item->setProperty(name.latin1(), QVariant(rect));
			}
			else
			{
				return;
			}
		}
	}

	char *iname = item->name();
	insert(iname, item);
}

WidgetWatcher::~WidgetWatcher()
{
}

}

#include "widgetwatcher.moc"

