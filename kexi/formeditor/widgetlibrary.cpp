/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2015 Jarosław Staniek <staniek@kde.org>

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

#include "widgetlibrary.h"
#include <KexiIcon.h>
#include "WidgetInfo.h"
#include "widgetfactory.h"
#include "libactionwidget.h"
#include "container.h"
#include "form.h"
#include "formIO.h"
#include "FormWidgetInterface.h"
#include "objecttree.h"
#include "KexiJsonTrader.h"
#include "KexiFormWidgetsPluginMetaData.h"
#include <core/kexiguimsghandler.h>

#include <KActionCollection>
#include <KLocalizedString>

#include <QDomDocument>
#include <QMenu>
#include <QDebug>

namespace KFormDesigner
{

Q_GLOBAL_STATIC_WITH_ARGS(KexiJsonTrader, KexiFormWidgetsPluginTrader_instance, ("kexi/forms/widgets"))

//! @internal
class WidgetLibrary::Private
{
public:
    Private(WidgetLibrary *library, const QStringList& supportedFactoryGroups)
            : showAdvancedProperties(true)
            , q(library)
            , m_couldNotFindAnyFormWidgetPluginsErrorDisplayed(false)
            , m_supportedFactoryGroups(supportedFactoryGroups.toSet())
            , m_lookupDone(false)
            , m_loadFactoriesDone(false)
    {
        q->setMessageHandler(&messageHandler);
        m_advancedProperties.insert("acceptDrops");
        m_advancedProperties.insert("accessibleDescription");
        m_advancedProperties.insert("accessibleName");
        m_advancedProperties.insert("autoMask");
        m_advancedProperties.insert("backgroundOrigin");
        m_advancedProperties.insert("backgroundMode");//this is rather useless
        m_advancedProperties.insert("baseSize");
        m_advancedProperties.insert("contextMenuEnabled");
        m_advancedProperties.insert("contextMenuPolicy");
        m_advancedProperties.insert("cursorPosition");
        m_advancedProperties.insert("cursorMoveStyle");
        m_advancedProperties.insert("dragEnabled");
        m_advancedProperties.insert("enableSqueezedText");
        m_advancedProperties.insert("layout");// too large risk to break things
        m_advancedProperties.insert("layoutDirection");
        m_advancedProperties.insert("locale");
        m_advancedProperties.insert("mouseTracking");
/*! @todo: reenable */ m_advancedProperties.insert("palette");
        m_advancedProperties.insert("sizeAdjustPolicy"); //QAbstractScrollArea
        m_advancedProperties.insert("sizeIncrement");
        m_advancedProperties.insert("sizePolicy");
        m_advancedProperties.insert("statusTip");
        m_advancedProperties.insert("toolTipDuration");
        m_advancedProperties.insert("trapEnterKeyEvent");
        m_advancedProperties.insert("windowModality");
        m_advancedProperties.insert("autoExclusive");
        // by providing this in propeditor
        m_advancedProperties.insert("minimumSize");
        m_advancedProperties.insert("maximumSize");
        m_advancedProperties.insert("clickMessage"); // for backward compatibility Kexi projects created with Qt < 4.7
        m_advancedProperties.insert("showClearButton"); // for backward compatibility Kexi projects created with Qt 4
#ifndef KEXI_SHOW_UNFINISHED
/*! @todo reenable */
        m_advancedProperties.insert("accel");
        m_advancedProperties.insert("icon");
        m_advancedProperties.insert("paletteBackgroundPixmap");
        m_advancedProperties.insert("pixmap");
        m_advancedProperties.insert("shortcut"); // renamed from "accel" in Qt 4
        m_advancedProperties.insert("windowIcon"); // renamed from "icon" in Qt 4
#endif
    }
    ~Private() {
        qDeleteAll(m_factories);
        m_factories.clear();
        qDeleteAll(m_pluginsMetaData);
        m_pluginsMetaData.clear();
    }

