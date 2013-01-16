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
#include <QWidget>
#include <QVariant>
#include <QDomDocument>
#include <QTextStream>

#include "form.h"
#include "container.h"
#include "objecttree.h"


using namespace KFormDesigner;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////        ObjectTreeItem                                    /////////////
////////////////////////////////////////////////////////////////////////////////////////

class ObjectTreeItem::Private
{
public:
    Private(const QString &classn, const QString &name_, QWidget *widget_,
            Container *parentContainer_, Container *c);

    ~Private();

    QString className;
    QString name;
    ObjectTreeList children;
    QPointer<Container> container;
    QHash<QString, QVariant> props;
    QHash<QString, QVariant> *subprops;
    QString  unknownProps;
    QHash<QByteArray, QString> pixmapNames;
    ObjectTreeItem* parent;
    QPointer<QWidget> widget;
    QPointer<EventEater> eater;

    bool  enabled;

    int row, col, rowspan, colspan;
    bool span;
};

ObjectTreeItem::Private::Private(const QString &classn, const QString &name_, QWidget *widget_,
                        Container *parentContainer_, Container *c)
    : className(classn), name(name_), container(c)
    , subprops(0), parent(0), widget(widget_), eater(new EventEater(widget_, parentContainer_))
    , enabled(true), row(-1), col(-1), rowspan(-1), colspan(-1), span(false)
{

}

ObjectTreeItem::Private::~Private()
{
    delete subprops;
}

ObjectTreeItem::ObjectTreeItem(const QString &classn, const QString &name, QWidget *widget,
                               Container *parentContainer, Container *container)
    : d(new Private(classn, name, widget, parentContainer, container))
{
    kDebug() << classn << name << widget->objectName() << "parentContainer:" << parentContainer << "container:" << container;
}

ObjectTreeItem::~ObjectTreeItem()
{
    delete d;
}

void
ObjectTreeItem::rename(const QString &name)
{
    d->name = name;
}

void
ObjectTreeItem::addChild(ObjectTreeItem *c)
{
    d->children.append(c);
    c->setParent(this);
}

void
ObjectTreeItem::removeChild(ObjectTreeItem *c)
{
    d->children.removeAt( d->children.indexOf(c) );
}

void
ObjectTreeItem::addModifiedProperty(const QByteArray &property, const QVariant &oldValue)
{
    if (property == "objectName")
        return;

    if (!d->props.contains(property)) {
        d->props.insert(property, oldValue);
        kDebug() << "Added this property in the list: " << property << " oldValue: " << oldValue;
    }
}

void
ObjectTreeItem::addSubproperty(const QByteArray &property, const QVariant& value)
{
    if (!d->subprops)
        d->subprops = new QHash<QString, QVariant>();
    if (!d->props.contains(property))
        d->subprops->insert(property, value);
}

void
ObjectTreeItem::storeUnknownProperty(QDomElement &el)
{
    if (!el.isNull()) {
        QTextStream ts(&d->unknownProps, QIODevice::WriteOnly | QIODevice::Append);
        el.save(ts, 0);
    }
}

QString ObjectTreeItem::unknownProperties()
{
    return d->unknownProps;
}

void ObjectTreeItem::setUnknownProperties(const QString &set)
{
    d->unknownProps = set;
}

void
ObjectTreeItem::setPixmapName(const QByteArray &property, const QString &name)
{
    d->pixmapNames.insert(property, name);
}

QString
ObjectTreeItem::pixmapName(const QByteArray &property)
{
    return d->pixmapNames.value(property);
}

void
ObjectTreeItem::setGridPos(int row, int col, int rowspan, int colspan)
{
    d->row = row;  d->col = col;
    d->rowspan = rowspan;
    d->colspan = colspan;
    if (colspan || rowspan)
        d->span = true;
    else
        d->span = false;
}

QHash<QString, QVariant>* ObjectTreeItem::subproperties() const
{
    return d->subprops;
}

void ObjectTreeItem::setEnabled(bool enabled)
{
    d->enabled = enabled;
}

bool ObjectTreeItem::isEnabled() const
{
    return d->enabled;
}

int ObjectTreeItem::gridRow() const
{
    return d->row;
}

int ObjectTreeItem::gridCol() const
{
    return d->col;
}

int ObjectTreeItem::gridRowSpan() const
{
    return d->rowspan;
}

int ObjectTreeItem::gridColSpan() const
{
    return d->colspan;
}

bool ObjectTreeItem::spanMultipleCells() const
{
    return d->span;
}

QString ObjectTreeItem::name() const
{
    return d->name;
}

QString ObjectTreeItem::className() const
{
    return d->className;
}

QWidget* ObjectTreeItem::widget() const
{
    return d->widget;
}

EventEater* ObjectTreeItem::eventEater() const
{
    return d->eater;
}

ObjectTreeItem* ObjectTreeItem::parent() const
{
    return d->parent;
}

ObjectTreeList* ObjectTreeItem::children()
{
    return &d->children;
}

const QHash<QString, QVariant>* ObjectTreeItem::modifiedProperties() const
{
    return &d->props;
}

Container* ObjectTreeItem::container() const
{
    return d->container;
}

void ObjectTreeItem::setWidget(QWidget *w)
{
    d->widget = w;
}

void ObjectTreeItem::setParent(ObjectTreeItem *parent)
{
    d->parent = parent;
}


/////////////////////////////////////////////////////////////////////////////////////////
///                      ObjectTree                                             /////////
////////////////////////////////////////////////////////////////////////////////////////

class ObjectTree::Private
{
public:
    Private()
    {

    }

    ~Private()
    {

    }

    ObjectTreeHash treeHash;
};

ObjectTree::ObjectTree(const QString &classn, const QString &name, QWidget *widget, Container *container)
    : ObjectTreeItem(classn, name, widget, container, container), d(new Private())
{
}

ObjectTree::~ObjectTree()
{
    while (!children()->isEmpty()) {
        removeItem(children()->first());
    }
    delete d;
}

bool
ObjectTree::rename(const QString &oldname, const QString &newname)
{
    if (oldname == name()) {
        ObjectTreeItem::rename(newname);
        return true;
    }

    ObjectTreeItem *it = lookup(oldname);
    if (!it)
        return false;

    it->rename(newname);
    d->treeHash.remove(oldname);
    d->treeHash.insert(newname, it);

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
        return d->treeHash.value(name);
}

void
ObjectTree::addItem(ObjectTreeItem *parent, ObjectTreeItem *c)
{
    d->treeHash.insert(c->name(), c);

    if (!parent)
        parent = this;
    parent->addChild(c);
    container()->form()->emitChildAdded(c);

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
    if (container() && container()->form())
        container()->form()->emitChildRemoved(c);

    foreach (ObjectTreeItem *titem, *c->children()) {
        removeItem(titem->name());
    }

    d->treeHash.remove(c->name());
    c->parent()->removeChild(c);
    delete c;
}

QByteArray
ObjectTree::generateUniqueName(const QByteArray &prefix, bool numberSuffixRequired)
{
    if (!numberSuffixRequired && !lookup(prefix))
        return prefix;
    QString name(prefix);
    int i = 2; //start from 2, i.e. we have: "widget", "widget2", etc.
    while (lookup(name + QString::number(i)))
        i++;

    return (name + QString::number(i)).toLatin1();
}

ObjectTreeHash* ObjectTree::hash()
{
    return &d->treeHash;
}
