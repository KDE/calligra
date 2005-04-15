/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
		 , factories(101, false)
		 , factoriesLoaded(false)
		{
			services.setAutoDelete(true);
		}
		// dict which associates a class name with a Widget class
		WidgetInfo::Dict widgets;//, alternateWidgets;
		QAsciiDict<KService::Ptr> services;
		QAsciiDict<char> supportedFactoryGroups;
		QAsciiDict<WidgetFactory> factories;
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
WidgetLibrary::loadFactoryWidgets(WidgetFactory *f)
{
	const WidgetInfo::Dict widgets = f->classes();
	WidgetInfo *w;
	for(QAsciiDictIterator<WidgetInfo> it(widgets); (w = it.current()); ++it)
	{
		// check if we want to inherit a widget from a different factory
		if (!w->m_parentFactoryName.isEmpty() && !w->m_inheritedClassName.isEmpty()) {
			WidgetFactory *parentFactory = d->factories[w->m_parentFactoryName];
			if (!parentFactory) {
				kdWarning() << "WidgetLibrary::loadFactoryWidgets(): class '" << w->className()
					<< "' - no such parent factory '" << w->m_parentFactoryName << "'" << endl;
				continue;
			}
			WidgetInfo* inheritedClass = parentFactory->m_classesByName[ w->m_inheritedClassName ];
			if (!inheritedClass) {
				kdWarning() << "WidgetLibrary::loadFactoryWidgets(): class '" << w->m_inheritedClassName
					<< "' - no such class to inherit in factory '" << w->m_parentFactoryName << "'" << endl;
				continue;
			}
			//ok: inherit properties:
			w->m_inheritedClass = inheritedClass;
			if (w->pixmap().isEmpty())
				w->setPixmap( inheritedClass->pixmap() );
			//ok?
			foreach (QStringList::ConstIterator, it_alt, inheritedClass->m_alternateNames) {
				w->addAlternateClassName( *it_alt, inheritedClass->isOverriddenClassName( *it_alt ) );
			}
			if (w->includeFileName().isEmpty())
				w->setIncludeFileName( inheritedClass->includeFileName() );
			if (w->name().isEmpty())
				w->setName( inheritedClass->name() );
			if (w->namePrefix().isEmpty())
				w->setNamePrefix( inheritedClass->namePrefix() );
			if (w->description().isEmpty())
				w->setDescription( inheritedClass->description() );
		}

//		kdDebug() << "WidgetLibrary::addFactory(): adding class " << w->className() << endl;
		QStringList l = w->alternateClassNames();
		l.prepend( w->className() );
		//d->widgets.insert(w->className(), w);
//		if(!w->alternateClassName().isEmpty()) {
//			QStringList l = QStringList::split("|", w->alternateClassName());
		QStringList::ConstIterator endIt = l.constEnd();
		for(QStringList::ConstIterator it = l.constBegin(); it != endIt; ++it) {
			WidgetInfo *widgetForClass = d->widgets.find( (*it).local8Bit());
			if (!widgetForClass || (widgetForClass && !widgetForClass->isOverriddenClassName(*it))) {
				//insert a widgetinfo, if:
				//1) this class has no alternate class assigned yet, or
				//2) this class has alternate class assigned but without 'override' flag
				d->widgets.replace( (*it).local8Bit(), w);
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
			kdWarning() << "WidgetLibrary::loadFactories(): creating factory failed! "
				<< (*it.current())->library() << endl;
			continue;
		}
		d->factories.insert( f->name(), f );
	}

	//now we have factories instantiated: load widgets
	QPtrList<WidgetFactory> loadLater;
	for (QAsciiDictIterator<WidgetFactory> it(d->factories); it.current(); ++it) {
		//ONE LEVEL, FLAT INHERITANCE, but works!
		//if this factory inherits from something, load its witgets later
//! @todo improve
		if (it.current()->inheritsFactories())
			loadLater.append( it.current() );
		else
			loadFactoryWidgets(it.current());
	}
	//load now the rest
	for (QPtrListIterator<WidgetFactory> it(loadLater); it.current(); ++it) {
		loadFactoryWidgets(it.current());
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

	QAsciiDictIterator<WidgetInfo> it(d->widgets);
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
WidgetLibrary::addCreateWidgetActions(KActionCollection *parent,  QObject *receiver, const char *slot)
{
	loadFactories();
	ActionList actions;
	for (QAsciiDictIterator<WidgetInfo> it(d->widgets); it.current(); ++it)
	{
//		kdDebug() << "WidgetLibrary::createActions():" << it.current()->className() << endl;
		LibActionWidget *a = new LibActionWidget(it.current(), parent);
		connect(a, SIGNAL(prepareInsert(const QString &)), receiver, slot);
		actions.append(a);
	}
	return actions;
}

void
WidgetLibrary::addCustomWidgetActions(KActionCollection *parent)
{
	for (QAsciiDictIterator<WidgetFactory> it(d->factories); it.current(); ++it)
	{
		it.current()->createCustomActions( parent );
	}
}

QWidget*
WidgetLibrary::createWidget(const QCString &classname, QWidget *parent, const char *name, Container *c)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return 0;

	QWidget *widget = wclass->factory()->create(wclass->className().local8Bit(), parent, name, c);
	if (widget)
		return widget;
	//try to instantiate from inherited class
	if (wclass->inheritedClass())
		return wclass->inheritedClass()->factory()->create(wclass->className().local8Bit(), parent, name, c);
	return 0;
}

bool
WidgetLibrary::createMenuActions(const QCString &c, QWidget *w, QPopupMenu *menu, 
	KFormDesigner::Container *container)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[c];
	if(!wclass)
		return false;

	wclass->factory()->m_widget = w;
	wclass->factory()->m_container = container;
	if (wclass->factory()->createMenuActions(c, w, menu, container))
		return true;
	//try from inherited class
	if (wclass->inheritedClass())
		return wclass->inheritedClass()->factory()->createMenuActions(c, w, menu, container);
	return false;
}

bool
WidgetLibrary::startEditing(const QCString &classname, QWidget *w, Container *container)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return false;

	if (wclass->factory()->startEditing(classname, w, container))
		return true;
	//try from inherited class
	if (wclass->inheritedClass())
		return wclass->inheritedClass()->factory()->startEditing(classname, w, container);
	return false;
}

