/**
  * This file is part of the KDE project
  * Copyright (C) 2008 Rafael Fernández López <ereslibre@kde.org>
  * Copyright (C) 2008 Kevin Ottens <ervin@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#ifndef KWIDGETITEMDELEGATEPOOL_P_H
#define KWIDGETITEMDELEGATEPOOL_P_H

#include <QtCore/QModelIndex>
#include <QtCore/QHash>
#include <QtCore/QList>

class QWidget;
class QStyleOptionViewItem;
class KWidgetItemDelegate;
class KWidgetItemDelegatePoolPrivate;


/**
  * @internal
  */
class KWidgetItemDelegatePool
{
public:
    enum UpdateWidgetsEnum {
        UpdateWidgets = 0,
        NotUpdateWidgets
    };

    /**
      * Creates a new ItemDelegatePool.
      *
      * @param delegate the ItemDelegate for this pool.
      */

    KWidgetItemDelegatePool(KWidgetItemDelegate *delegate);

    /**
      * Destroys an ItemDelegatePool.
      */
    ~KWidgetItemDelegatePool();

    /**
      * @brief Returns the widget associated to @p index and @p widget
      * @param index The index to search into.
      * @param option a QStyleOptionViewItem.
      * @return A QList of the pointers to the widgets found.
      * @internal
      */
    QList<QWidget*> findWidgets(const QPersistentModelIndex &index, const QStyleOptionViewItem &option,
                                UpdateWidgetsEnum updateWidgets = UpdateWidgets) const;

    /**
      * @internal
      */
    QList<QWidget*> invalidIndexesWidgets() const;

    /**
      * @internal
      */
    void fullClear();

private:
    friend class KWidgetItemDelegate;
    friend class KWidgetItemDelegatePrivate;
    KWidgetItemDelegatePoolPrivate *const d;
};

class KWidgetItemDelegateEventListener;

/**
  * @internal
  */
class KWidgetItemDelegatePoolPrivate
{
public:

    KWidgetItemDelegatePoolPrivate(KWidgetItemDelegate *d);

    KWidgetItemDelegate *delegate;
    KWidgetItemDelegateEventListener *eventListener;

    QList<QList<QWidget*> > allocatedWidgets;
    QHash<QPersistentModelIndex, QList<QWidget*> > usedWidgets;
    QHash<QWidget*, QPersistentModelIndex> widgetInIndex;

    bool clearing;
};

#endif