    QHash<QByteArray, WidgetInfo*> widgets() {
        KDbMessageGuard mg(q);
        (void)loadFactories();
        return m_widgets;
    }

    QHash<QByteArray, WidgetFactory*> factories() {
        KDbMessageGuard mg(q);
        (void)loadFactories();
        return m_factories;
    }

    bool isAdvancedProperty(const QByteArray &property) const {
        return m_advancedProperties.contains(property);
    }

    bool showAdvancedProperties;

private:
    //! Performs a form widget plugins lookup. @retrun true on success.
    //! @todo This method generates a few warnings, maybe we want to optionally display them somewhere (via the message handler)?
    bool lookup() {
        //! @todo Allow refreshing
        if (m_lookupDone) {
            return m_lookupResult;
        }
        m_lookupDone = true;
        m_lookupResult = false;
        q->clearResult();

        QStringList serviceTypes;
        serviceTypes << "Kexi/FormWidgets";
        const QList<QPluginLoader*> offers = KexiFormWidgetsPluginTrader_instance->query(serviceTypes);
        foreach(const QPluginLoader *loader, offers) {
            QScopedPointer<KexiFormWidgetsPluginMetaData> metaData(new KexiFormWidgetsPluginMetaData(*loader));
            if (metaData->id().isEmpty()) {
                qWarning() << "No plugin ID (X-KDE-PluginInfo-Name) specified for Kexi Form Widgets plugin"
                           << metaData->fileName() << "-- skipping!";
                continue;
            }
            // check version
            if (metaData->majorVersion() != KFormDesigner::version()) {
                qWarning() << "Kexi Form Widgets plugin" << metaData->id() << "has version (X-KDE-PluginInfo-Version)"
                           << metaData->majorVersion() << "but required version is" << KFormDesigner::version()
                           << "-- skipping!";
                continue;
            }
            // skip duplicates
            if (m_pluginsMetaData.contains(metaData->id())) {
                qWarning() << "More than one Kexi Form Widgets plugin with ID"
                           << metaData->id() << metaData->fileName() << "-- skipping this one";
                continue;
            }
            //qDebug() << "found factory:" << ptr->name();

            if (!metaData->group().isEmpty() && !m_supportedFactoryGroups.contains(metaData->group())) {
                qDebug() << "Factory group" << metaData->group()
                         << "for Form Widgets plugin"
                         << metaData->id() << metaData->fileName()
                         << "is not supported -- skipping!";
                continue;
            }

            m_pluginsMetaData.insert(metaData->id(), metaData.data());
            metaData.take();
        }
        if (m_pluginsMetaData.isEmpty()) {
            q->m_result = KDbResult(i18n("Could not find any form widget plugins."));
            m_couldNotFindAnyFormWidgetPluginsErrorDisplayed = true;
            return false;
        }
        m_lookupResult = true;
        return true;
    }

