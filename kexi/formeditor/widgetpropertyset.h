/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KFD_WIDGETPROPERTYSET_H
#define KFD_WIDGETPROPERTYSET_H

#include <QObject>
#include <QStringList>
#include <QMetaProperty>

#include <kexi_export.h>
#include <koproperty/Set.h>
#include <koproperty/Property.h>

class QWidget;
class QEvent;

namespace KFormDesigner
{

class FormManager;
class ObjectTreeItem;
class WidgetPropertySetPrivate;
class WidgetInfo;
class CommandGroup;

class KFORMEDITOR_EXPORT WidgetPropertySet : public QObject
{
    Q_OBJECT

public:
    WidgetPropertySet(QObject *parent);
    ~WidgetPropertySet();

//  FormManager* manager();

    KoProperty::Property&  operator[](const QByteArray &name);

    KoProperty::Property&  property(const QByteArray &name);

    bool  contains(const QByteArray &property);

    /*! i18n function used by factories to add new property caption.
      Should be called on Factory creation. */
    void  addPropertyCaption(const QByteArray &property, const QString &caption);

    void  addValueCaption(const QByteArray &value, const QString &caption);

public slots:
    /*! Sets the widget which properties are shown in the property editor.
     If \a add is true, the list switch to multiple widget mode
     (only common properties are shown). Should be directly
     connected to Form::widgetSelected() signal.
     If \a forceReload is true, the the properties will be redisplayed in the property editor
     even if these were already displayed.
     If \a showPropertySet is true (the default), property editor will be updated for the current selection.
     This flag is set to false when we're selecting multiple widgets. */
    void setSelectedWidget(QWidget *w, bool add = false, bool forceReload = false,
                           bool moreWillBeSelected = false);

    void setSelectedWidgetWithoutReload(QWidget *w, bool add = false, bool moreWillBeSelected = false) {
        setSelectedWidget(w, add, false, moreWillBeSelected);
    }

    /*!  This function is called every time a property is modifed.  It also takes
     care of saving set and enum properties. */
    void slotPropertyChanged(KoProperty::Set& set, KoProperty::Property& property);

    /*! This slot is called when a property is reset using the "reload" button in PropertyEditor. */
    void slotPropertyReset(KoProperty::Set& set, KoProperty::Property& property);

    /*! This slot is called when the watched widget is destroyed. Resets the buffer.*/
    void slotWidgetDestroyed();

//  void setPropertyValueInDesignMode(QWidget* widget, const QMap<QByteArray, QVariant> &propValues,
    void createPropertyCommandsInDesignMode(QWidget* widget, 
                                            const QHash<QByteArray, QVariant> &propValues,
                                            CommandGroup *group, bool addToActiveForm = true,
                                            bool execFlagForSubCommands = false);

signals:
    /*! This signal is emitted when a property was changed.
      \a widg is the widget concerned, \a property
      is the name of the modified property, and \a v is the new value of this property. */
    void widgetPropertyChanged(QWidget *w, const QByteArray &property, const QVariant &v);

    /*! This signal is emitted when the name of the widget is modified.
    \a oldname is the name of the widget before the
      change, \a newname is the name after renaming. */
    void widgetNameChanged(const QByteArray &oldname, const QByteArray &newname);

protected:
    /*! Adds the widget in d->widgets, and updates property visibilty. */
    void addWidget(QWidget *w);

    /*! Fills the list with properties related to the widget \a w. Also updates
    properties old value and changed state. */
    void createPropertiesForWidget(QWidget *w);

    /*! Creates a map property description->prop. value from
     the list of keys \a list. */
    KoProperty::Property::ListData* createValueList(WidgetInfo *winfo,
            const QStringList &list);

    /*! Changes \a property old value and changed state, using the value
    stored in \a tree. Optional \a meta can be specified if you need to handle enum values. */
    void updatePropertyValue(ObjectTreeItem *tree, const char *property,
                             const QMetaProperty &meta = QMetaProperty());

    /*! \return the property list hold by this object. Do not modify the list,
     just use this method to change Editor's list. */
    KoProperty::Set*  set();

    /*! Clears the set, and reset all members. */
    void clearSet(bool dontSignalShowPropertySet = false);

    /*! Saves old values of modified properties in ObjectTreeItem, so
     that we can restore them later.*/
    void saveModifiedProperties();

    /*! Checks if the name entered by user is valid, ie that it is
     a valid identifier, and that there is no name conflict.  */
    bool isNameValid(const QString &name);

    /*! Saves 'enabled' property, and takes care of updating widget's palette. */
    void saveEnabledProperty(bool value);

    /*! This function filters the event of the selected widget to
    automatically updates the "geometry" property
      when the widget is moved or resized in the Form. */
    bool eventFilter(QObject *o, QEvent *ev);

    /*! Changes undoing state of the list. Used by Undo command to
     prevent recursion. */
    void setUndoing(bool isUndoing);

    bool isUndoing();

    /*! This function is used to filter the properties to be shown
       (ie not show "caption" if the widget isn't toplevel).
       \return true if the property should be shown. False otherwise.*/
    bool isPropertyVisible(const QByteArray &property, bool isTopLevel,
                           const QByteArray &classname = QByteArray());

    // Following functions are used to create special types of properties, different
    // from Q_PROPERTY

    /*! Creates the properties related to alignment (ie hAlign, vAlign and WordBreak) for
     the QWidget \a widget. \a subwidget is the same as \a widget if the widget itself handles
     the property and it's a child widget if the child handles the property.
     For example, the second case is true for KexiDBAutoField.
     \a meta  is the QMetaProperty for "alignment" property" of subwidget.  */
    void createAlignProperty(const QMetaProperty &meta, QWidget *widget, QWidget *subwidget);

    /*! Saves the properties related to alignment (ie hAlign, vAlign and WordBreak)
     and modifies the "alignment" property of  the widget.*/
    void saveAlignProperty(const QString &property);

    /*! Creates the "layout" property, for the Container representing \a item. */
    void createLayoutProperty(ObjectTreeItem *item);

    /*! Saves the "layout" property and changes the Container 's layout (
    using Container::setLayout() ).*/
    void saveLayoutProperty(const QString &property, const QVariant &value);

    // Some i18n functions
    //! Adds translations for general properties, by adding items in d->propDesc
    void initPropertiesDescription();

    /*! \return The i18n'ed name of the property whose name is \a name, that will be
    displayed in PropertyEditor. */
    QString propertyCaption(const QByteArray &name);

    /*! \return The i18n'ed name of the property's value whose name is \a name. */
    QString valueCaption(const QByteArray &name);

    /*! \return The i18n'ed list of values, that will be shown by Property
    Editor (using descFromValue()).*/
    //QStringList captionForList(const QStringList &list);

    //! Helper
    void emitWidgetPropertyChanged(QWidget *w, const QByteArray& property, const QVariant& value);

private:
    WidgetPropertySetPrivate *d;

    friend class FormManager;
    friend class PropertyCommand;
    friend class LayoutPropertyCommand;
    friend class GeometryPropertyCommand;
};

}

#endif
