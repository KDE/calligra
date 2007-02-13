/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <qstrlist.h>

#include <kdebug.h>
#include <klocale.h>
#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <ktrader.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

#include "widgetfactory.h"
#include "widgetlibrary.h"
#include "libactionwidget.h"
#include "container.h"
#include "form.h"
#include "formIO.h"

namespace KFormDesigner {

//! @internal
class XMLGUIClient : public QObject, public KXMLGUIClient
{
	public:
		XMLGUIClient(KXMLGUIClient* parent, const QString& xmlFileName)
		 : QObject(parent->actionCollection()), KXMLGUIClient(parent)
		{
			setXMLFile( xmlFileName, true /*merge*/ );
		}
};

//! @internal
class WidgetLibraryPrivate
{
	public:
		WidgetLibraryPrivate()
		 : widgets(101)
//		 , alternateWidgets(101)
		 , services(101, false)
		 , supportedFactoryGroups(17, false)
		 , factories(101, false)
		 , advancedProperties(1009, true)
		 , hiddenClasses(101, true)
		 , showAdvancedProperties(true)
		 , factoriesLoaded(false)
		{
			services.setAutoDelete(true);
			advancedProperties.insert("autoMask", (char*)1);
			advancedProperties.insert("baseSize", (char*)1);
			advancedProperties.insert("mouseTracking", (char*)1);
			advancedProperties.insert("acceptDrops", (char*)1);
			advancedProperties.insert("cursorPosition", (char*)1);
			advancedProperties.insert("contextMenuEnabled", (char*)1);
			advancedProperties.insert("trapEnterKeyEvent", (char*)1);
			advancedProperties.insert("dragEnabled", (char*)1);
			advancedProperties.insert("enableSqueezedText", (char*)1);
			advancedProperties.insert("sizeIncrement", (char*)1);
/*! @todo: reenable */ advancedProperties.insert("palette", (char*)1);
			advancedProperties.insert("backgroundOrigin", (char*)1);
			advancedProperties.insert("backgroundMode", (char*)1);//this is rather useless
			advancedProperties.insert("layout", (char*)1);// too large risk to break things
			                                              // by providing this in propeditor
			advancedProperties.insert("minimumSize", (char*)1);
			advancedProperties.insert("maximumSize", (char*)1);
#ifdef KEXI_NO_UNFINISHED
/*! @todo reenable */
			advancedProperties.insert("paletteBackgroundPixmap", (char*)1);
			advancedProperties.insert("icon", (char*)1);
			advancedProperties.insert("pixmap", (char*)1);
			advancedProperties.insert("accel", (char*)1);
#endif
		}
		// dict which associates a class name with a Widget class
		WidgetInfo::Dict widgets;//, alternateWidgets;
		QAsciiDict<KService::Ptr> services;
		QAsciiDict<char> supportedFactoryGroups;
		QAsciiDict<WidgetFactory> factories;
		QAsciiDict<char> advancedProperties;
		QAsciiDict<char> hiddenClasses;
		bool showAdvancedProperties : 1;
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
		if (0 != d->hiddenClasses[ w->className() ])
			continue; //this class is hidden
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
			foreach (QValueList<QCString>::ConstIterator, it_alt, inheritedClass->m_alternateNames) {
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
		QValueList<QCString> l = w->alternateClassNames();
		l.prepend( w->className() );
		//d->widgets.insert(w->className(), w);
//		if(!w->alternateClassName().isEmpty()) {
//			QStringList l = QStringList::split("|", w->alternateClassName());
		QValueList<QCString>::ConstIterator endIt = l.constEnd();
		for(QValueList<QCString>::ConstIterator it = l.constBegin(); it != endIt; ++it) {
			WidgetInfo *widgetForClass = d->widgets.find( *it );
			if (!widgetForClass || (widgetForClass && !widgetForClass->isOverriddenClassName(*it))) {
				//insert a widgetinfo, if:
				//1) this class has no alternate class assigned yet, or
				//2) this class has alternate class assigned but without 'override' flag
				d->widgets.replace( *it, w);
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
	KTrader::OfferList::ConstIterator it, end( tlist.constEnd() );
	for( it = tlist.constBegin(); it != end; ++it)
	{
		KService::Ptr ptr = (*it);
		KService::Ptr* existingService = (d->services)[ptr->library().latin1()];
		if (existingService) {
			kdWarning() << "WidgetLibrary::lookupFactories(): factory '" << ptr->name()
				<< "' already found (library="<< (*existingService)->library()
				<<")! skipping this one: library=" << ptr->library() << endl;
			continue;
		}
		kdDebug() << "WidgetLibrary::lookupFactories(): found factory: " << ptr->name() << endl;

		QCString groupName = ptr->property("X-KFormDesigner-FactoryGroup").toCString();
		if (!groupName.isEmpty() && !d->supportedFactoryGroups[groupName]) {
			kdDebug() << "WidgetLibrary::lookupFactories(): factory group '" << groupName
				<< "' is unsupported by this application (library=" << ptr->library() << ")"<< endl;
			continue;
		}
		const uint factoryVersion = ptr->property("X-KFormDesigner-WidgetFactoryVersion").toUInt();
		if (KFormDesigner::version()!=factoryVersion) {
			kdWarning() << QString("WidgetLibrary::lookupFactories(): factory '%1'" 
				" has version '%2' but required Widget Factory version is '%3'\n"
				" -- skipping this factory!").arg(ptr->library()).arg(factoryVersion)
				.arg(KFormDesigner::version()) << endl;
			continue;
		}
		d->services.insert(ptr->library().latin1(), new KService::Ptr( ptr ));
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
		f->m_library = this;
		f->m_showAdvancedProperties = d->showAdvancedProperties; //inherit this flag from the library
		f->m_xmlGUIFileName = (*it.current())->property("X-KFormDesigner-XMLGUIFileName").toString();
		d->factories.insert( f->name(), f );

		//collect information about classes to be hidden
		if (f->m_hiddenClasses) {
			for (QAsciiDictIterator<char> it2(*f->m_hiddenClasses); it2.current(); ++it2) {
				d->hiddenClasses.replace( it2.currentKey(), (char*)1 );
			}
		}
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

/* old
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
}*/

ActionList
WidgetLibrary::createWidgetActions(KXMLGUIClient* client, KActionCollection *parent, 
	QObject *receiver, const char *slot)
{
	loadFactories();

	// init XML gui clients (custom factories have their own .rc files)
	for (QAsciiDictIterator<WidgetFactory> it(d->factories); it.current(); ++it)
	{
		if (it.current()->m_xmlGUIFileName.isEmpty()) { // probably a built-in factory, with GUI file like kexiformpartinstui.rc
			it.current()->m_guiClient = 0;
		}
		else { // a custom factory with its own .rc file
			it.current()->m_guiClient = new XMLGUIClient(client, it.current()->m_xmlGUIFileName);
		}
	}

	ActionList actions;
	for (QAsciiDictIterator<WidgetInfo> it(d->widgets); it.current(); ++it)
	{
		LibActionWidget *a = new LibActionWidget(it.current(), 
			it.current()->factory()->m_guiClient 
			? it.current()->factory()->m_guiClient->actionCollection() : parent);
		connect(a, SIGNAL(prepareInsert(const QCString &)), receiver, slot);
		actions.append(a);
	}
	return actions;
}

void
WidgetLibrary::addCustomWidgetActions(KActionCollection *col)
{
	for (QAsciiDictIterator<WidgetFactory> it(d->factories); it.current(); ++it)
	{
		it.current()->createCustomActions(
			it.current()->m_guiClient 
			? it.current()->m_guiClient->actionCollection() : col);
	}
}

QWidget*
WidgetLibrary::createWidget(const QCString &classname, QWidget *parent, const char *name, Container *c,
	int options)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return 0;

	QWidget *widget = wclass->factory()->createWidget(wclass->className(), parent, name, c, options);
	if (!widget) {
		//try to instantiate from inherited class
		if (wclass->inheritedClass())
			widget = wclass->inheritedClass()->factory()->createWidget(
				wclass->className(), parent, name, c, options);
		if (!widget)
			return 0;
	}
	widget->setAcceptDrops(true);
	emit widgetCreated(widget);
	return widget;
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
		return wclass->inheritedClass()->factory()
			->createMenuActions(wclass->className(), w, menu, container);
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
		return wclass->inheritedClass()->factory()->startEditing(wclass->className(), w, container);
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
		return wclass->inheritedClass()->factory()->previewWidget(wclass->className(), widget, container);
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
		return wclass->inheritedClass()->factory()->clearWidgetContent(wclass->className(), w);
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

	return classname;
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
WidgetLibrary::classNameForAlternate(const QCString &classname)
{
	loadFactories();
	if(d->widgets.find(classname))
		return classname;

	WidgetInfo *wi =  d->widgets[classname];
	if (wi) {
		return wi->className();
	}

	// widget not supported
	return "CustomWidget";
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
WidgetLibrary::iconName(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(wi)
		return wi->pixmap();

	return QString::fromLatin1("unknown_widget");
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
		return wi->inheritedClass()->factory()->saveSpecialProperty(wi->className(), name, value, w, parentNode, parent);
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
		return wi->inheritedClass()->factory()->readSpecialProperty(wi->className(), node, w, item);
	return false;
}

void WidgetLibrary::setAdvancedPropertiesVisible(bool set)
{
	d->showAdvancedProperties = set;
}

bool WidgetLibrary::advancedPropertiesVisible() const
{
	return d->showAdvancedProperties;
}

bool
WidgetLibrary::isPropertyVisible(const QCString &classname, QWidget *w,
	const QCString &property, bool multiple, bool isTopLevel)
{
	if (isTopLevel) {
		// no focus policy for top-level form widget...
		if (!d->showAdvancedProperties && property == "focusPolicy")
			return false;
	}

	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if (!wi)
		return false;
	if (!d->showAdvancedProperties && d->advancedProperties[ property ]) {
		//this is advanced property, should we hide it?
		if (wi->factory()->internalProperty(classname, "forceShowAdvancedProperty:"+property).isEmpty()
			&& (!wi->inheritedClass() || wi->inheritedClass()->factory()->internalProperty(classname, "forceShowAdvancedProperty:"+property).isEmpty()))
		{
			return false; //hide it
		}
	}

	if (!wi->factory()->isPropertyVisible(classname, w, property, multiple, isTopLevel))
		return false;
	//try from inherited class
	if (wi->inheritedClass()
		&& !wi->inheritedClass()->factory()->isPropertyVisible(wi->className(), w, property, multiple, isTopLevel))
		return false;

	return true;
}

QValueList<QCString>
WidgetLibrary::autoSaveProperties(const QCString &classname)
{
	loadFactories();
	WidgetInfo *wi = d->widgets.find(classname);
	if(!wi)
		return QValueList<QCString>();
	QValueList<QCString> lst;
	//prepend from inherited class
	if (wi->inheritedClass())
		lst = wi->inheritedClass()->factory()->autoSaveProperties(wi->className());
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

QString WidgetLibrary::propertyDescForName(WidgetInfo *winfo, const QCString& propertyName)
{
	if (!winfo || !winfo->factory())
		return QString::null;
	QString desc( winfo->factory()->propertyDescForName(propertyName) );
	if (!desc.isEmpty())
		return desc;
	if (winfo->m_parentFactoryName.isEmpty())
		return QString::null;

	//try in parent factory, if exists
	WidgetFactory *parentFactory = d->factories[winfo->m_parentFactoryName];
	if (!parentFactory)
		return QString::null;

	return parentFactory->propertyDescForName(propertyName);
}

QString WidgetLibrary::propertyDescForValue(WidgetInfo *winfo, const QCString& name)
{
	if (!winfo->factory())
		return QString::null;
	QString desc( winfo->factory()->propertyDescForValue(name) );
	if (!desc.isEmpty())
		return desc;
	if (winfo->m_parentFactoryName.isEmpty())
		return QString::null;

	//try in parent factory, if exists
	WidgetFactory *parentFactory = d->factories[winfo->m_parentFactoryName];
	if (!parentFactory)
		return QString::null;

	return parentFactory->propertyDescForValue(name);
}

void WidgetLibrary::setPropertyOptions( WidgetPropertySet& buf, const WidgetInfo& winfo, QWidget* w )
{
	if (!winfo.factory())
		return;
	winfo.factory()->setPropertyOptions(buf, winfo, w);
	if (winfo.m_parentFactoryName.isEmpty())
		return;
	WidgetFactory *parentFactory = d->factories[winfo.m_parentFactoryName];
	if (!parentFactory)
		return;
	parentFactory->setPropertyOptions(buf, winfo, w);
}

WidgetFactory* WidgetLibrary::factory(const char* factoryName) const
{
	return d->factories[factoryName];
}

QString WidgetLibrary::internalProperty(const QCString& classname, const QCString& property)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return QString::null;
	QString value( wclass->factory()->internalProperty(classname, property) );
	if (value.isEmpty() && wclass->inheritedClass())
		return wclass->inheritedClass()->factory()->internalProperty(classname, property);
	return value;
}

WidgetFactory::CreateWidgetOptions WidgetLibrary::showOrientationSelectionPopup(
	const QCString &classname, QWidget* parent, const QPoint& pos)
{
	loadFactories();
	WidgetInfo *wclass = d->widgets[classname];
	if(!wclass)
		return WidgetFactory::AnyOrientation;

	//get custom icons and strings
	QPixmap iconHorizontal, iconVertical;
	QString iconName( wclass->factory()->internalProperty(classname, "orientationSelectionPopup:horizontalIcon") );
	if (iconName.isEmpty() && wclass->inheritedClass())
		iconName = wclass->inheritedClass()->factory()->internalProperty(classname, "orientationSelectionPopup:horizontalIcon");
	if (!iconName.isEmpty())
		iconHorizontal = SmallIcon(iconName);

	iconName = wclass->factory()->internalProperty(classname, "orientationSelectionPopup:verticalIcon");
	if (iconName.isEmpty() && wclass->inheritedClass())
		iconName = wclass->inheritedClass()->factory()->internalProperty(classname, "orientationSelectionPopup:verticalIcon");
	if (!iconName.isEmpty())
		iconVertical = SmallIcon(iconName);

	QString textHorizontal = wclass->factory()->internalProperty(classname, "orientationSelectionPopup:horizontalText");
	if (textHorizontal.isEmpty() && wclass->inheritedClass())
		iconName = wclass->inheritedClass()->factory()->internalProperty(classname, "orientationSelectionPopup:horizontalText");
	if (textHorizontal.isEmpty()) //default
		textHorizontal = i18n("Insert Horizontal Widget", "Insert Horizontal");

	QString textVertical = wclass->factory()->internalProperty(classname, "orientationSelectionPopup:verticalText");
	if (textVertical.isEmpty() && wclass->inheritedClass())
		iconName = wclass->inheritedClass()->factory()->internalProperty(classname, "orientationSelectionPopup:verticalText");
	if (textVertical.isEmpty()) //default
		textVertical = i18n("Insert Vertical Widget", "Insert Vertical");

	KPopupMenu* popup = new KPopupMenu(parent, "orientationSelectionPopup");
	popup->insertTitle(SmallIcon(wclass->pixmap()), i18n("Insert Widget: %1").arg(wclass->name()));
	popup->insertItem(iconHorizontal, textHorizontal, 1);
	popup->insertItem(iconVertical, textVertical, 2);
	popup->insertSeparator();
	popup->insertItem(SmallIcon("button_cancel"), i18n("Cancel"), 3);
	WidgetFactory::CreateWidgetOptions result;
	switch (popup->exec(pos)) {
	case 1:
		result = WidgetFactory::HorizontalOrientation; break;
	case 2:
		result = WidgetFactory::VerticalOrientation; break;
	default:
		result = WidgetFactory::AnyOrientation; //means "cancelled"
	}
	delete popup;
	return result;
}

bool WidgetLibrary::propertySetShouldBeReloadedAfterPropertyChange(
	const QCString& classname, QWidget *w, const QCString& property)
{
	WidgetInfo *winfo = widgetInfoForClassName(classname);
	if (!winfo)
		return false;
	return winfo->factory()->propertySetShouldBeReloadedAfterPropertyChange(classname, w, property);
}

#include "widgetlibrary.moc"
