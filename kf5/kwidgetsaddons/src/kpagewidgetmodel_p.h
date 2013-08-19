/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef KPAGEWIDGETMODEL_P_H
#define KPAGEWIDGETMODEL_P_H

#include "kpagemodel_p.h"
#include "kpagewidgetmodel.h"

class PageItem
{
    public:
        explicit PageItem(KPageWidgetItem *pageItem, PageItem *parent = 0);
        ~PageItem();

        void appendChild(PageItem *child);
        void insertChild(int row, PageItem *child);
        void removeChild(int row);

        PageItem *child(int row);
        int childCount() const;
        int columnCount() const;
        int row() const;
        PageItem *parent();

        KPageWidgetItem *pageWidgetItem() const;

        PageItem *findChild(const KPageWidgetItem *item);

        void dump(int indent = 0);

    private:
        KPageWidgetItem *mPageWidgetItem;

        QList<PageItem*> mChildItems;
        PageItem *mParentItem;
};

class KPageWidgetModelPrivate : public KPageModelPrivate
{
    Q_DECLARE_PUBLIC(KPageWidgetModel)
    protected:
        KPageWidgetModelPrivate()
            : rootItem(new PageItem(0, 0))
        {
        }

        ~KPageWidgetModelPrivate()
        {
            delete rootItem;
            rootItem = 0;
        }

        PageItem *rootItem;

        void _k_itemChanged()
        {
            Q_Q(KPageWidgetModel);
            KPageWidgetItem *item = qobject_cast<KPageWidgetItem*>(q->sender());
            if (!item) {
                return;
            }

            const QModelIndex index = q->index(item);
            if (!index.isValid()) {
                return;
            }

            emit q->dataChanged(index, index);
        }

        void _k_itemToggled(bool checked)
        {
            Q_Q(KPageWidgetModel);
            KPageWidgetItem *item = qobject_cast<KPageWidgetItem*>(q->sender());
            if (!item) {
                return;
            }

            emit q->toggled(item, checked);
        }
};

#endif // KPAGEWIDGETMODEL_P_H
// vim: sw=4 sts=4 et tw=100
