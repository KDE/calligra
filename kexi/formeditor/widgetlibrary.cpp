/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
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

#include <qdom.h>

#include <kdebug.h>
#include <klocale.h>
#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>

#include "widgetfactory.h"
#include "widgetlibrary.h"
#include "libactionwidget.h"

namespace KFormDesigner {
WidgetLibrary::WidgetLibrary(QObject *parent)
 : QObject(parent)
{
	scan();
}

void
WidgetLibrary::addFactory(WidgetFactory *f)
{
	if(!f)
		return;

	WidgetList widgets = f->classes();
	for(Widget *w = widgets.first(); w; w = widgets.next())
	{
		kdDebug() << "WidgetLibrary::addFactory(): adding class " << w->className() << endl;
		m_widgets.insert(w->className(), w);
		if(!w->alternateClassName().isEmpty())
			m_alternates.insert(w->alternateClassName(), w->className());
	}
}

void
WidgetLibrary::scan()
{
	KTrader::OfferList tlist = KTrader::self()->query("KFormDesigner/WidgetFactory");
	KTrader::OfferList::Iterator it(tlist.begin());
	for(; it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		kdDebug() << "WidgetLibrary::scan(): found factory: " << ptr->name() << endl;
		//FIXME: check if this name matches the filter...

		WidgetFactory *f = KParts::ComponentFactory::createInstanceFromService<WidgetFactory>(ptr,
		 this, "wf", QStringList());
		addFactory(f);
	}
}

QString
WidgetLibrary::createXML()
{
	QDomDocument doc("kpartgui");
	QDomElement root = doc.createElement("kpartgui");

	root.setAttribute("name", "kformdesigner");
	root.setAttribute("version", "0.3");
	doc.appendChild(root);

	QDomElement toolbar = doc.createElement("ToolBar");
	toolbar.setAttribute("name", "widgets");
	root.appendChild(toolbar);

	QDomElement texttb = doc.createElement("text");
	toolbar.appendChild(texttb);
	QDomText ttext = doc.createTextNode("Widgets");
	texttb.appendChild(ttext);

	QDomElement menubar = doc.createElement("MenuBar");
	toolbar.setAttribute("name", "widgets");
	root.appendChild(menubar);

	QDomElement Mtextb = doc.createElement("text");
	toolbar.appendChild(Mtextb);
	QDomText Mtext = doc.createTextNode("Widgets");
	Mtextb.appendChild(Mtext);
	QDomElement menu = doc.createElement("Menu");
	menu.setAttribute("name", "widgets");

	QDictIterator<Widget> it(m_widgets);
	int i = 0;
	for(; it.current(); ++it)
	{

		QDomElement action = doc.createElement("Action");
		action.setAttribute("name", "library_widget" + it.current()->className());
		toolbar.appendChild(action);

		i++;
	}

	return doc.toString();
}

Actions
WidgetLibrary::createActions(KActionCollection *parent,  QObject *receiver, const char *slot)
{
	Actions actions;
	QDictIterator<Widget> it(m_widgets);
	for(; it.current(); ++it)
	{
		LibActionWidget *a = new LibActionWidget(*it, parent);
		kdDebug() << "WidgetLibrary::createActions(): action " << a << " added" << endl;
		connect(a, SIGNAL(prepareInsert(const QString &)), receiver, slot);
		actions.append(a);
	}

	return actions;
}

QWidget*
WidgetLibrary::createWidget(const QString &w, QWidget *parent, const char *name, Container *c)
{
	Widget *wfactory = m_widgets[w];
	kdDebug() << "WidgetLibrary::createWidget(): " << w << "  " << name << endl;
	if(!wfactory)
		return 0;

	return wfactory->factory()->create(w, parent, name, c);
}

bool
WidgetLibrary::createMenuActions(const QString &c, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container, QValueVector<int> *menuIds)
{
	Widget *wfactory = m_widgets[c];
	if(!wfactory)
		return false;

	return wfactory->factory()->createMenuActions(c, w, menu, container, menuIds);
}

void
WidgetLibrary::startEditing(const QString &classname, QWidget *w, Container *container)
{
	Widget *wfactory = m_widgets[classname];
	if(!wfactory)
		return ;

	wfactory->factory()->startEditing(classname, w, container);
}

void
WidgetLibrary::previewWidget(const QString &classname, QWidget *widget, Container *container)
{
	Widget *wfactory = m_widgets[classname];
	if(!wfactory)
		return ;

	wfactory->factory()->previewWidget(classname, widget, container);
}

QString
WidgetLibrary::displayName(const QString &classname)
{
	if(m_widgets.find(classname))
		return m_widgets[classname]->name();
	else
		return i18n("Form");
}

QString
WidgetLibrary::namePrefix(const QString &classname)
{
	if(m_widgets.find(classname))
		return m_widgets[classname]->namePrefix();
	else
		return i18n("Form");
}

QString
WidgetLibrary::textForWidgetName(const QString &name, const QString &className)
{
	Widget *widget = m_widgets[className];
	if(!widget)
		return QString::null;

	QString newName = name;
	newName.remove(widget->namePrefix());
	newName = widget->name() + " " + newName;
	return newName;

}

QString
WidgetLibrary::checkAlternateName(const QString &classname)
{
	if(m_widgets.find(classname))
		return classname;
	else if(m_alternates.contains(classname))
	{
		kdDebug() << "WidgetLibrary::alternateName() : The name " << classname << " will be replaced with " << m_alternates[classname] << endl;
		return m_alternates[classname];
	}
	else
		return classname;
}

QString
WidgetLibrary::includeFile(const QString &classname)
{
	if(m_widgets.find(classname))
		return m_widgets[classname]->includeFile();
	else
		return QString::null;
}

QString
WidgetLibrary::icon(const QString &classname)
{
	if(m_widgets.find(classname))
		return m_widgets[classname]->pixmap();
	else
		return QString("form");
}

void
WidgetLibrary::saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent)
{
	if(m_widgets.find(classname))
		m_widgets[classname]->factory()->saveSpecialProperty(classname, name, value, w, parentNode, parent);
	else
		return;
}

void
WidgetLibrary::readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, ObjectTreeItem *item)
{
	if(m_widgets.find(classname))
		m_widgets[classname]->factory()->readSpecialProperty(classname, node, w, item);
	else
		return;
}

bool
WidgetLibrary::showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple)
{
	if(m_widgets.find(classname))
		return m_widgets[classname]->factory()->showProperty(classname, w, property, multiple);
	else
		return true;
}

QStringList
WidgetLibrary::autoSaveProperties(const QString &classname)
{
	if(m_widgets.find(classname))
		return m_widgets[classname]->factory()->autoSaveProperties(classname);
	else
		return QStringList();
}

WidgetLibrary::~WidgetLibrary()
{
}
}

#include "widgetlibrary.moc"
