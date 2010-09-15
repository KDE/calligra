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

#include <kdebug.h>
#include <qwidget.h>
#include <qvariant.h>
#include <qdom.h>
#include <qtextstream.h>

#include "form.h"
#include "container.h"
#include "objecttree.h"


using namespace KFormDesigner;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////        ObjectTreeItem                                    /////////////
////////////////////////////////////////////////////////////////////////////////////////


ObjectTreeItem::ObjectTreeItem(const QString &classn, const QString &name, QWidget *widget,
                               Container *parentContainer, Container *container)
        : m_enabled(true), m_row(-1), m_col(-1), m_rowspan(-1), m_colspan(-1), m_span(false)
{
    kDebug() << classn << name << widget->objectName() << "parentContainer:" << parentContainer << "container:" << container;
    m_className = classn;
    m_name = name;
    m_widget = widget;
    m_container = container;
    m_eater = new EventEater(widget, parentContainer);
    m_parent = 0;
    m_subprops = 0;
}

ObjectTreeItem::~ObjectTreeItem()
{
// kDebug() << "ObjectTreeItem deleted: " << name();
    delete m_subprops;
}

void
ObjectTreeItem::rename(const QString &name)
{
    m_name = name;
}

void
ObjectTreeItem::addChild(ObjectTreeItem *c)
{
    m_children.append(c);
    c->setParent(this);
}

void
ObjectTreeItem::removeChild(ObjectTreeItem *c)
{
    m_children.removeAt( m_children.indexOf(c) );
}

void
ObjectTreeItem::addModifiedProperty(const QByteArray &property, const QVariant &oldValue)
{
    if (property == "objectName")
        return;

    if (!m_props.contains(property)) {
        m_props.insert(property, oldValue);
        kDebug() << "Added this property in the list: " << property << " oldValue: " << oldValue;
    }
}

void
ObjectTreeItem::addSubproperty(const QByteArray &property, const QVariant& value)
{
    if (!m_subprops)
        m_subprops = new QHash<QString, QVariant>();
    if (!m_props.contains(property))
        m_subprops->insert(property, value);
}

void
ObjectTreeItem::storeUnknownProperty(QDomElement &el)
{
    if (!el.isNull()) {
        QTextStream ts(&m_unknownProps, QIODevice::WriteOnly | QIODevice::Append);
        el.save(ts, 0);
    }
}

void
ObjectTreeItem::setPixmapName(const QByteArray &property, const QString &name)
{
    m_pixmapNames.insert(property, name);
}

QString
ObjectTreeItem::pixmapName(const QByteArray &property)
{
    return m_pixmapNames.value(property);
}

void
ObjectTreeItem::setGridPos(int row, int col, int rowspan, int colspan)
{
    m_row = row;  m_col = col;
    m_rowspan = rowspan;
    m_colspan = colspan;
    if (colspan || rowspan)
        m_span = true;
    else
        m_span = false;
}

ObjectTreeItem* ObjectTreeItem::selectableItem()
{
    if (parent() && parent()->widget()) {
        if (qobject_cast<QTabWidget*>(parent()->widget())) {
            // tab widget's page
            return parent();
        }
    }
    return this;
}

/////////////////////////////////////////////////////////////////////////////////////////
///                      ObjectTree                                             /////////
////////////////////////////////////////////////////////////////////////////////////////

ObjectTree::ObjectTree(const QString &classn, const QString &name, QWidget *widget, Container *container)
        : ObjectTreeItem(classn, name, widget, container, container)
{
}

ObjectTree::~ObjectTree()
{
    while (!children()->isEmpty()) {
        removeItem(children()->first());
    }
}

bool
ObjectTree::rename(const QString &oldname, const QString &newname)
{
    if (oldname == m_name) {
        ObjectTreeItem::rename(newname);
        return true;
    }

    ObjectTreeItem *it = lookup(oldname);
    if (!it)
        return false;

    it->rename(newname);
    m_treeHash.remove(oldname);
    m_treeHash.insert(newname, it);

    return true;
}

bool
ObjectTree::reparent(const QString &name, const QString &newparent)
{
    ObjectTreeItem *item = lookup(name);
    if (!item)   return false;
    ObjectTreeItem *parent = lookup(newparent);
    if (!parent)   return false;

    item->parent()->removeChild(item);
    parent->addChild(item);
    return true;
}

ObjectTreeItem*
ObjectTree::lookup(const QString &name)
{
    if (name == this->name())
        return this;
    else
        return m_treeHash.value(name);
}

void
ObjectTree::addItem(ObjectTreeItem *parent, ObjectTreeItem *c)
{
    m_treeHash.insert(c->name(), c);

    if (!parent)
        parent = this;
    parent->addChild(c);
    m_container->form()->emitChildAdded(c);

    kDebug() << "adding " << c->name() << " to " << parent->name();
}

void
ObjectTree::removeItem(const QString &name)
{
    ObjectTreeItem *c = lookup(name);
    removeItem(c);
}

void
ObjectTree::removeItem(ObjectTreeItem *c)
{
    if (m_container && m_container->form())
        m_container->form()->emitChildRemoved(c);

    foreach (ObjectTreeItem *titem, *c->children()) {
        removeItem(titem->name());
    }

    m_treeHash.remove(c->name());
    c->parent()->removeChild(c);
    delete c;
}

QByteArray
ObjectTree::generateUniqueName(const QByteArray &prefix, bool numberSuffixRequired)
{
    /* old way of naming widgets
    int appendix = m_names[c] + 1;
    QString name(c);
    name.append(QString::number(appendix));
    m_names[c] = appendix;*/
    if (!numberSuffixRequired && !lookup(prefix))
        return prefix;
    QString name(prefix);
    int i = 2; //start from 2, i.e. we have: "widget", "widget2", etc.
    while (lookup(name + QString::number(i)))
        i++;

    return (name + QString::number(i)).toLatin1();
}

