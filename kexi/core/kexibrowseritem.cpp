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

#include "kexiprojecthandler.h"
#include "kexiprojecthandlerproxy.h"
#include "kexiprojecthandleritem.h"

#include "kexibrowseritem.h"

#include <kdebug.h>

KexiBrowserItem::KexiBrowserItem(KListView *parent, QString mime, QString name, QString identifier)
 : KListViewItem(parent, name)
{
	m_mime = mime;
	m_name = name;
	m_identifier = identifier;

	m_proxy = 0;
	m_item = 0;
	
	initItem();
	m_fifoSorting = 1; //because this is top level item
}

KexiBrowserItem::KexiBrowserItem(KListView *parent, KexiProjectHandlerProxy *proxy)
 : KListViewItem(parent, proxy->part()->name())
{
	m_mime = proxy->part()->mime();
	m_name = proxy->part()->name();
	m_identifier = QString::null;

	m_proxy = proxy;
	m_item = 0;
	
	initItem();
	m_fifoSorting = 1; //because this is top level item
}

KexiBrowserItem::KexiBrowserItem(KListView *parent, KexiProjectHandlerItem *item)
 : KListViewItem(parent, item->title())
{
	m_mime = item->mime();
	m_name = item->title();
	m_identifier = item->fullIdentifier();
	m_item = item;
	m_proxy = 0;
	
	initItem();
	m_fifoSorting = 1; //because this is top level item
}

KexiBrowserItem::KexiBrowserItem(KListViewItem *parent, QString mime, QString name, QString identifier)
 : KListViewItem(parent, name)
{
	m_mime = mime;
	m_name = name;
	m_identifier = identifier;
	m_proxy = 0;
	m_item = 0;
	
	initItem();
}

KexiBrowserItem::KexiBrowserItem(KListViewItem *parent, KexiProjectHandlerItem *item)
 : KListViewItem(parent, item->title())
{
	m_mime = item->mime();
	m_name = item->title();
	m_identifier = item->fullIdentifier();
	m_item = item;
	m_proxy = 0;
	
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

KexiProjectHandlerProxy*
KexiBrowserItem::proxy()
{
	return m_proxy;
}

KexiProjectHandlerItem*
KexiBrowserItem::item()
{
	return m_item;
}

QString
KexiBrowserItem::mime()
{
	return m_mime;
}

QString
KexiBrowserItem::name()
{
	return m_name;
}

QString
KexiBrowserItem::identifier()
{
	return m_identifier;
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

