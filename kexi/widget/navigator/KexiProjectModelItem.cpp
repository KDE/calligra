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
#include <KIcon>
#include <QtAlgorithms>
#include <kdebug.h>

class KexiProjectModelItem::Private
{
public:
    Private(KexiPart::Info *info_, KexiPart::Item *item_, KexiProjectModelItem *p)
      : parentItem(p), dirty(false), info(info_), item(item_)
    {
    }

    ~Private()
    {
        qDeleteAll(childItems);
    }

    /* private: */
    QList<KexiProjectModelItem*> childItems;
    KexiProjectModelItem *parentItem;
    QIcon icon;
    bool dirty;
    /* protected: */
    KexiPart::Info *info;
    KexiPart::Item *item;
    QString groupName;
};

KexiProjectModelItem::KexiProjectModelItem(const QString& n, KexiProjectModelItem *p)
    : d(new Private(0, 0, p))
{
    d->groupName = n;
}

KexiProjectModelItem::KexiProjectModelItem(KexiPart::Info &i, KexiProjectModelItem *p)
    : d(new Private(&i, 0, p))
{
    d->icon = KIcon(i.itemIconName());
}

KexiProjectModelItem::KexiProjectModelItem(KexiPart::Info &i, KexiPart::Item &item, KexiProjectModelItem *p)
    : d(new Private(&i, &item, p))
{
    d->icon = KIcon(i.itemIconName());
}

KexiProjectModelItem::~KexiProjectModelItem()
{
    delete d;
}

void KexiProjectModelItem::appendChild(KexiProjectModelItem* c)
{
    d->childItems.append(c);
}

void KexiProjectModelItem::debugPrint() const
{
    if (d->item) {
        kDebug() << d->item->captionOrName();
    } else if (d->info) {
        kDebug() << d->info->groupName();
    } else   {
        kDebug() << d->groupName;
    }

    foreach(KexiProjectModelItem* itm, d->childItems) {
        itm->debugPrint();
    }
}

void KexiProjectModelItem::clearChildren()
{
    qDeleteAll(d->childItems);
}

KexiPart::Info *KexiProjectModelItem::partInfo() const
{
    return d->info;
}

KexiPart::Item* KexiProjectModelItem::partItem() const
{
    return d->item;
}

KexiProjectModelItem* KexiProjectModelItem::parent()
{
    return d->parentItem;
}

KexiProjectModelItem* KexiProjectModelItem::child(int row)
{
    return d->childItems.value(row);
}

int KexiProjectModelItem::childCount() const
{
    return d->childItems.count();
}

int KexiProjectModelItem::columnCount() const
{
    return 1;
}

QVariant KexiProjectModelItem::data(int column) const
{
    Q_UNUSED(column);
    if (d->item) {
#ifdef KEXI_MOBILE
        return d->item->captionOrName();
#else
        return d->item->name() + (d->dirty ? "*" : "");
#endif
    } else if (d->info) {
        return d->info->groupName();
    } else   {
        return d->groupName;
    }
}

int KexiProjectModelItem::row()
{
     if (d->parentItem)
     {
         //kDebug() << d->parentItem->d->childItems << this << data(0);
         return d->parentItem->d->childItems.indexOf(this);
     }
     kDebug() << "No parent item!";
     return 0;
}

QIcon KexiProjectModelItem::icon() const
{
    return d->icon;
}

Qt::ItemFlags KexiProjectModelItem::flags() const
{
    if (d->item) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    } else {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

void KexiProjectModelItem::removeChild(const KexiPart::Item& item)
{
    KexiProjectModelItem *to_delete = 0;
    int i = 0;
    if (!d->item) {
        foreach(KexiProjectModelItem *child, d->childItems) {
            ++i;
            if (!to_delete) {
                if (child->d->item) {
                    if (child->d->item && child->d->item->identifier() == item.identifier()) {
                        to_delete = d->childItems.takeAt(i-1);
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

    if (!d->item) {
        foreach(KexiProjectModelItem *child, d->childItems) {
            if (child->d->item) {
               if (child->d->item && child->d->item->identifier() == item.identifier()) {
                    itm = child;
                }
            } else {
                    itm = child->modelItemFromItem(item);
            }
            if (itm) {
                return itm;
            }
        }
    }
    return 0;
}

KexiProjectModelItem* KexiProjectModelItem::modelItemFromName(const QString& name) const
{
    KexiProjectModelItem* itm = 0;

    foreach(KexiProjectModelItem *child, d->childItems) {
        if ((child->d->item && child->d->item->name() == name) || (child->d->info && child->d->info->partClass() == name) || (child->d->groupName == name)) {
                itm = child;
        } else {
                itm = child->modelItemFromName(name);
        }
        if (itm) {
            return itm;
        }
    }
    return 0;
}

void KexiProjectModelItem::sortChildren()
{
    qSort(d->childItems.begin(), d->childItems.end(), itemLessThan);
}

bool itemLessThan(const KexiProjectModelItem *a, const KexiProjectModelItem *b)
{
    return a->data(0).toString() < b->data(0).toString();
}

void KexiProjectModelItem::setDirty(bool set)
{
    d->dirty = set;
}
