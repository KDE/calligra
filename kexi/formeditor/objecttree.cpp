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
#include <iostream>
#include <kdebug.h>

#include "objecttree.h"


namespace KFormDesigner
{

ObjectTreeItem::ObjectTreeItem(const QString &classn, const QString &name)
{
	m_className = classn;
	m_name = name;
}

bool
ObjectTreeItem::rename(const QString &/*name*/)
{
	//do something useful (e.g. update the list etc)
	return true;
}

ObjectTreeItem::~ObjectTreeItem()
{
	// delete all children...
	// take me out of the dict... :)
}

void
ObjectTreeItem::addChild(ObjectTreeItem *c)
{
	m_children.append(c);
}

void
ObjectTreeItem::debug(int ident)
{
	for(ObjectTreeItem *it = m_children.first(); it; it = m_children.next())
	{
		for(int i=0; i < ident; i++)
			std::cerr << " ";

		qDebug("%s (%s)", it->className().latin1(), it->name().latin1());
		it->debug(ident + 4);
	}
}

/* object tree */

ObjectTree::ObjectTree(const QString &classn, const QString &name)
 : ObjectTreeItem(classn, name)
{
}

bool
ObjectTree::rename(const QString &name)
{
	//do something useful (e.g. update the list etc)
	return true;
}

ObjectTreeItem*
ObjectTree::lookup(const QString &name)
{
	return m_treeDict[name];
}

void
ObjectTree::addChild(ObjectTreeItem *parent, ObjectTreeItem *c)
{
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
	}
	kdDebug() << "ObjectTree::addChild(): adding " << c->name() << " to " << parent->name() << endl;
}

void
ObjectTree::addChild(ObjectTreeItem *c)
{
	m_treeDict.insert(c->name(), c);
	ObjectTreeItem::addChild(c);
	kdDebug() << "ObjectTree::addChild(): count is now: " << children().count() << endl;
}

void
ObjectTree::removeChild(const QString &name)
{
	ObjectTreeItem *c = lookup(name);
	m_treeDict.remove(name);
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
}

}