bool
WidgetLibrary::previewWidget(const QCString &classname, QWidget *widget, Container *container)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return false;

	if (wclass->factory()->previewWidget(classname, widget, container))
		return true;
	//try from inherited class
	if (wclass->inheritedClass())
		return wclass->inheritedClass()->factory()->previewWidget(classname, widget, container);
	return false;
}

bool
WidgetLibrary::clearWidgetContent(const QCString &classname, QWidget *w)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return false;

	if (wclass->factory()->clearWidgetContent(classname, w))
		return true;
	//try from inherited class
	if (wclass->inheritedClass())
		return wclass->inheritedClass()->factory()->clearWidgetContent(classname, w);
	return false;
}

QString
WidgetLibrary::displayName(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->name();

	return classname;
}

QString
WidgetLibrary::savingName(const QCString &classname)
{
	loadFactories();
	QString s;
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi && !wi->savingName().isEmpty())
		return wi->savingName();

	return classname;
}

QString
WidgetLibrary::namePrefix(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->namePrefix();

	return i18n("Form");
}

QString
WidgetLibrary::textForWidgetName(const QCString &name, const QCString &className)
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

QCString
WidgetLibrary::checkAlternateName(const QCString &classname)
{
	loadFactories();
	if(d->widgets.find(classname))
		return classname;

//	WidgetInfo *wi =  d->alternateWidgets[classname];
	WidgetInfo *wi =  d->widgets[classname];
	if (wi) {
//		kdDebug() << "WidgetLibrary::alternateName() : The name " << classname << " will be replaced with " << wi->className() << endl;
		return wi->className().local8Bit();
	}

	// widget not supported
	return QCString("CustomWidget");
}

QString
WidgetLibrary::includeFileName(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->includeFileName();

	return QString::null;
}

QString
WidgetLibrary::icon(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->pixmap();

	return QString("form");
}

bool
WidgetLibrary::saveSpecialProperty(const QCString &classname, const QString &name, const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if (!wi)
		return false;

	if (wi->factory()->saveSpecialProperty(classname, name, value, w, parentNode, parent))
		return true;
	//try from inherited class
	if (wi->inheritedClass())
		return wi->inheritedClass()->factory()->saveSpecialProperty(classname, name, value, w, parentNode, parent);
	return false;
}

bool
WidgetLibrary::readSpecialProperty(const QCString &classname, QDomElement &node, QWidget *w, ObjectTreeItem *item)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if (!wi)
		return false;
	if (wi->factory()->readSpecialProperty(classname, node, w, item))
		return true;
	//try from inherited class
	if (wi->inheritedClass())
		return wi->inheritedClass()->factory()->readSpecialProperty(classname, node, w, item);
	return false;
}

bool
WidgetLibrary::showProperty(const QCString &classname, QWidget *w, const QString &property, bool multiple)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if (!wi)
		return false;
	if (wi->factory()->showProperty(classname, w, property, multiple))
		return true;
	//try from inherited class
	if (wi->inheritedClass())
		return wi->inheritedClass()->factory()->showProperty(classname, w, property, multiple);
	return false;
}

QStringList
WidgetLibrary::autoSaveProperties(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(!wi)
		return QStringList();
	QStringList lst;
	//prepend from inherited class
	if (wi->inheritedClass())
		lst = wi->inheritedClass()->factory()->autoSaveProperties(classname);
	lst += wi->factory()->autoSaveProperties(classname);
	return lst;
}

WidgetInfo*
WidgetLibrary::widgetInfoForClassName(const char* classname)
{
	loadFactories();
	return d->widgets.find(classname);
}

WidgetFactory*
WidgetLibrary::factoryForClassName(const char* classname)
{
	WidgetInfo *wi = widgetInfoForClassName(classname);
	return wi ? wi->factory() : 0;
}

#include "widgetlibrary.moc"
