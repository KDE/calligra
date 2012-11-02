/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2006-2010 Jarosław Staniek <staniek@kde.org>

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

#ifndef KFORMDESIGNEROBJECTTREE_H
#define KFORMDESIGNEROBJECTTREE_H

#include <QList>
#include <QHash>
#include <QVariant>
#include <QString>
#include <QPointer>
#include <QByteArray>

#include <kexi_export.h>

#include "container.h"
//#include "eventeater.h"

class QWidget;
class QDomElement;

namespace KFormDesigner
{

class ObjectTreeItem;

//! @short An list of ObjectTreeItem pointers.
typedef QList<ObjectTreeItem*> ObjectTreeList;

//! @short A QString-based disctionary of ObjectTreeItem pointers.
typedef QHash<QString, ObjectTreeItem*> ObjectTreeHash;

/*!
 @short An item representing a widget
 Holds the properties of a widget (classname, name, parent, children ..).
 @author Lucijan Busch <lucijan@kde.org>
 */
class KFORMEDITOR_EXPORT ObjectTreeItem
{
public:
    ObjectTreeItem(const QString &className, const QString &name, QWidget *widget, Container *parentContainer, Container *container = 0);
    virtual ~ObjectTreeItem();

    QString name() const {
        return m_name;
    }
    QString className() const {
        return m_className;
    }
    QWidget* widget() const {
        return m_widget;
    }
    EventEater* eventEater() const {
        return m_eater;
    }
    ObjectTreeItem* parent() const {
        return m_parent;
    }
    ObjectTreeList* children() {
        return &m_children;
    }

    /*! \return a QHash<QString, QVariant> of all modified properties for this widget.
      The QVariant is the old value (ie first value) of the property whose name is the QString. */
    const QHash<QString, QVariant>* modifiedProperties() const {
        return &m_props;
    }

    //! \return the widget's Container, or 0 if the widget is not a Container.
    Container* container() const {
        return m_container;
    }

    void setWidget(QWidget *w) {
        m_widget = w;
    }
    void setParent(ObjectTreeItem *parent)  {
        m_parent = parent;
    }

    void debug(int ident);
    void rename(const QString &name);

    void addChild(ObjectTreeItem *it);
    void removeChild(ObjectTreeItem *it);

    /*! Adds \a property in the list of the modified properties for this object.
        These modified properties are written in the .ui files when saving the form.
    */
    void addModifiedProperty(const QByteArray &property, const QVariant &oldValue);
    void storeUnknownProperty(QDomElement &el);

    /*! Adds subproperty \a property value \a value (a property of subwidget).
     Remembering it for delayed setting is needed because on loading
     the subwidget could be not created yet (true e.g. for KexiDBAutoField). */
    void addSubproperty(const QByteArray &property, const QVariant& value);

    /*! \return subproperties for this item, added by addSubproperty()
     or 0 is there are no subproperties. */
    QHash<QString, QVariant>* subproperties() const {
        return m_subprops;
    }

    void setPixmapName(const QByteArray &property, const QString &name);
    QString pixmapName(const QByteArray &property);

    void setEnabled(bool enabled)  {
        m_enabled = enabled;
    }
    bool isEnabled() const {
        return m_enabled;
    }

    int gridRow() const {
        return m_row;
    }
    int gridCol() const {
        return m_col;
    }
    int gridRowSpan() const {
        return m_rowspan;
    }
    int gridColSpan() const {
        return m_colspan;
    }
    bool spanMultipleCells() const {
        return m_span;
    }
    void setGridPos(int row, int col, int rowspan, int colspan);

#if 0 // moved to WidgetLibrary
    //! @return selectable item for this item.
    //! In most cases it is equal to this item but e.g. for pages of QTabWidget,
    //! item for the widget itself is returned.
    //! Used when user clicks on widget tree item or when parent of the current
    //! widget should to be selected. Defaults can be overridden by widget factories.
    ObjectTreeItem* selectableItem();
#endif
protected:
    QString m_className;
    QString m_name;
    ObjectTreeList m_children;
    QPointer<Container> m_container;
    QHash<QString, QVariant> m_props;
    QHash<QString, QVariant> *m_subprops;
    QString  m_unknownProps;
    QHash<QByteArray, QString> m_pixmapNames;
    ObjectTreeItem* m_parent;
    QPointer<QWidget> m_widget;
    QPointer<EventEater> m_eater;

    bool  m_enabled;

    int m_row, m_col, m_rowspan, m_colspan;
    bool m_span;

    friend class ObjectTree;
    friend class FormIO;
};

/*! @short Represents all the objects available within a form.
 This class holds ObjectTreeItem for each widget in a Form. */
class KFORMEDITOR_EXPORT ObjectTree : public ObjectTreeItem
{
public:
    ObjectTree(const QString &className = QString(), const QString &name = QString(),
               QWidget *widget = 0, Container *container = 0);
    virtual ~ObjectTree();

    /*! Renames the item named \a oldname to \a newname. \return false if widget named \a newname
     already exists and renaming failed. */
    bool rename(const QString &oldname, const QString &newname);
    /*! Sets \a newparent as new parent for the item whose name is \a name. */
    bool reparent(const QString &name, const QString &newparent);

    /*! \return the ObjectTreeItem named \a name, or 0 if doesn't exist. */
    ObjectTreeItem* lookup(const QString &name);

    /*! \return a hash containing all ObjectTreeItem in this ObjectTree. */
    ObjectTreeHash* hash() {
        return &m_treeHash;
    }

    void addItem(ObjectTreeItem *parent, ObjectTreeItem *c);
    void removeItem(const QString &name);
    void removeItem(ObjectTreeItem *c);

    /*! Generates a new, unique name for a new widget using prefix \a prefix
     (e.g. if \a prefix is "lineEdit", "lineEdit1" is returned).
     \a prefix must be a valid identifier.
     If \a numberSuffixRequired is true (the default) a number suffix is mandatory.
     If \a numberSuffixRequired is false and there's a widget prefix \a prefix,
     then \a prefix is returned (e.g. if \a prefix is "lineEdit", and "lineEdit" doesn't exist yet,
     "lineEdit" is returned). */
    QByteArray generateUniqueName(const QByteArray &prefix, bool numberSuffixRequired = true);

private:
    ObjectTreeHash m_treeHash;
};

}

#endif
