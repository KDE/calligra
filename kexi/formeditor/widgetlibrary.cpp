/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
class WidgetLibraryPrivate
{
	public:
		WidgetLibraryPrivate()
		 : widgets(101)
//		 , alternateWidgets(101)
		 , services(101, false)
		 , supportedFactoryGroups(17, false)
		 , factoriesLoaded(false)
		{
			services.setAutoDelete(true);
		}
		// dict which associates a class name with a Widget class
		WidgetInfo::Dict widgets;//, alternateWidgets;
		QAsciiDict<KService::Ptr> services;
		QAsciiDict<char> supportedFactoryGroups;
		bool factoriesLoaded : 1;
};
}

using namespace KFormDesigner;

//-------------------------------------------

WidgetLibrary::WidgetLibrary(QObject *parent, const QStringList& supportedFactoryGroups)
 : QObject(parent)
 , d(new WidgetLibraryPrivate())
{
	for (QStringList::ConstIterator it = supportedFactoryGroups.constBegin();
		it!=supportedFactoryGroups.constEnd(); ++it)
	{
		d->supportedFactoryGroups.insert( (*it).lower().latin1(), (char*)1);
	}
	lookupFactories();
}

WidgetLibrary::~WidgetLibrary()
{
	delete d;
}

void
WidgetLibrary::addFactory(WidgetFactory *f)
{
	if(!f)
		return;

	WidgetInfo::List widgets = f->classes();
	for(WidgetInfo *w = widgets.first(); w; w = widgets.next())
	{
//		kdDebug() << "WidgetLibrary::addFactory(): adding class " << w->className() << endl;
		QStringList l = w->alternateClassNames();
		l.prepend( w->className() );
		//d->widgets.insert(w->className(), w);
//		if(!w->alternateClassName().isEmpty()) {
//			QStringList l = QStringList::split("|", w->alternateClassName());
		QStringList::ConstIterator endIt = l.constEnd();
		for(QStringList::ConstIterator it = l.constBegin(); it != endIt; ++it) {
			WidgetInfo *widgetForClass = d->widgets.find(*it);
			if (!widgetForClass || (widgetForClass && !widgetForClass->isOverriddenClassName(*it))) {
				//insert a widgetinfo, if:
				//1) this class has no alternate class assigned yet, or
				//2) this class has alternate class assigned but without 'override' flag
				d->widgets.replace(*it, w);
			}

/*			WidgetInfo *widgetForClass = d->alternateWidgets.find(*it);
			if (!widgetForClass || (widgetForClass && !widgetForClass->isOverriddenClassName(*it))) {
				//insert a widgetinfo, if:
				//1) this class has no alternate class assigned yet, or
				//2) this class has alternate class assigned but without 'override' flag
				d->alternateWidgets.replace(*it, w);
			}*/
		}
	}
}

void
WidgetLibrary::lookupFactories()
{
	KTrader::OfferList tlist = KTrader::self()->query("KFormDesigner/WidgetFactory");
	KTrader::OfferList::Iterator it(tlist.begin());
	for(; it != tlist.end(); ++it)
	{
		KService::Ptr ptr = (*it);
		KService::Ptr* existingService = (d->services)[ptr->library().local8Bit()];
		if (existingService) {
			kdWarning() << "WidgetLibrary::scan(): factory '" << ptr->name() 
				<< "' already found (library="<< (*existingService)->library() 
				<<")! skipping this one: library=" << ptr->library() << endl;
			continue;
		}
		kdDebug() << "WidgetLibrary::scan(): found factory: " << ptr->name() << endl;

		QCString groupName = ptr->property("X-KFormDesigner-FactoryGroup").toCString();
		if (!groupName.isEmpty() && !d->supportedFactoryGroups[groupName]) {
			kdDebug() << "WidgetLibrary::scan(): factory group '" << groupName
				<< "' is unsupported by this application (library=" << ptr->library() << ")"<< endl;
			continue;
		}
		//FIXME: check if this name matches the filter...
		d->services.insert(ptr->library().local8Bit(), new KService::Ptr( ptr ));
	}
}

