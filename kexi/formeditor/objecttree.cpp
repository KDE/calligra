/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <kdebug.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qvariant.h>
#include <qdom.h>
#include <qtextstream.h>

#include "form.h"
#include "container.h"
#include "objecttree.h"


namespace KFormDesigner
{

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////        ObjectTreeItem                                    /////////////
////////////////////////////////////////////////////////////////////////////////////////


ObjectTreeItem::ObjectTreeItem(const QString &classn, const QString &name, QWidget *widget, Container *parentContainer, Container *container)
 : m_row(-1), m_col(-1), m_rowspan(-1), m_colspan(-1), m_span(false)
{
	m_className = classn;
	m_name = name;
	m_widget = widget;
	m_container = container;
	m_eater = new EventEater(widget, parentContainer);
	m_parent = 0;
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
	m_children.remove(c);
}

void
ObjectTreeItem::addModifiedProperty(const QString &property, const QVariant &oldValue)
{
	if(property == "name")
		return;

	if(!m_props.contains(property))
	{
		m_props.insert(property, oldValue);
		//kdDebug() << "ObjectTree::adModProperty(): Added this property in the list: " << property << endl;
	}
}

void
ObjectTreeItem::storeUnknownProperty(QDomElement &el)
{
	if(!el.isNull()) {
		QTextStream ts(m_unknownProps, IO_WriteOnly|IO_Append );
		el.save(ts, 0);
	}
}

void
ObjectTreeItem::addPixmapName(const QString &property, const QString &name)
{
	m_pixmapNames[property] = name;
}

QString
ObjectTreeItem::pixmapName(const QString &property)
{
	if(m_pixmapNames.contains(property))
		return m_pixmapNames[property];
	return QString::null;
}

void
ObjectTreeItem::setGridPos(int row, int col, int rowspan, int colspan)
{
	m_row = row;  m_col = col;
	m_rowspan = rowspan;
	m_colspan = colspan;
	if(colspan || rowspan)
		m_span = true;
	else
		m_span = false;
}

ObjectTreeItem::~ObjectTreeItem()
{
	kdDebug() << "ObjectTreeItem deleted: " << this->name() << endl;
}

/////////////////////////////////////////////////////////////////////////////////////////
///                      ObjectTree                                             /////////
////////////////////////////////////////////////////////////////////////////////////////

ObjectTree::ObjectTree(const QString &classn, const QString &name, QWidget *widget, Container *container)
 : ObjectTreeItem(classn, name, widget, container, container)
{
}

bool
ObjectTree::rename(const QString &oldname, const QString &newname)
{
	if(lookup(newname))
		return false;

	if(oldname == m_name)
	{
		ObjectTreeItem::rename(newname);
		return true;
	}

	ObjectTreeItem *it = lookup(oldname);
	it->rename(newname);
	m_treeDict.remove(oldname);
	m_treeDict.insert(newname, it);

	return true;
}

bool
ObjectTree::reparent(const QString &name, const QString &newparent)
{
	ObjectTreeItem *item = lookup(name);
	if(!item)   return false;
	ObjectTreeItem *parent = lookup(newparent);
	if(!parent)   return false;

	item->parent()->removeChild(item);
	parent->addChild(item);
	return true;
}

ObjectTreeItem*
ObjectTree::lookup(const QString &name)
{
	if(name == this->name())
		return this;
	else
		return m_treeDict[name];
}

void
ObjectTree::addItem(ObjectTreeItem *parent, ObjectTreeItem *c)
{
	m_treeDict.insert(c->name(), c);

	if(!parent)
		parent = this;
	parent->addChild(c);
	m_container->form()->emitChildAdded(c);

	kdDebug() << "ObjectTree::addItem(): adding " << c->name() << " to " << parent->name() << endl;
}

void
ObjectTree::removeItem(const QString &name)
{
	ObjectTreeItem *c = lookup(name);

	if (m_container && m_container->form())
		m_container->form()->emitChildRemoved(c);

	for(ObjectTreeItem *it = c->children()->first(); it; it = c->children()->next())
		removeItem(it->name());

	m_treeDict.remove(name);
	c->parent()->removeChild(c);
	delete c;
}

QString
ObjectTree::genName(const QString &c)
{
	/* old way of naming widgets
	int appendix = m_names[c] + 1;
	QString name(c);
	name.append(QString::number(appendix));
	m_names[c] = appendix;*/
	QString name(c + "1");
	int i = 1;

	while(lookup(name))
	{
		i++;
		name = name.left(name.length()-1) + QString::number(i);
	}

	return name;
}

ObjectTree::~ObjectTree()
{
	for(ObjectTreeItem *it = children()->first(); it; it = children()->next())
		removeItem(it->name());
}

}
