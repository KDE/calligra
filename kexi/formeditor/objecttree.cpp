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

#include "objecttree.h"


namespace KFormDesigner
{

ObjectTreeItem::ObjectTreeItem(const QString &classn, const QString &name)
{
	m_className = classn;
	m_name = name;
}

bool
ObjectTreeItem::rename(const QString &name)
{
	//do something useful (e.g. update the list etc)
	return true;
}

ObjectTreeItem::~ObjectTreeItem()
{
	// delete all children...
	// take me out of the dict... :)
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
	parent->children().append(c);
}

void
ObjectTree::addChild(ObjectTreeItem *c)
{
	m_treeDict.insert(c->name(), c);
	children().append(c);
}

QString
ObjectTree::genName(const QString &c)
{
	int appendix = m_names[c];
	QString name = c + QString::number(appendix);
	while(m_treeDict[name])
	{
		appendix++;
		name = c + QString::number(appendix);
	}

	m_names.insert(c, appendix);
	return name;
}

ObjectTree::~ObjectTree()
{
}

}