void
WidgetLibrary::loadFactories()
{
	if (d->factoriesLoaded)
		return;
	d->factoriesLoaded = true;
	for (QAsciiDictIterator<KService::Ptr> it(d->services); it.current(); ++it) {
		WidgetFactory *f = KParts::ComponentFactory::createInstanceFromService<WidgetFactory>(
			*it.current(), this, (*it.current())->library().latin1(), QStringList());
		if (!f) {
			kdWarning() << "WidgetLibrary::scan(): creating factory failed!" 
				<< (*it.current())->library() << endl;
			continue;
		}
		addFactory(f);
	}
}

QString
WidgetLibrary::createXML()
{
	loadFactories();

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

	QDictIterator<WidgetInfo> it(d->widgets);
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

ActionList
WidgetLibrary::createActions(KActionCollection *parent,  QObject *receiver, const char *slot)
{
	loadFactories();
	ActionList actions;
	QDictIterator<WidgetInfo> it(d->widgets);
	for(; it.current(); ++it)
	{
		LibActionWidget *a = new LibActionWidget(*it, parent);
//		kdDebug() << "WidgetLibrary::createActions(): action " << a << " added" << endl;
		connect(a, SIGNAL(prepareInsert(const QString &)), receiver, slot);
		actions.append(a);
	}

	return actions;
}

QWidget*
WidgetLibrary::createWidget(const QString &w, QWidget *parent, const char *name, Container *c)
{
	loadFactories();
	WidgetInfo *wfactory = d->widgets[w];
//	kdDebug() << "WidgetLibrary::createWidget(): " << w << "  " << name << endl;
	if(!wfactory)
		return 0;

	return wfactory->factory()->create(w, parent, name, c);
}

bool
WidgetLibrary::createMenuActions(const QString &c, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container, QValueVector<int> *menuIds)
{
	loadFactories();
	WidgetInfo *wfactory = d->widgets[c];
	if(!wfactory)
		return false;

	return wfactory->factory()->createMenuActions(c, w, menu, container, menuIds);
}

void
WidgetLibrary::startEditing(const QString &classname, QWidget *w, Container *container)
{
	loadFactories();
	WidgetInfo *wfactory = d->widgets[classname];
	if(!wfactory)
		return ;

	wfactory->factory()->startEditing(classname, w, container);
}

void
WidgetLibrary::previewWidget(const QString &classname, QWidget *widget, Container *container)
{
	loadFactories();
	WidgetInfo *wfactory = d->widgets[classname];
	if(!wfactory)
		return ;

	wfactory->factory()->previewWidget(classname, widget, container);
}

void
WidgetLibrary::clearWidgetContent(const QString &classname, QWidget *w)
{
	loadFactories();
	WidgetInfo *wfactory = d->widgets[classname];
	if(!wfactory)
		return ;

	wfactory->factory()->clearWidgetContent(classname, w);
}

QString
WidgetLibrary::displayName(const QString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->name();
	else
		return classname;
}

QString
WidgetLibrary::savingName(const QString &classname)
{
	loadFactories();
	QString s;
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi && !wi->savingName().isEmpty())
		return wi->savingName();
	else
		return classname;
}

QString
WidgetLibrary::namePrefix(const QString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->namePrefix();
	else
		return i18n("Form");
}

QString
WidgetLibrary::textForWidgetName(const QString &name, const QString &className)
{
	loadFactories();
	WidgetInfo *widget = d->widgets[className];
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
	loadFactories();
	if(d->widgets.find(classname))
		return classname;

//	WidgetInfo *wi =  d->alternateWidgets[classname];
	WidgetInfo *wi =  d->widgets[classname];
	if (wi) {
//		kdDebug() << "WidgetLibrary::alternateName() : The name " << classname << " will be replaced with " << wi->className() << endl;
		return wi->className();
	}

	// widget not supported
	return QString("CustomWidget");
}

QString
WidgetLibrary::includeFileName(const QString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->includeFileName();
	else
		return QString::null;
}

QString
WidgetLibrary::icon(const QString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->pixmap();
	else
		return QString("form");
}

void
WidgetLibrary::saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		wi->factory()->saveSpecialProperty(classname, name, value, w, parentNode, parent);
	else
		return;
}

bool
WidgetLibrary::readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, ObjectTreeItem *item)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->factory()->readSpecialProperty(classname, node, w, item);
	else
		return false;
}

bool
WidgetLibrary::showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->factory()->showProperty(classname, w, property, multiple);
	else
		return true;
}

QStringList
WidgetLibrary::autoSaveProperties(const QString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->factory()->autoSaveProperties(classname);
	else
		return QStringList();
}

#include "widgetlibrary.moc"
