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

#include "kwidgetitemdelegatepool_p.h"

#include <QtCore/qobjectdefs.h>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaMethod>
#include <QtCore/QMetaProperty>
#include <QtCore/QPair>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QWidget>
#include <QAbstractItemView>
#include <QApplication>
#include <QInputEvent>
#include <QAbstractProxyModel>
#include <QtDebug>

#include "kwidgetitemdelegate.h"
#include "kwidgetitemdelegate_p.h"

#define POOL_USAGE 0

Q_DECLARE_METATYPE(QModelIndex)

/**
  Private class that helps to provide binary compatibility between releases.
  @internal
*/
//@cond PRIVATE
class KWidgetItemDelegateEventListener
    : public QObject
{
public:
    KWidgetItemDelegateEventListener(KWidgetItemDelegatePoolPrivate *poolPrivate, QObject *parent = 0)
        : QObject(parent)
        , poolPrivate(poolPrivate)
    {
    }

    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    KWidgetItemDelegatePoolPrivate *poolPrivate;
};

KWidgetItemDelegatePoolPrivate::KWidgetItemDelegatePoolPrivate(KWidgetItemDelegate *d)
    : delegate(d)
    , eventListener(new KWidgetItemDelegateEventListener(this))
    , clearing(false)
{
}

KWidgetItemDelegatePool::KWidgetItemDelegatePool(KWidgetItemDelegate *delegate)
    : d(new KWidgetItemDelegatePoolPrivate(delegate))
{
}

KWidgetItemDelegatePool::~KWidgetItemDelegatePool()
{
    delete d->eventListener;
    delete d;
}

QList<QWidget*> KWidgetItemDelegatePool::findWidgets(const QPersistentModelIndex &idx,
                                                     const QStyleOptionViewItem &option,
                                                     UpdateWidgetsEnum updateWidgets) const
{
    QList<QWidget*> result;

    if (!idx.isValid()) {
        return result;
    }

    QModelIndex index;
    if (const QAbstractProxyModel *proxyModel = qobject_cast<const QAbstractProxyModel*>(idx.model())) {
        index = proxyModel->mapToSource(idx);
    } else {
        index = idx;
    }

    if (!index.isValid()) {
        return result;
    }

    if (d->usedWidgets.contains(index)) {
        result = d->usedWidgets[index];
    } else {
        // ### KDE5 This sets a property on the delegate because we can't add an argument to createItemWidgets
        d->delegate->setProperty("goya:creatingWidgetForIndex", QVariant::fromValue(index));
        result = d->delegate->createItemWidgets();
        d->delegate->setProperty("goya:creatingWidgetForIndex", QVariant());
        d->allocatedWidgets << result;
        d->usedWidgets[index] = result;
        foreach (QWidget *widget, result) {
            d->widgetInIndex[widget] = index;
            widget->setParent(d->delegate->d->itemView->viewport());
            widget->installEventFilter(d->eventListener);
            widget->setVisible(true);
        }
    }

    if (updateWidgets == UpdateWidgets) {
        foreach (QWidget *widget, result) {
            widget->setVisible(true);
        }

        d->delegate->updateItemWidgets(result, option, idx);

        foreach (QWidget *widget, result) {
            widget->move(widget->x() + option.rect.left(), widget->y() + option.rect.top());
        }
    }

    return result;
}

QList<QWidget*> KWidgetItemDelegatePool::invalidIndexesWidgets() const
{
    QList<QWidget*> result;
    foreach (QWidget *widget, d->widgetInIndex.keys()) {
        const QAbstractProxyModel *proxyModel = qobject_cast<const QAbstractProxyModel*>(d->delegate->d->model);
        QModelIndex index;
        if (proxyModel) {
            index = proxyModel->mapFromSource(d->widgetInIndex[widget]);
        } else {
            index = d->widgetInIndex[widget];
        }
        if (!index.isValid()) {
            result << widget;
        }
    }
    return result;
}

void KWidgetItemDelegatePool::fullClear()
{
    d->clearing = true;
    qDeleteAll(d->widgetInIndex.keys());
    d->clearing = false;
    d->allocatedWidgets.clear();
    d->usedWidgets.clear();
    d->widgetInIndex.clear();
}

bool KWidgetItemDelegateEventListener::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *widget = static_cast<QWidget*>(watched);

    if (event->type() == QEvent::Destroy && !poolPrivate->clearing) {
        qWarning() << "User of KWidgetItemDelegate should not delete widgets created by createItemWidgets!";
        // assume the application has kept a list of widgets and tries to delete them manually
        // they have been reparented to the view in any case, so no leaking occurs
        poolPrivate->widgetInIndex.remove(widget);
        QWidget *viewport = poolPrivate->delegate->d->itemView->viewport();
        QApplication::sendEvent(viewport, event);
    }
    if (dynamic_cast<QInputEvent*>(event) && !poolPrivate->delegate->blockedEventTypes(widget).contains(event->type())) {
        QWidget *viewport = poolPrivate->delegate->d->itemView->viewport();
        switch(event->type()) {
            case QEvent::MouseMove:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick: {
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                    QMouseEvent evt(event->type(), viewport->mapFromGlobal(mouseEvent->globalPos()),
                                    mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                    QApplication::sendEvent(viewport, &evt);
                }
                break;
            case QEvent::Wheel: {
                    QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
                    QWheelEvent evt(viewport->mapFromGlobal(wheelEvent->globalPos()),
                                    wheelEvent->delta(), wheelEvent->buttons(), wheelEvent->modifiers(),
                                    wheelEvent->orientation());
                    QApplication::sendEvent(viewport, &evt);
                }
                break;
            case QEvent::TabletMove:
            case QEvent::TabletPress:
            case QEvent::TabletRelease:
            case QEvent::TabletEnterProximity:
            case QEvent::TabletLeaveProximity: {
                    QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(event);
                    QTabletEvent evt(event->type(), QPointF(viewport->mapFromGlobal(tabletEvent->globalPos())),
                                     tabletEvent->globalPosF(), tabletEvent->device(),
                                     tabletEvent->pointerType(), tabletEvent->pressure(), tabletEvent->xTilt(),
                                     tabletEvent->yTilt(), tabletEvent->tangentialPressure(), tabletEvent->rotation(),
                                     tabletEvent->z(), tabletEvent->modifiers(), tabletEvent->uniqueId());
                    QApplication::sendEvent(viewport, &evt);
                }
                break;
            default:
                QApplication::sendEvent(viewport, event);
                break;
        }
    }

    return QObject::eventFilter(watched, event);
}
//@endcond