    //! Loads all factory plugins
    bool loadFactories() {
        if (m_loadFactoriesDone) {
            if (m_couldNotFindAnyFormWidgetPluginsErrorDisplayed) {
                q->clearResult(); // show the warning only once
            }
            return m_loadFactoriesResult;
        }
        m_loadFactoriesDone = true;
        m_loadFactoriesResult = false;
        if (!lookup()) {
            return false;
        }
        foreach (KexiFormWidgetsPluginMetaData *pluginMetaData, m_pluginsMetaData) {
            WidgetFactory *factory = loadFactory(pluginMetaData);
            if (!factory) {
                continue;
            }
            //collect information about classes to be hidden
            if (factory->hiddenClasses()) {
                foreach (const QByteArray &c, *factory->hiddenClasses()) {
                    m_hiddenClasses.insert(c);
                }
            }
        }

        //now we have factories instantiated: load widgets
        QList<WidgetFactory*> loadLater;
        foreach (WidgetFactory *factory, m_factories) {
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
        m_loadFactoriesResult = true;
        return true;
    }

    //! Loads of a single factory. @return true on success
    WidgetFactory *loadFactory(KexiFormWidgetsPluginMetaData *pluginMetaData) {
        KPluginFactory *factory = qobject_cast<KPluginFactory*>(pluginMetaData->instantiate());
        if (!factory) {
            q->m_result = KDbResult(ERR_OBJECT_NOT_FOUND,
                                    xi18nc("@info", "Could not load Kexi Form Widgets plugin file \"%1\".",
                                    pluginMetaData->fileName()));
            pluginMetaData->setErrorMessage(q->result().message());
            qWarning() << q->result().message();
            return 0;
        }
        WidgetFactory *widgetFactory = factory->create<WidgetFactory>(q);
        if (!widgetFactory) {
            q->m_result = KDbResult(ERR_CANNOT_LOAD_OBJECT,
                                    i18n("Could not open Kexi Form Widgets plugin \"%1\".")
                                    .arg(pluginMetaData->fileName()));
            qWarning() << q->m_result.message();
            return 0;
        }
        widgetFactory->setLibrary(q);
        widgetFactory->setObjectName(pluginMetaData->id());
        widgetFactory->setAdvancedPropertiesVisible(showAdvancedProperties); //inherit this flag from the library
        m_factories.insert(pluginMetaData->id().toLatin1(), widgetFactory);
        return widgetFactory;
    }

    //! Loads widgets for factory @a f
    void loadFactoryWidgets(WidgetFactory *f) {
        QHash<QByteArray, WidgetInfo*> widgetsForFactory(f->classes());
        foreach (WidgetInfo *w, widgetsForFactory) {
            if (m_hiddenClasses.contains( w->className() ))
                continue; //this class is hidden
            // check if we want to inherit a widget from a different factory
            if (!w->parentFactoryName().isEmpty() && !w->inheritedClassName().isEmpty()) {
                WidgetFactory *parentFactory = m_factories.value(w->parentFactoryName().toLower());
                if (!parentFactory) {
                    qWarning() << "class" << w->className() << ": no such parent factory" << w->parentFactoryName();
                    continue;
                }
                WidgetInfo* inheritedClass = parentFactory->widgetInfoForClassName(w->inheritedClassName());
                if (!inheritedClass) {
                    qWarning() << "class" << w->inheritedClassName() << " - no such class to inherit in factory"
                        << w->parentFactoryName();
                    continue;
                }
                //ok: inherit properties:
                w->setInheritedClass( inheritedClass );
                if (w->iconName().isEmpty())
                    w->setIconName(inheritedClass->iconName());
                //ok?
                foreach(const QByteArray& alternateName, inheritedClass->alternateClassNames()) {
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

            QList<QByteArray> cnames( w->alternateClassNames() );
            cnames.prepend(w->className());
            foreach (const QByteArray &wname, cnames) {
                WidgetInfo *widgetForClass = widgetsForFactory.value(wname);
                if (!widgetForClass || (widgetForClass && !widgetForClass->isOverriddenClassName(wname))) {
                    //insert a widgetinfo, if:
                    //1) this class has no alternate class assigned yet, or
                    //2) this class has alternate class assigned but without 'override' flag
                    m_widgets.insert(wname, w);
                }
            }
        }
    }

    WidgetLibrary *q;
    KexiGUIMessageHandler messageHandler;
    //! A map which associates a class name with a Widget class
    QHash<QString, KexiFormWidgetsPluginMetaData*> m_pluginsMetaData; //!< owner
    bool m_couldNotFindAnyFormWidgetPluginsErrorDisplayed;
    QSet<QString> m_supportedFactoryGroups;
    QHash<QByteArray, WidgetFactory*> m_factories; //!< owner
    QHash<QByteArray, WidgetInfo*> m_widgets; //!< owner
    QSet<QByteArray> m_advancedProperties;
    QSet<QByteArray> m_hiddenClasses;
    bool m_lookupDone;
    bool m_lookupResult;
    bool m_loadFactoriesDone;
    bool m_loadFactoriesResult;
};
}

using namespace KFormDesigner;

//-------------------------------------------

WidgetLibrary::WidgetLibrary(QObject *parent, const QStringList& supportedFactoryGroups)
        : QObject(parent)
        , KDbResultable()
        , d(new Private(this, supportedFactoryGroups))
{
}

WidgetLibrary::~WidgetLibrary()
{
    delete d;
}

void WidgetLibrary::createWidgetActions(ActionGroup *group)
{
    foreach (WidgetInfo *winfo, d->widgets()) {
        LibActionWidget *a = new LibActionWidget(group, winfo);
        connect(a, SIGNAL(toggled(QByteArray)), this, SIGNAL(widgetActionToggled(QByteArray)));
    }
}

void
WidgetLibrary::addCustomWidgetActions(KActionCollection *col)
{
    if (!col)
        return;
    foreach (WidgetFactory *factory, d->factories()) {
        factory->createCustomActions(col);
    }
}

QWidget* WidgetLibrary::createWidget(const QByteArray &classname, QWidget *parent,
                                     const char *name, Container *c,
                                     WidgetFactory::CreateWidgetOptions options)
{
    WidgetInfo *wclass = d->widgets().value(classname);
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
    if (options & WidgetFactory::DesignViewMode) {
        FormWidgetInterface* fwiface = dynamic_cast<FormWidgetInterface*>(widget);
        if (fwiface)
            fwiface->setDesignMode(true);
    }
    emit widgetCreated(widget);
    return widget;
}

bool WidgetLibrary::createMenuActions(const QByteArray &c, QWidget *w, QMenu *menu,
                                      KFormDesigner::Container *container)
{
    WidgetInfo *wclass = d->widgets().value(c);
    if (!wclass)
        return false;

    if (wclass->factory()->createMenuActions(c, w, menu, container)) {
        return true;
    }
    //try from inherited class
    if (wclass->inheritedClass()) {
        return wclass->inheritedClass()->factory()->createMenuActions(
                   wclass->className(), w, menu, container);
    }
    return false;
}

bool WidgetLibrary::startInlineEditing(const QByteArray &classname, QWidget *w,
                                       Container *container)
{
    WidgetInfo *wclass = d->widgets().value(classname);
    if (!wclass)
        return false;

    FormWidgetInterface* fwiface = dynamic_cast<FormWidgetInterface*>(w);
    {
        KFormDesigner::WidgetFactory::InlineEditorCreationArguments args(classname, w, container);
        if (wclass->factory()->startInlineEditing(args)) {
            args.container->form()->createInlineEditor(args);
            if (fwiface)
                fwiface->setEditingMode(true);
            return true;
        }
    }
    if (wclass->inheritedClass()) {
        //try from inherited class
        KFormDesigner::WidgetFactory::InlineEditorCreationArguments args(wclass->className(), w, container);
        if (wclass->inheritedClass()->factory()->startInlineEditing(args)) {
            args.container->form()->createInlineEditor(args);
            if (fwiface)
                fwiface->setEditingMode(true);
            return true;
        }
    }
    return false;
}

bool WidgetLibrary::previewWidget(const QByteArray &classname, QWidget *widget, Container *container)
{
    WidgetInfo *wclass = d->widgets().value(classname);
    if (!wclass)
        return false;

    FormWidgetInterface* fwiface = dynamic_cast<FormWidgetInterface*>(widget);
    if (fwiface)
        fwiface->setDesignMode(false);
    if (wclass->factory()->previewWidget(classname, widget, container))
        return true;
    //try from inherited class
    if (wclass->inheritedClass())
        return wclass->inheritedClass()->factory()->previewWidget(wclass->className(), widget, container);
    return false;
}

bool WidgetLibrary::clearWidgetContent(const QByteArray &classname, QWidget *w)
{
    WidgetInfo *wclass = d->widgets().value(classname);
    if (!wclass)
        return false;

    if (wclass->factory()->clearWidgetContent(classname, w))
        return true;
    //try from inherited class
    if (wclass->inheritedClass())
        return wclass->inheritedClass()->factory()->clearWidgetContent(wclass->className(), w);
    return false;
}

QString WidgetLibrary::displayName(const QByteArray &classname)
{
    WidgetInfo *wi = d->widgets().value(classname);
    if (wi)
        return wi->name();

    return classname;
}

QString WidgetLibrary::savingName(const QByteArray &classname)
{
    QString s;
    WidgetInfo *wi = d->widgets().value(classname);
    if (wi && !wi->savingName().isEmpty())
        return wi->savingName();

    return classname;
}

QString WidgetLibrary::namePrefix(const QByteArray &classname)
{
    WidgetInfo *wi = d->widgets().value(classname);
    if (wi)
        return wi->namePrefix();

    return classname;
}

QString WidgetLibrary::textForWidgetName(const QByteArray &name, const QByteArray &className)
{
    WidgetInfo *widget = d->widgets().value(className);
    if (!widget)
        return QString();

    QString newName = name;
    newName.remove(widget->namePrefix());
    newName = widget->name() + (newName.isEmpty() ? QString() : (QLatin1String(" ") + newName));
    return newName;
}

QByteArray
WidgetLibrary::classNameForAlternate(const QByteArray &classname)
{
    if (d->widgets().value(classname))
        return classname;

    WidgetInfo *wi =  d->widgets().value(classname);
    if (wi) {
        return wi->className();
    }

    // widget not supported
    return "CustomWidget";
}

QString WidgetLibrary::includeFileName(const QByteArray &classname)
{
    WidgetInfo *wi = d->widgets().value(classname);
    if (wi)
        return wi->includeFileName();

    return QString();
}

QString
WidgetLibrary::iconName(const QByteArray &classname)
{
    WidgetInfo *wi = d->widgets().value(classname);
    if (wi)
        return wi->iconName();

    return koIconName("unknown_widget");
}

bool
WidgetLibrary::saveSpecialProperty(const QByteArray &classname,
    const QString &name, const QVariant &value, QWidget *w,
    QDomElement &parentNode, QDomDocument &parent)
{
    WidgetInfo *wi = d->widgets().value(classname);
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
    WidgetInfo *wi = d->widgets().value(classname);
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

    WidgetInfo *wi = d->widgets().value(classname);
    if (!wi)
        return false;
    if (!d->showAdvancedProperties && d->isAdvancedProperty(property)) {
        //this is advanced property, should we hide it?
        if (!wi->internalProperty("forceShowAdvancedProperty:" + property).toBool()
                && (!wi->inheritedClass() || !wi->inheritedClass()->internalProperty("forceShowAdvancedProperty:" + property).toBool())) {
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

QList<QByteArray> WidgetLibrary::autoSaveProperties(const QByteArray &classname)
{
    WidgetInfo *wi = d->widgets().value(classname);
    if (!wi)
        return QList<QByteArray>();
    return wi->autoSaveProperties();
}

WidgetInfo*
WidgetLibrary::widgetInfoForClassName(const char* classname)
{
    return d->widgets().value(classname);
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
    QString desc(winfo->factory()->propertyDescription(propertyName));
    if (!desc.isEmpty())
        return desc;
    if (winfo->parentFactoryName().isEmpty())
        return QString();

    //try in parent factory, if exists
    WidgetFactory *parentFactory = d->factories().value(winfo->parentFactoryName());
    if (!parentFactory)
        return QString();

    return parentFactory->propertyDescription(propertyName);
}

QString WidgetLibrary::propertyDescForValue(WidgetInfo *winfo, const QByteArray& name)
{
    if (!winfo->factory())
        return QString();
    QString desc(winfo->factory()->valueDescription(name));
    if (!desc.isEmpty())
        return desc;
    if (winfo->parentFactoryName().isEmpty())
        return QString();

    //try in parent factory, if exists
    WidgetFactory *parentFactory = d->factories().value(winfo->parentFactoryName());
    if (!parentFactory)
        return QString();

    return parentFactory->valueDescription(name);
}

void WidgetLibrary::setPropertyOptions(KPropertySet& set, const WidgetInfo& winfo, QWidget* w)
{
    if (!winfo.factory())
        return;
    winfo.factory()->setPropertyOptions(set, winfo, w);
    if (winfo.parentFactoryName().isEmpty())
        return;
    WidgetFactory *parentFactory = d->factories().value(winfo.parentFactoryName());
    if (!parentFactory)
        return;
    parentFactory->setPropertyOptions(set, winfo, w);
}

WidgetFactory* WidgetLibrary::factory(const char* factoryName) const
{
    return d->factories().value(factoryName);
}

QVariant WidgetLibrary::internalProperty(const QByteArray& classname, const QByteArray& property)
{
    WidgetInfo *wclass = d->widgets().value(classname);
    if (!wclass)
        return QString();
    QVariant value(wclass->internalProperty(property));
    if (value.isNull() && wclass->inheritedClass())
        return wclass->inheritedClass()->internalProperty(property);
    return value;
}

WidgetFactory::CreateWidgetOption WidgetLibrary::showOrientationSelectionPopup(
    const QByteArray &classname, QWidget* parent, const QPoint& pos)
{
    WidgetInfo *wclass = d->widgets().value(classname);
    if (!wclass)
        return WidgetFactory::AnyOrientation;

    //get custom icons and strings
    QIcon iconHorizontal, iconVertical;
    QString iconName(wclass->internalProperty("orientationSelectionPopup:horizontalIcon").toString());
    if (iconName.isEmpty() && wclass->inheritedClass())
        iconName = wclass->inheritedClass()->internalProperty("orientationSelectionPopup:horizontalIcon").toString();
    if (!iconName.isEmpty())
        iconHorizontal = QIcon::fromTheme(iconName);

    iconName = wclass->internalProperty("orientationSelectionPopup:verticalIcon").toString();
    if (iconName.isEmpty() && wclass->inheritedClass())
        iconName = wclass->inheritedClass()->internalProperty("orientationSelectionPopup:verticalIcon").toString();
    if (!iconName.isEmpty())
        iconVertical = QIcon::fromTheme(iconName);

    QString textHorizontal = wclass->internalProperty("orientationSelectionPopup:horizontalText").toString();
    if (textHorizontal.isEmpty() && wclass->inheritedClass())
        iconName = wclass->inheritedClass()->internalProperty("orientationSelectionPopup:horizontalText").toString();
    if (textHorizontal.isEmpty()) //default
        textHorizontal = xi18nc("Insert Horizontal Widget", "Insert Horizontal");

    QString textVertical = wclass->internalProperty("orientationSelectionPopup:verticalText").toString();
    if (textVertical.isEmpty() && wclass->inheritedClass())
        iconName = wclass->inheritedClass()->internalProperty("orientationSelectionPopup:verticalText").toString();
    if (textVertical.isEmpty()) //default
        textVertical = xi18nc("Insert Vertical Widget", "Insert Vertical");

    QMenu popup(parent);
    popup.setObjectName("orientationSelectionPopup");
    popup.addSection(QIcon::fromTheme(wclass->iconName()), xi18n("Insert Widget: %1", wclass->name()));
    QAction* horizAction = popup.addAction(iconHorizontal, textHorizontal);
    QAction* vertAction = popup.addAction(iconVertical, textVertical);
    popup.addSeparator();
    popup.addAction(koIcon("dialog-cancel"), xi18n("Cancel"));
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

ObjectTreeItem* WidgetLibrary::selectableItem(ObjectTreeItem* item)
{
    qDebug() << item->widget()->metaObject()->className();
    WidgetInfo *wi = d->widgets().value(item->widget()->metaObject()->className());
    if (!wi)
        return item;
    return wi->factory()->selectableItem(item);
}

