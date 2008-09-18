/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#ifndef KFORMDESIGNERWIDGETLIBRARY_H
#define KFORMDESIGNERWIDGETLIBRARY_H

#include <qobject.h>
#include <qmap.h>
#include <q3dict.h>
#include <Q3ValueList>
#include <Q3CString>
#include <Q3PtrList>

#include "widgetfactory.h"

template<class type> class Q3PtrList;
template<class type> class Q3ValueVector;
class KActionCollection;
class KAction;
class QMenu;
class QWidget;
class QVariant;
class QDomDocument;
class QDomElement;

namespace KFormDesigner
{

class Container;
class ObjectTreeItem;
class WidgetLibraryPrivate;
class WidgetPropertySet;

typedef Q3PtrList<KAction> ActionList;

/**
 * This class searches for factories and provides KActions for widget creation.
 * Every widget can be located using this library.
 * You call WidgetLibrary functions instead of calling directly factories.
 * See WidgetFactory for a description of the functions.
 */
class KFORMEDITOR_EXPORT WidgetLibrary : public QObject
{
    Q_OBJECT

public:
    /*! Constructs WidgetLibrary object.
     In \a supportedFactoryGroups you can provide
     factory group list to be supported. Factory groups are defined by
     "X-KFormDesigner-FactoryGroup" field in every factory serviece's .desktop file.
     By default (when supportedFactoryGroups is empty) only factories having empty
     "X-KFormDesigner-FactoryGroup" field will be loaded.
     Factory group names are case-insensitive. */
    WidgetLibrary(QObject *parent = 0, const QStringList& supportedFactoryGroups = QStringList());

    virtual ~WidgetLibrary();

    /**
     * creates actions for widget creating
     */
    ActionList createWidgetActions(KXMLGUIClient* client, KActionCollection *parent,
                                   QObject *receiver, const char *slot);

    void addCustomWidgetActions(KActionCollection *col);

//old  /**
//old   * creates the XML for widget actions
//old   */
//old  QString createXML();

    /**
     * searches the right factory and creates a widget.
     * \return the widget or 0 if something falid
     */
    QWidget *createWidget(const Q3CString &classname, QWidget *parent, const char *name, Container *c,
                          int options = WidgetFactory::DefaultOptions);

    bool createMenuActions(const Q3CString &c, QWidget *w, QMenu *menu,
                           KFormDesigner::Container *container);

    /**
     * Shows orientation selection popup.
     * \return one of the following values:
     * - WidgetFactory::AnyOrientation (means no selection has been made, i.e. it was cancelled)
     * - WidgetFactory::HorizontalOrientation
     * - WidgetFactory::VerticalOrientation
     */
    WidgetFactory::CreateWidgetOptions showOrientationSelectionPopup(
        const Q3CString &classname, QWidget* parent, const QPoint& pos);

    QString internalProperty(const Q3CString& classname, const Q3CString& property);

    QString displayName(const Q3CString &classname);
    QString namePrefix(const Q3CString &classname);
    QString textForWidgetName(const Q3CString &name, const Q3CString &className);

    /*! Checks if the \a classname is an alternate classname,
     and returns the good classname.
     If \a classname is not alternate, \a classname is returned. */
    Q3CString classNameForAlternate(const Q3CString &classname);
    QString iconName(const Q3CString &classname);
    QString includeFileName(const Q3CString &classname);
    QString savingName(const Q3CString &classname);

    bool startEditing(const Q3CString &classname, QWidget *w, Container *container);
    bool previewWidget(const Q3CString &classname, QWidget *widget, Container *container);
    bool clearWidgetContent(const Q3CString &classname, QWidget *w);

    bool saveSpecialProperty(const Q3CString &classname, const QString &name,
                             const QVariant &value, QWidget *w, QDomElement &parentNode, QDomDocument &parent);
    bool readSpecialProperty(const Q3CString &classname, QDomElement &node, QWidget *w,
                             ObjectTreeItem *item);
    bool isPropertyVisible(const Q3CString &classname, QWidget *w,
                           const Q3CString &property, bool multiple = false, bool isTopLevel = false);

    Q3ValueList<Q3CString> autoSaveProperties(const Q3CString &classname);

    WidgetInfo* widgetInfoForClassName(const char* classname);

    WidgetFactory* factoryForClassName(const char* className);

    WidgetFactory* factory(const char* factoryName) const;

    /*! \return true if advanced properties like "mouseTracking" should
     be user-visible. True by default (in KFD), but Kexi set's this to false.
     See WidgetLibraryPrivate class implementation for complete list
     of advanced properties. */
    bool advancedPropertiesVisible() const;

    /*! Sets advanced properties to be visible or not. */
    void setAdvancedPropertiesVisible(bool set);

    /*! \return The i18n'ed name of the property \a propertyName
     for a class described by \a winfo. The name can be displayed in
     PropertyEditor. The name is retrieved from class' widget library.
     If this library doesn't define description for such property,
     and there is a parent library for \a winfo defined, parent library
     is asked for returning description string.
     Eventually, if even this failed, empty string is returned.
     @see WidgetFactory::propertyDescForName() */
    QString propertyDescForName(WidgetInfo *winfo, const Q3CString& propertyName);

    /*! \return The i18n'ed name of the property's value whose name is \a name.
     Works in the same way as propertyDescForName(): if actual library
     does not define a description we are looking for, parent factory is asked
     to return such description.
     Eventually, if even this failed, empty string is returned.
     @see WidgetFactory::propertyDescForValue() */
    QString propertyDescForValue(WidgetInfo *winfo, const Q3CString& name);

    /*! Used by WidgetPropertySet::setWidget() after creating properties. */
    void setPropertyOptions(WidgetPropertySet &list, const WidgetInfo& winfo, QWidget* w);

    /*! \return true if property sets should be reloaded for \a property property,
     \a classname class and widget \a w when a given property value changed. */
    bool propertySetShouldBeReloadedAfterPropertyChange(const Q3CString& classname, QWidget *w,
            const Q3CString& property);

signals:
    void prepareInsert(const Q3CString &c);

    //! Received by KexiFormPart::slotWidgetCreatedByFormsLibrary() so we can add drag/drop
    //! connection for the new widget
    void widgetCreated(QWidget *widget);

protected:
    /**
     * Adds a factory to the library, creates actions for widgets in the added factory.
     * This function is not called directly but by the factory locater.
     */
    void loadFactoryWidgets(WidgetFactory *f);

#if 0 //UNIMPLEMENTED
    /**
     * you can restrict the loaded factories by setting the filter to a pattern
     * like 'kexi|containers' in that case only factory containing 'kexi' or containers will be loaded.
     * this is useful if you want to embedd formeditor and provide e.g. a LineEdit with special features
     * but don't want to confuse the user... are you confused now?
     * NB: not implemented yet
     */
    void setFilter(const QRegExp &expr);
#endif

    /**
     * Lookups widget factories list (note that this function get called once in ctor)
     */
    void lookupFactories();

    /**
     * Loads widget factories found in lookupFactories(). This is called once.
     */
    void loadFactories();

    WidgetLibraryPrivate *d;
};

}
#endif
