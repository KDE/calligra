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

#include "form.h"
#include "container.h"
#include "objecttree.h"


namespace KFormDesigner
{

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////        ObjectTreeItem                                    /////////////
////////////////////////////////////////////////////////////////////////////////////////


ObjectTreeItem::ObjectTreeItem(const QString &classn, const QString &name, QWidget *widget, Container *container)
 : m_row(-1), m_col(-1), m_rowspan(-1), m_colspan(-1), m_span(false)
{
	m_className = classn;
	m_name = name;
	m_widget = widget;
	m_container = container;
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
	kdDebug() << "ObjectTreeItem::addChild()" << endl;
	m_children.append(c);
}

void
ObjectTreeItem::remChild(ObjectTreeItem *c)
{
	m_children.remove(c);
}

void
ObjectTreeItem::debug(int ident)
{
	for(ObjectTreeItem *it = m_children.first(); it; it = m_children.next())
	{
		QString str;
		for(int i=0; i < ident; i++)
			str += " ";

		kdDebug() << str << it->className().latin1() << " : " << it->name().latin1() << " (" << it->parent()->name().latin1() << ")" << endl;
		it->debug(ident + 4);
	}
}

void
ObjectTreeItem::addModProperty(const QString &property, const QVariant &oldValue)
{
	if(property == "name")
		return;

	if(!m_props.contains(property))
	{
		m_props.insert(property, oldValue);
		kdDebug() << "ObjectTree::adModProperty() added the property in my list" << property << endl;
	}
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
 : ObjectTreeItem(classn, name, widget, container)
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

ObjectTreeItem*
ObjectTree::lookup(const QString &name)
{
	if(name == this->name())
		return this;
	else
		return m_treeDict[name];
}

void
ObjectTree::addChild(ObjectTreeItem *parent, ObjectTreeItem *c)
{
	kdDebug() << "ObjectTree::addChild()" << endl;
	m_treeDict.insert(c->name(), c);
	if(!parent)
	{
		kdDebug() << "*************************************************" << endl;
		kdDebug() << "* ObjectTree::addChild(): no parent!            *" << endl;
		kdDebug() << "*************************************************" << endl;
	}
	else
	{
		parent->addChild(c);
		c->setParent(parent);
	}
	kdDebug() << "ObjectTree::addChild(): adding " << c->name() << " to " << parent->name() << endl;
	m_container->form()->emitChildAdded(c);
}

void
ObjectTree::addChild(ObjectTreeItem *c)
{
	ObjectTreeItem::addChild(c);
	kdDebug() << "ObjectTree::addChild(): count is now: " << children()->count() << endl;
}

void
ObjectTree::removeChild(const QString &name)
{
	kdDebug() << "ObjectTree:: remove the object item " << name << endl;
	ObjectTreeItem *c = lookup(name);
	m_container->form()->emitChildRemoved(c);
	for(ObjectTreeItem *it = c->children()->first(); it; it = c->children()->next())
		removeChild(it->name());
	m_treeDict.remove(name);
	c->parent()->remChild(c);
	delete c;
}

QString
ObjectTree::genName(const QString &c)
{
	int appendix = m_names[c] + 1;
	QString name(c);
	name.append(QString::number(appendix));
	m_names[c] = appendix;
	return name;
}

void
ObjectTree::debug()
{
	kdDebug() << "ObjectTree::debug(): tree dumb" << endl;
	ObjectTreeItem::debug(0);
}

ObjectTree::~ObjectTree()
{
	for(ObjectTreeItem *it = children()->first(); it; it = children()->next())
		removeChild(it->name());
}

}
