/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>

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

#include "kexibrowseritem.h"

#include "kexipartinfo.h"

#include <kdebug.h>
#include <kiconloader.h>

KexiBrowserItem::KexiBrowserItem(KListView *parent, KexiPart::Info *i)
 : KListViewItem(parent, " "+ i->groupName())
{
	m_item = 0;
	m_info = i;
	setPixmap(0, SmallIcon(i->groupIcon()));
	setOpen(true);
	setSelectable(false);
	initItem();
	m_fifoSorting = 1; //because this is top level item
}

KexiBrowserItem::KexiBrowserItem(KListViewItem *parent, KexiPart::Info *i, KexiPart::Item *item)
 : KListViewItem(parent, " "+ item->name())
{
	m_item = item;
	m_info = i;
	setPixmap(0, SmallIcon(i->itemIcon()));
	initItem();
}

void KexiBrowserItem::initItem()
{
	m_fifoSorting = 0;
	int sortKey = 0;
	// set sorting key with FIFO order
	if (parent()) {
		sortKey = parent()->childCount();
	} else if (listView()) {
		sortKey = listView()->childCount();
	}
	m_sortKey.sprintf("%2.2d",sortKey);
	kdDebug() << "m_sortKey=" << m_sortKey << endl;
}

void
KexiBrowserItem::clearChildren()
{
	KexiBrowserItem* child;
	
	while((child = static_cast<KexiBrowserItem*>(firstChild())))
	{
		delete child;
	}
}

QString KexiBrowserItem::key( int column, bool ascending ) const
{
	return m_fifoSorting ? m_sortKey : KListViewItem::key(column,ascending);
}

