/* This file is part of the KDE project
   Copyright (C) 2002-2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

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

#include "KexiProjectModelItem.h"


#include <core/kexipartinfo.h>
#include <qalgorithms.h>
#include <kdebug.h>
#include <kiconloader.h>

KexiProjectModelItem::KexiProjectModelItem(const QString& n, KexiProjectModelItem *p) : m_info(0), m_item(0), m_parentItem(p)
{
    m_groupName = n;
}

KexiProjectModelItem::KexiProjectModelItem(KexiPart::Info &i, KexiProjectModelItem *p) : m_info(&i), m_item(0), m_parentItem(p)
{
    m_icon = SmallIcon(i.itemIcon());
    initItem();
    m_fifoSorting = 1; //because this is top level item
}

KexiProjectModelItem::KexiProjectModelItem(KexiPart::Info &i, KexiPart::Item &item, KexiProjectModelItem *p) : m_info(&i), m_item(&item), m_parentItem(p)
{
    m_icon = SmallIcon(i.itemIcon());
    initItem();
}

KexiProjectModelItem::~KexiProjectModelItem()
{
}

void KexiProjectModelItem::initItem()
{
    m_fifoSorting = 0;
    int sortKey = 0;
    // set sorting key with FIFO order
    if (parent()) {
        sortKey = parent()->childCount();
    }
    
    m_sortKey.sprintf("%2.2d", sortKey);
// kDebug() << "m_sortKey=" << m_sortKey;
}

void KexiProjectModelItem::appendChild(KexiProjectModelItem* c)
{
    m_childItems.append(c);
}

void KexiProjectModelItem::debugPrint()
{
    if (m_item) {
        kDebug() << m_item->captionOrName();
    } else if (m_info) {
        kDebug() << m_info->groupName();
    } else   {
        kDebug() << m_groupName;
    }

    foreach(KexiProjectModelItem* itm, m_childItems) {
        itm->debugPrint();
    }
}

void KexiProjectModelItem::clearChildren()
{
    qDeleteAll(m_childItems);
}

QString KexiProjectModelItem::key(int column, bool ascending) const
{
// kDebug() << "KexiBrowserItem::key() : " << (m_fifoSorting ? m_sortKey : K3ListViewItem::key(column,ascending));
//    return m_fifoSorting ? m_sortKey : K3ListViewItem::key(column, ascending);
return QString();
    
}

KexiProjectModelItem* KexiProjectModelItem::parent()
{
    return m_parentItem;
}

KexiProjectModelItem* KexiProjectModelItem::child(int row)
{
    return m_childItems.value(row);
}

int KexiProjectModelItem::childCount() const
{
    return m_childItems.count();
}

int KexiProjectModelItem::columnCount() const
{
    return 1;
}

QVariant KexiProjectModelItem::data(int column) const
{
    if (m_item) {
        return m_item->name();
    } else if (m_info) {
        return m_info->groupName();
    } else   {
        return m_groupName;
    }
}

int KexiProjectModelItem::row()
{
     if (m_parentItem)
     {
         //kDebug() << m_parentItem->m_childItems << this << data(0);;
         return m_parentItem->m_childItems.indexOf(this);
     }
     kDebug() << "No parent item!";
     return 0;
}

QIcon KexiProjectModelItem::icon()
{
    return m_icon;
}

Qt::ItemFlags KexiProjectModelItem::flags()
{
    if (m_item) {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    } else {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

void KexiProjectModelItem::removeChild(const KexiPart::Item& item)
{
    KexiProjectModelItem *to_delete = 0;
    int i = 0;
    if (!m_item) {
        foreach(KexiProjectModelItem *child, m_childItems) {
            ++i;
            if (!to_delete) {
                if (child->m_item) {
                    if (child->m_item && child->m_item->identifier() == item.identifier()) {
                        to_delete = m_childItems.takeAt(i-1);
                    }
                }
            }
        }
    }
    if (to_delete) {
        delete to_delete;
    }
}

KexiProjectModelItem* KexiProjectModelItem::modelItemFromItem(const KexiPart::Item& item) const
{
    KexiProjectModelItem* itm = 0;
    
    if (!m_item) {
        foreach(KexiProjectModelItem *child, m_childItems) {
            if (!itm) {
                if (child->m_item) {
                    if (child->m_item && child->m_item->identifier() == item.identifier()) {
                        itm = child;
                    }
                } else {
                        itm = child->modelItemFromItem(item);
                }
            }
        }
    }
    return itm;
}

KexiProjectModelItem* KexiProjectModelItem::modelItemFromName(const QString& name) const
{
    KexiProjectModelItem* itm = 0;

        foreach(KexiProjectModelItem *child, m_childItems) {
            if (!itm) {
                if ((child->m_item && child->m_item->name() == name) || (child->m_info && child->m_info->partClass() == name) || (child->m_groupName == name)) {
                        itm = child;
                } else {
                     itm = child->modelItemFromName(name);
                }
            }
        }
    return itm;
}

