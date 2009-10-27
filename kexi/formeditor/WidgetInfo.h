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

#ifndef KFORMDESIGNERWIDGETINFO_H
#define KFORMDESIGNERWIDGETINFO_H

#include <QObject>
#include <QPointer>
#include <QHash>

#include <kexi_export.h>
#include <kexiutils/tristate.h>

namespace KFormDesigner
{

class WidgetFactory;
class WidgetLibrary;

//! A class providing properties of widget classes offered by a factory
class KFORMEDITOR_EXPORT WidgetInfo
{
public:
    WidgetInfo(WidgetFactory *f);

//2.0    WidgetInfo(WidgetFactory *f, const char* parentFactoryName, const char* inheritedClassName = 0);

    virtual ~WidgetInfo();

    //! \return a pixmap associated with the widget
    QString pixmap() const;

    void setPixmap(const QString &p);

    //! @return the class name of a widget e.g. "QLineEdit"
    QByteArray className() const;

    void setClassName(const QByteArray& className);

    QByteArray inheritedClassName() const;

    void setInheritedClassName(const QByteArray& inheritedClassName);

    /*! \return the name used to name widget, that will appear eg in scripts (must not contain spaces
      nor non-latin1 characters) */
    QString namePrefix() const;

    void setNamePrefix(const QString &n);

    //! \return the real name e.g. 'Line Edit', showed eg in ObjectTreeView
    QString name() const;

    void setName(const QString &n);

    QString description() const;

    void setDescription(const QString &desc);

    QString includeFileName() const;

    /*! Sets the C++ include file corresponding to this class,
     that uic will need to add when creating the file. You don't have to set this for Qt std widgets.*/
    void setIncludeFileName(const QString &name);

    QList<QByteArray> alternateClassNames() const;

    QByteArray savingName() const;

    WidgetFactory *factory() const;

    /*! Sets alternate names for this class.
     If this name is found when loading a .ui file, the className() will be used instead.
     It allows to support both KDE and Qt versions of widget, without duplicating code.
     As a rule, className() should always return a class name which is inherited from
     alternate class. For example KLineEdit class has alternate QLineEdit class.

     \a override parameter overrides class name of a widget,
     even if it was implemented in other factory.
     By default it's set to false, what means that no other class is overridden
     by this widget class if there is already a class implementing it
     (no matter in which factory).
     By forced overriding existing class with other - custom, user
     will be able to see more or less properties and experience different behaviour.
     For example, in Kexi application, KLineEdit class contains additional
     "datasource" property for binding to database sources.
    */
    void addAlternateClassName(const QByteArray& alternateName, bool override = false);

    /*! \return true is a class \a alternateName is defined as alternate name with
     'override' flag set to true, using addAlternateClassName().
     If this flag is set to false (the default) or there's no such alternate class
     name defined. */
    bool isOverriddenClassName(const QByteArray& alternateName) const;

    /*! Sets the name that will be written in the .ui file when saving.
     This name must be one of alternate names (or loading will be impossible).

     On form data saving to XML .ui format, saveName is used instead,
     so .ui format is not broken and still usable with other software as Qt Designer.
     Custom properties are saved as well with 'stdset' attribute set to 0. */
    void setSavingName(const QByteArray &saveName);

    /*! Sets autoSync flag for property \a propertyName.
     This allows to override autoSync flag for certain widget's property, because
     e.g. KoProperty::EditorView can have autoSync flag set to false or true, but
     not all properties have to comply with that.
     \a flag equal to cancelled value means there is no overriding (the default). */
    void setAutoSyncForProperty(const char *propertyName, tristate flag);

    /*! \return autoSync override value (true or false) for \a propertyName.
     If cancelled value is returned, there is no overriding (the default). */
    tristate autoSyncForProperty(const char *propertyName) const;

    QByteArray parentFactoryName() const;

    void setParentFactoryName(const QByteArray& factoryName);

    WidgetInfo* inheritedClass() const;

    /*! Sets custom type \a type for property \a propertyName.
     This allows to override default type, especially when custom property
     and custom property editor item has to be used. */
    void setCustomTypeForProperty(const char *propertyName, int type);

    /*! \return custom type for property \a propertyName. If no specific custom type has been assigned,
     KoProperty::Auto is returned.
     @see setCustomTypeForProperty() */
    int customTypeForProperty(const char *propertyName) const;

protected:
    void setInheritedClass(WidgetInfo *inheritedClass);

private:
    class Private;
    Private * const d;

    friend class WidgetLibrary;
};

typedef QList<WidgetInfo*> WidgetInfoList;
typedef QHash<QByteArray, WidgetInfo*> WidgetInfoHash;

} // namespace KFormDesigner

#endif
