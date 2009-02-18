/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include <kdebug.h>
#include <klocale.h>
#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kactioncollection.h>

#include "widgetfactory.h"
#include "widgetlibrary.h"
#include "libactionwidget.h"
#include "container.h"
#include "form.h"
#include "formIO.h"

#include "../kexi_global.h"

namespace KFormDesigner
{

//! @internal
class XMLGUIClient : public QObject, public KXMLGUIClient
{
public:
    XMLGUIClient(KXMLGUIClient* parent, const QString& xmlFileName)
            : QObject(parent->actionCollection()), KXMLGUIClient(parent) {
        setXMLFile(xmlFileName, true /*merge*/);
    }
};

//! @internal
class WidgetLibraryPrivate
{
public:
    WidgetLibraryPrivate()
            : showAdvancedProperties(true)
            , factoriesLoaded(false)
    {
        advancedProperties.insert("autoMask");
        advancedProperties.insert("baseSize");
        advancedProperties.insert("mouseTracking");
        advancedProperties.insert("acceptDrops");
        advancedProperties.insert("cursorPosition");
        advancedProperties.insert("contextMenuEnabled");
        advancedProperties.insert("trapEnterKeyEvent");
        advancedProperties.insert("dragEnabled");
        advancedProperties.insert("enableSqueezedText");
        advancedProperties.insert("sizeIncrement");
/*! @todo: reenable */ advancedProperties.insert("palette");
//2.0 obsolete        advancedProperties.insert("backgroundOrigin");
        advancedProperties.insert("backgroundMode");//this is rather useless
        advancedProperties.insert("layout");// too large risk to break things
        // by providing this in propeditor
        advancedProperties.insert("minimumSize");
        advancedProperties.insert("maximumSize");
#ifdef KEXI_NO_UNFINISHED
/*! @todo reenable */
        advancedProperties.insert("paletteBackgroundPixmap");
        advancedProperties.insert("icon");
        advancedProperties.insert("pixmap");
        advancedProperties.insert("accel");
#endif
    }
    // dict which associates a class name with a Widget class
    WidgetInfo::Hash widgets;//, alternateWidgets;
    QHash<QByteArray, KService::Ptr> services;
    QSet<QByteArray> supportedFactoryGroups;
    QHash<QByteArray, WidgetFactory*> factories;
    QSet<QByteArray> advancedProperties;
    QSet<QByteArray> hiddenClasses;
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
    foreach (const QString &group, supportedFactoryGroups) {
        d->supportedFactoryGroups.insert(group.toLower().toLatin1());
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
    const WidgetInfo::Hash widgets( f->classes() );
    foreach (WidgetInfo *w, widgets) {
        if (d->hiddenClasses.contains( w->className() ))
            continue; //this class is hidden
        // check if we want to inherit a widget from a different factory
        if (!w->m_parentFactoryName.isEmpty() && !w->m_inheritedClassName.isEmpty()) {
            WidgetFactory *parentFactory = d->factories.value(w->m_parentFactoryName.toLower());
            if (!parentFactory) {
                kWarning() << "class '" << w->className()
                    << "' - no such parent factory '" << w->m_parentFactoryName << "'";
                continue;
            }
            WidgetInfo* inheritedClass = parentFactory->m_classesByName.value( w->m_inheritedClassName );
            if (!inheritedClass) {
                kWarning() << "class '" << w->m_inheritedClassName
                    << "' - no such class to inherit in factory '" << w->m_parentFactoryName << "'";
                continue;
            }
            //ok: inherit properties:
            w->m_inheritedClass = inheritedClass;
            if (w->pixmap().isEmpty())
                w->setPixmap(inheritedClass->pixmap());
            //ok?
            foreach(const QByteArray& alternateName, inheritedClass->m_alternateNames) {
                w->addAlternateClassName(
                    alternateName, inheritedClass->isOverriddenClassName(alternateName));
            }
            if (w->includeFileName().isEmpty())
                w->setIncludeFileName(inheritedClass->includeFileName());
            if (w->name().isEmpty())
                w->setName(inheritedClass->name());
            if (w->namePrefix().isEmpty())
                w->setNamePrefix(inheritedClass->namePrefix());
            if (w->description().isEmpty())
                w->setDescription(inheritedClass->description());
        }

//  kDebug() << "adding class " << w->className();
        QList<QByteArray> cnames( w->alternateClassNames() );
        cnames.prepend(w->className());
        //d->widgets.insert(w->className(), w);
//  if(!w->alternateClassName().isEmpty()) {
//   QStringList l = QStringList::split("|", w->alternateClassName());
        foreach (const QByteArray &wname, cnames) {
            WidgetInfo *widgetForClass = d->widgets.value(wname);
            if (!widgetForClass || (widgetForClass && !widgetForClass->isOverriddenClassName(wname))) {
                //insert a widgetinfo, if:
                //1) this class has no alternate class assigned yet, or
                //2) this class has alternate class assigned but without 'override' flag
                d->widgets.insert(wname, w);
            }

            /*   WidgetInfo *widgetForClass = d->alternateWidgets.find(*it);
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
    KService::List tlist = KServiceTypeTrader::self()->query("KFormDesigner/WidgetFactory");
    foreach (KService::Ptr ptr, tlist) {
        KService::Ptr existingService = d->services.value(ptr->library().toLower().toLatin1());
        if (!existingService.isNull()) {
            kWarning() << "factory '" << ptr->name()
                << "' already found (library=" << existingService->library()
                << ")! skipping this one: library=" << ptr->library();
            continue;
        }
        kDebug() << "found factory: " << ptr->name();

        QByteArray groupName = ptr->property("X-KFormDesigner-FactoryGroup").toByteArray();
        if (!groupName.isEmpty() && !d->supportedFactoryGroups.contains(groupName.toLower())) {
            kDebug() << "factory group '" << groupName
            << "' is unsupported by this application (library=" << ptr->library() << ")";
            continue;
        }
        const uint factoryVersion = ptr->property("X-KFormDesigner-WidgetFactoryVersion").toUInt();
        if (KFormDesigner::version() != factoryVersion) {
            kWarning() << QString("factory '%1'"
                                  " has version '%2' but required Widget Factory version is '%3'\n"
                                  " -- skipping this factory!").arg(ptr->library()).arg(factoryVersion)
            .arg(KFormDesigner::version());
            continue;
        }
        d->services.insert(ptr->library().toLower().toLatin1(), ptr);
    }
}

void
WidgetLibrary::loadFactories()
{
    if (d->factoriesLoaded)
        return;
    d->factoriesLoaded = true;
    foreach (KService::Ptr ptr, d->services) {
        WidgetFactory *f = KService::createInstance<WidgetFactory>(
                               ptr, this, QStringList());
        if (!f) {
            kWarning() << "creating factory failed! " << ptr->library();
            continue;
        }
        f->setObjectName(ptr->library());
        f->m_library = this;
        f->m_showAdvancedProperties = d->showAdvancedProperties; //inherit this flag from the library
        f->m_xmlGUIFileName = ptr->property("X-KFormDesigner-XMLGUIFileName").toString();
        d->factories.insert(f->objectName().toLower().toLatin1(), f);

        //collect information about classes to be hidden
        if (f->m_hiddenClasses) {
            foreach (const QByteArray &c, *f->m_hiddenClasses) {
                d->hiddenClasses.insert(c);
            }
        }
    }

    //now we have factories instantiated: load widgets
    QList<WidgetFactory*> loadLater;
    foreach (WidgetFactory *factory, d->factories) {
        //ONE LEVEL, FLAT INHERITANCE, but works!
        //if this factory inherits from something, load its witgets later
//! @todo improve
        if (factory->inheritsFactories())
            loadLater.append(factory);
        else
            loadFactoryWidgets(factory);
    }
    //load now the rest
    foreach (WidgetFactory* f, loadLater) {
        loadFactoryWidgets(f);
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

  Q3AsciiDictIterator<WidgetInfo> it(d->widgets);
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

void WidgetLibrary::createWidgetActions(QActionGroup *group)
{
    loadFactories();

#if 0 // 2.0: we're removing XML gui client stuff
    // init XML gui clients (custom factories have their own .rc files)
    foreach (WidgetFactory *factory, d->factories) {
        if (factory->m_xmlGUIFileName.isEmpty()) { // probably a built-in factory, with GUI file like kexiformpartinstui.rc
            factory->m_guiClient = 0;
        } else { // a custom factory with its own .rc file
            factory->m_guiClient = new XMLGUIClient(client, factory->m_xmlGUIFileName);
        }
    }
#endif

//2.0    ActionList actions;
    foreach (WidgetInfo *winfo, d->widgets) {
        LibActionWidget *a = new LibActionWidget(group, winfo);
 //2.0               winfo->factory()->m_guiClient
 //2.0               ? winfo->factory()->m_guiClient->actionCollection() : parent);
        connect(a, SIGNAL(toggled(const QByteArray &)), this, SIGNAL(widgetActionToggled(const QByteArray &)));
//2.0        actions.append(a);
    }
//2.0    return actions;
}

void
WidgetLibrary::addCustomWidgetActions(KActionCollection *col)
{
    if (!col)
        return;
    foreach (WidgetFactory *factory, d->factories) {
        factory->createCustomActions(
            factory->m_guiClient
            ? factory->m_guiClient->actionCollection() : col);
    }
}

QWidget*
WidgetLibrary::createWidget(const QByteArray &classname, QWidget *parent, const char *name, Container *c,
                            WidgetFactory::CreateWidgetOptions options)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(classname);
    if (!wclass)
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
WidgetLibrary::createMenuActions(const QByteArray &c, QWidget *w, QMenu *menu,
                                 KFormDesigner::Container *container)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(c);
    if (!wclass)
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
WidgetLibrary::startEditing(const QByteArray &classname, QWidget *w, Container *container)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(classname);
    if (!wclass)
        return false;

    if (wclass->factory()->startEditing(classname, w, container))
        return true;
    //try from inherited class
    if (wclass->inheritedClass())
        return wclass->inheritedClass()->factory()->startEditing(wclass->className(), w, container);
    return false;
}

bool
WidgetLibrary::previewWidget(const QByteArray &classname, QWidget *widget, Container *container)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(classname);
    if (!wclass)
        return false;

    if (wclass->factory()->previewWidget(classname, widget, container))
        return true;
    //try from inherited class
    if (wclass->inheritedClass())
        return wclass->inheritedClass()->factory()->previewWidget(wclass->className(), widget, container);
    return false;
}

bool
WidgetLibrary::clearWidgetContent(const QByteArray &classname, QWidget *w)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(classname);
    if (!wclass)
        return false;

    if (wclass->factory()->clearWidgetContent(classname, w))
        return true;
    //try from inherited class
    if (wclass->inheritedClass())
        return wclass->inheritedClass()->factory()->clearWidgetContent(wclass->className(), w);
    return false;
}

QString
WidgetLibrary::displayName(const QByteArray &classname)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
    if (wi)
        return wi->name();

    return classname;
}

QString
WidgetLibrary::savingName(const QByteArray &classname)
{
    loadFactories();
    QString s;
    WidgetInfo *wi = d->widgets.value(classname);
    if (wi && !wi->savingName().isEmpty())
        return wi->savingName();

    return classname;
}

QString
WidgetLibrary::namePrefix(const QByteArray &classname)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
    if (wi)
        return wi->namePrefix();

    return classname;
}

QString
WidgetLibrary::textForWidgetName(const QByteArray &name, const QByteArray &className)
{
    loadFactories();
    WidgetInfo *widget = d->widgets.value(className);
    if (!widget)
        return QString();

    QString newName = name;
    newName.remove(widget->namePrefix());
    newName = widget->name() + " " + newName;
    return newName;
}

QByteArray
WidgetLibrary::classNameForAlternate(const QByteArray &classname)
{
    loadFactories();
    if (d->widgets.value(classname))
        return classname;

    WidgetInfo *wi =  d->widgets.value(classname);
    if (wi) {
        return wi->className();
    }

    // widget not supported
    return "CustomWidget";
}

QString
WidgetLibrary::includeFileName(const QByteArray &classname)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
    if (wi)
        return wi->includeFileName();

    return QString();
}

QString
WidgetLibrary::iconName(const QByteArray &classname)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
    if (wi)
        return wi->pixmap();

    return QLatin1String("unknown_widget");
}

bool
WidgetLibrary::saveSpecialProperty(const QByteArray &classname, 
    const QString &name, const QVariant &value, QWidget *w, 
    QDomElement &parentNode, QDomDocument &parent)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
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
WidgetLibrary::readSpecialProperty(const QByteArray &classname, 
    QDomElement &node, QWidget *w, ObjectTreeItem *item)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
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
WidgetLibrary::isPropertyVisible(const QByteArray &classname, QWidget *w,
                                 const QByteArray &property, bool multiple, bool isTopLevel)
{
    if (isTopLevel) {
        // no focus policy for top-level form widget...
        if (!d->showAdvancedProperties && property == "focusPolicy")
            return false;
    }

    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
    if (!wi)
        return false;
    if (!d->showAdvancedProperties && d->advancedProperties.contains( property )) {
        //this is advanced property, should we hide it?
        if (wi->factory()->internalProperty(classname, "forceShowAdvancedProperty:" + property).isEmpty()
                && (!wi->inheritedClass() || wi->inheritedClass()->factory()->internalProperty(classname, "forceShowAdvancedProperty:" + property).isEmpty())) {
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

QList<QByteArray>
WidgetLibrary::autoSaveProperties(const QByteArray &classname)
{
    loadFactories();
    WidgetInfo *wi = d->widgets.value(classname);
    if (!wi)
        return QList<QByteArray>();
    QList<QByteArray> lst;
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
    return d->widgets.value(classname);
}

WidgetFactory*
WidgetLibrary::factoryForClassName(const char* classname)
{
    WidgetInfo *wi = widgetInfoForClassName(classname);
    return wi ? wi->factory() : 0;
}

QString WidgetLibrary::propertyDescForName(WidgetInfo *winfo, const QByteArray& propertyName)
{
    if (!winfo || !winfo->factory())
        return QString();
    QString desc(winfo->factory()->propertyDescForName(propertyName));
    if (!desc.isEmpty())
        return desc;
    if (winfo->m_parentFactoryName.isEmpty())
        return QString();

    //try in parent factory, if exists
    WidgetFactory *parentFactory = d->factories.value(winfo->m_parentFactoryName);
    if (!parentFactory)
        return QString();

    return parentFactory->propertyDescForName(propertyName);
}

QString WidgetLibrary::propertyDescForValue(WidgetInfo *winfo, const QByteArray& name)
{
    if (!winfo->factory())
        return QString();
    QString desc(winfo->factory()->propertyDescForValue(name));
    if (!desc.isEmpty())
        return desc;
    if (winfo->m_parentFactoryName.isEmpty())
        return QString();

    //try in parent factory, if exists
    WidgetFactory *parentFactory = d->factories.value(winfo->m_parentFactoryName);
    if (!parentFactory)
        return QString();

    return parentFactory->propertyDescForValue(name);
}

void WidgetLibrary::setPropertyOptions(KoProperty::Set& set, const WidgetInfo& winfo, QWidget* w)
{
    if (!winfo.factory())
        return;
    winfo.factory()->setPropertyOptions(set, winfo, w);
    if (winfo.m_parentFactoryName.isEmpty())
        return;
    WidgetFactory *parentFactory = d->factories.value(winfo.m_parentFactoryName);
    if (!parentFactory)
        return;
    parentFactory->setPropertyOptions(set, winfo, w);
}

WidgetFactory* WidgetLibrary::factory(const char* factoryName) const
{
    return d->factories.value(factoryName);
}

QString WidgetLibrary::internalProperty(const QByteArray& classname, const QByteArray& property)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(classname);
    if (!wclass)
        return QString();
    QString value(wclass->factory()->internalProperty(classname, property));
    if (value.isEmpty() && wclass->inheritedClass())
        return wclass->inheritedClass()->factory()->internalProperty(classname, property);
    return value;
}

WidgetFactory::CreateWidgetOption WidgetLibrary::showOrientationSelectionPopup(
    const QByteArray &classname, QWidget* parent, const QPoint& pos)
{
    loadFactories();
    WidgetInfo *wclass = d->widgets.value(classname);
    if (!wclass)
        return WidgetFactory::AnyOrientation;

    //get custom icons and strings
    QPixmap iconHorizontal, iconVertical;
    QString iconName(wclass->factory()->internalProperty(classname, "orientationSelectionPopup:horizontalIcon"));
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
        textHorizontal = i18nc("Insert Horizontal Widget", "Insert Horizontal");

    QString textVertical = wclass->factory()->internalProperty(classname, "orientationSelectionPopup:verticalText");
    if (textVertical.isEmpty() && wclass->inheritedClass())
        iconName = wclass->inheritedClass()->factory()->internalProperty(classname, "orientationSelectionPopup:verticalText");
    if (textVertical.isEmpty()) //default
        textVertical = i18nc("Insert Vertical Widget", "Insert Vertical");

    KMenu popup(parent);
    popup.setObjectName("orientationSelectionPopup");
    popup.addTitle(SmallIcon(wclass->pixmap()), i18n("Insert Widget: %1", wclass->name()));
    QAction* horizAction = popup.addAction(iconHorizontal, textHorizontal);
    QAction* vertAction = popup.addAction(iconVertical, textVertical);
    popup.addSeparator();
    popup.addAction(SmallIcon("dialog-cancel"), i18n("Cancel"));
    QAction *a = popup.exec(pos);
    if (a == horizAction)
        return WidgetFactory::HorizontalOrientation;
    else if (a == vertAction)
        return WidgetFactory::VerticalOrientation;

    return WidgetFactory::AnyOrientation; //means "cancelled"
}

bool WidgetLibrary::propertySetShouldBeReloadedAfterPropertyChange(
    const QByteArray& classname, QWidget *w, const QByteArray& property)
{
    WidgetInfo *winfo = widgetInfoForClassName(classname);
    if (!winfo)
        return false;
    return winfo->factory()->propertySetShouldBeReloadedAfterPropertyChange(classname, w, property);
}

#include "widgetlibrary.moc"
