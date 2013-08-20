/* This file is part of the KDE libraries
    Copyright (C) 2006,2007 Andreas Hartmetz (ahartmetz@gmail.com)
    Copyright (C) 2008 Urs Wolfer (uwolfer @ kde.org)

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
    Boston, MA 02110-1301, USA.
*/

#include "kextendableitemdelegate.h"

#include <QModelIndex>
#include <QScrollBar>
#include <QTreeView>
#include <QPainter>
#include <QApplication>


class KExtendableItemDelegate::Private {
public:
    Private(KExtendableItemDelegate *parent) :
        q(parent),
        stateTick(0),
        cachedStateTick(-1),
        cachedRow(-20), //Qt uses -1 for invalid indices
        extender(0),
        extenderHeight(0)

    {}

    void _k_extenderDestructionHandler(QObject *destroyed);
    void _k_verticalScroll();

    QSize maybeExtendedSize(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QModelIndex indexOfExtendedColumnInSameRow(const QModelIndex &index) const;
    void scheduleUpdateViewLayout();

    KExtendableItemDelegate *q;

    /**
     * Delete all active extenders
     */
    void deleteExtenders();

    //this will trigger a lot of auto-casting QModelIndex <-> QPersistentModelIndex
    QHash<QPersistentModelIndex, QWidget *> extenders;
    QHash<QWidget *, QPersistentModelIndex> extenderIndices;
    QHash<QWidget *, QPersistentModelIndex> deletionQueue;
    QPixmap extendPixmap;
    QPixmap contractPixmap;
    int stateTick;
    int cachedStateTick;
    int cachedRow;
    QModelIndex cachedParentIndex;
    QWidget *extender;
    int extenderHeight;
};


KExtendableItemDelegate::KExtendableItemDelegate(QAbstractItemView* parent)
 : QStyledItemDelegate(parent),
   d(new Private(this))
{
    connect(parent->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(_k_verticalScroll()));
}


KExtendableItemDelegate::~KExtendableItemDelegate()
{
    delete d;
}


void KExtendableItemDelegate::extendItem(QWidget *ext, const QModelIndex &index)
{
    // qDebug() << "Creating extender at " << ext << " for item " << index.model()->data(index,Qt::DisplayRole).toString();

    if (!ext || !index.isValid()) {
        return;
    }
    //maintain the invariant "zero or one extender per row"
    d->stateTick++;
    contractItem(d->indexOfExtendedColumnInSameRow(index));
    d->stateTick++;
    //reparent, as promised in the docs
    QAbstractItemView *aiv = qobject_cast<QAbstractItemView *>(parent());
    if (!aiv) {
        return;
    }
    ext->setParent(aiv->viewport());
    d->extenders.insert(index, ext);
    d->extenderIndices.insert(ext, index);
    connect(ext, SIGNAL(destroyed(QObject*)), this, SLOT(_k_extenderDestructionHandler(QObject*)));
    emit extenderCreated(ext, index);
    d->scheduleUpdateViewLayout();
}


void KExtendableItemDelegate::contractItem(const QModelIndex& index)
{
    QWidget *extender = d->extenders.value(index);
    if (!extender) {
        return;
    }
    // qDebug() << "Collapse extender at " << extender << " for item " << index.model()->data(index,Qt::DisplayRole).toString();
    extender->hide();
    extender->deleteLater();

    QPersistentModelIndex persistentIndex = d->extenderIndices.take(extender);
    d->extenders.remove(persistentIndex);

    d->deletionQueue.insert(extender, persistentIndex);

    d->scheduleUpdateViewLayout();
}


void KExtendableItemDelegate::contractAll()
{
    d->deleteExtenders();
}


//slot
void KExtendableItemDelegate::Private::_k_extenderDestructionHandler(QObject *destroyed)
{
    // qDebug() << "Removing extender at " << destroyed;

    QWidget *extender = static_cast<QWidget *>(destroyed);
    stateTick++;

    QPersistentModelIndex persistentIndex = deletionQueue.take(extender);
    if (persistentIndex.isValid() &&
        q->receivers(SIGNAL(extenderDestroyed(QWidget*,QModelIndex)))) {

        QModelIndex index = persistentIndex;
        emit q->extenderDestroyed(extender, index);
    }

    scheduleUpdateViewLayout();
}


//slot
void KExtendableItemDelegate::Private::_k_verticalScroll()
{
    foreach (QWidget *extender, extenders) {
        // Fast scrolling can lead to artifacts where extenders stay in the viewport
        // of the parent's scroll area even though their items are scrolled out.
        // Therefore we hide all extenders when scrolling.
        // In paintEvent() show() will be called on actually visible extenders and
        // Qt's double buffering takes care of eliminating flicker.
        // ### This scales badly to many extenders. There are probably better ways to
        //     avoid the artifacts.
        extender->hide();
    }
}


bool KExtendableItemDelegate::isExtended(const QModelIndex &index) const
{
    return d->extenders.value(index);
}


QSize KExtendableItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize ret;

    if (!d->extenders.isEmpty()) {
        ret = d->maybeExtendedSize(option, index);
    } else {
        ret = QStyledItemDelegate::sizeHint(option, index);
    }

    bool showExtensionIndicator = index.model() ?
        index.model()->data(index, ShowExtensionIndicatorRole).toBool() : false;
    if (showExtensionIndicator) {
        ret.rwidth() += d->extendPixmap.width();
    }

    return ret;
}


void KExtendableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int indicatorX = 0;
    int indicatorY = 0;

    QStyleOptionViewItemV4 indicatorOption(option);
    initStyleOption(&indicatorOption, index);
    if (index.column() == 0) {
        indicatorOption.viewItemPosition = QStyleOptionViewItemV4::Beginning;
    } else if (index.column() == index.model()->columnCount() - 1) {
        indicatorOption.viewItemPosition = QStyleOptionViewItemV4::End;
    } else {
        indicatorOption.viewItemPosition = QStyleOptionViewItemV4::Middle;
    }

    QStyleOptionViewItemV4 itemOption(option);
    initStyleOption(&itemOption, index);
    if (index.column() == 0) {
        itemOption.viewItemPosition = QStyleOptionViewItemV4::Beginning;
    } else if (index.column() == index.model()->columnCount() - 1) {
        itemOption.viewItemPosition = QStyleOptionViewItemV4::End;
    } else {
        itemOption.viewItemPosition = QStyleOptionViewItemV4::Middle;
    }

    const bool showExtensionIndicator = index.model()->data(index, ShowExtensionIndicatorRole).toBool();

    if (showExtensionIndicator) {
        if (QApplication::isRightToLeft()) {
            indicatorX = option.rect.right() - d->extendPixmap.width();
            itemOption.rect.setRight(option.rect.right() - d->extendPixmap.width());
            indicatorOption.rect.setLeft(option.rect.right() - d->extendPixmap.width());
        } else {
            indicatorX = option.rect.left();
            indicatorOption.rect.setRight(option.rect.left() + d->extendPixmap.width());
            itemOption.rect.setLeft(option.rect.left() + d->extendPixmap.width());
        }
        indicatorY = option.rect.top() + ((option.rect.height() - d->extendPixmap.height()) >> 1);
    }

    //fast path
    if (d->extenders.isEmpty()) {
        QStyledItemDelegate::paint(painter, itemOption, index);
        if (showExtensionIndicator) {
            painter->save();
            QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &indicatorOption,
                                                 painter);
            painter->restore();
            painter->drawPixmap(indicatorX, indicatorY, d->extendPixmap);
        }
        return;
    }

    int row = index.row();
    QModelIndex parentIndex = index.parent();

    //indexOfExtendedColumnInSameRow() is very expensive, try to avoid calling it.
    if (row != d->cachedRow || d->cachedStateTick != d->stateTick
        || d->cachedParentIndex != parentIndex) {
        d->extender = d->extenders.value(d->indexOfExtendedColumnInSameRow(index));
        d->cachedStateTick = d->stateTick;
        d->cachedRow = row;
        d->cachedParentIndex = parentIndex;
        if (d->extender) {
            d->extenderHeight = d->extender->sizeHint().height();
        }
    }

    if (!d->extender) {
        QStyledItemDelegate::paint(painter, itemOption, index);
        if (showExtensionIndicator) {
            painter->save();
            QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &indicatorOption,
                                                 painter);
            painter->restore();
            painter->drawPixmap(indicatorX, indicatorY, d->extendPixmap);
        }
        return;
    }

    //an extender is present - make two rectangles: one to paint the original item, one for the extender
    if (isExtended(index)) {
        QStyleOptionViewItemV4 extOption(option);
        initStyleOption(&extOption, index);
        extOption.rect = extenderRect(d->extender, option, index);
        updateExtenderGeometry(d->extender, extOption, index);
        //if we show it before, it will briefly flash in the wrong location.
        //the downside is, of course, that an api user effectively can't hide it.
        d->extender->show();
    }

    indicatorOption.rect.setHeight(option.rect.height() - d->extenderHeight);
    itemOption.rect.setHeight(option.rect.height() - d->extenderHeight);
    //tricky:make sure that the modified options' rect really has the
    //same height as the unchanged option.rect if no extender is present
    //(seems to work OK)
    QStyledItemDelegate::paint(painter, itemOption, index);

    if (showExtensionIndicator) {
        //indicatorOption's height changed, change this too
        indicatorY = indicatorOption.rect.top() + ((indicatorOption.rect.height() -
                                                   d->extendPixmap.height()) >> 1);
        painter->save();
        QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &indicatorOption,
                                             painter);
        painter->restore();

        if (d->extenders.contains(index)) {
            painter->drawPixmap(indicatorX, indicatorY, d->contractPixmap);
        } else {
            painter->drawPixmap(indicatorX, indicatorY, d->extendPixmap);
        }
    }
}


QRect KExtendableItemDelegate::extenderRect(QWidget *extender, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(extender);
    QRect rect(option.rect);
    rect.setTop(rect.bottom() + 1 - extender->sizeHint().height());

    int indentation = 0;
    if (QTreeView *tv = qobject_cast<QTreeView *>(parent())) {
        int indentSteps = 0;
        for (QModelIndex idx(index.parent()); idx.isValid(); idx = idx.parent()) {
            indentSteps++;
        }
        if (tv->rootIsDecorated()) {
            indentSteps++;
        }
        indentation = indentSteps * tv->indentation();
    }

    QAbstractScrollArea *container = qobject_cast<QAbstractScrollArea *>(parent());
    Q_ASSERT(container);
    if (qApp->isLeftToRight()) {
        rect.setLeft(indentation);
        rect.setRight(container->viewport()->width() - 1);
    } else {
        rect.setRight(container->viewport()->width() - 1 - indentation);
        rect.setLeft(0);
    }
    return rect;
}


QSize KExtendableItemDelegate::Private::maybeExtendedSize(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *extender = extenders.value(index);
    QSize size(q->QStyledItemDelegate::sizeHint(option, index));
    if (!extender) {
        return size;
    }
    //add extender height to maximum height of any column in our row
    int itemHeight = size.height();

    int row = index.row();
    int thisColumn = index.column();

    //this is quite slow, but Qt is smart about when to call sizeHint().
    for (int column = 0; index.model()->columnCount() < column; column++) {
        if (column == thisColumn) {
            continue;
        }
        QModelIndex neighborIndex(index.sibling(row, column));
        if (!neighborIndex.isValid()) {
            break;
        }
        itemHeight = qMax(itemHeight, q->QStyledItemDelegate::sizeHint(option, neighborIndex).height());
    }

    //we only want to reserve vertical space, the horizontal extender layout is our private business.
    size.rheight() = itemHeight + extender->sizeHint().height();
    return size;
}


QModelIndex KExtendableItemDelegate::Private::indexOfExtendedColumnInSameRow(const QModelIndex &index) const
{
    const QAbstractItemModel *const model = index.model();
    const QModelIndex parentIndex(index.parent());
    const int row = index.row();
    const int columnCount = model->columnCount();

    //slow, slow, slow
    for (int column = 0; column < columnCount; column++) {
        QModelIndex indexOfExt(model->index(row, column, parentIndex));
        if (extenders.value(indexOfExt)) {
            return indexOfExt;
        }
    }

    return QModelIndex();
}


void KExtendableItemDelegate::updateExtenderGeometry(QWidget *extender, const QStyleOptionViewItem &option,
                                                     const QModelIndex &index) const
{
    Q_UNUSED(index);
    extender->setGeometry(option.rect);
}


void KExtendableItemDelegate::Private::deleteExtenders()
{
    foreach (QWidget *ext, extenders) {
        ext->hide();
        ext->deleteLater();
    }
    deletionQueue.unite(extenderIndices);
    extenders.clear();
    extenderIndices.clear();
}


//make the view re-ask for sizeHint() and redisplay items with their new size
//### starting from Qt 4.4 we could emit sizeHintChanged() instead
void KExtendableItemDelegate::Private::scheduleUpdateViewLayout()
{
    QAbstractItemView *aiv = qobject_cast<QAbstractItemView *>(q->parent());
    //prevent crashes during destruction of the view
    if (aiv) {
        //dirty hack to call aiv's protected scheduleDelayedItemsLayout()
        aiv->setRootIndex(aiv->rootIndex());
    }
}


void KExtendableItemDelegate::setExtendPixmap(const QPixmap &pixmap)
{
    d->extendPixmap = pixmap;
}


void KExtendableItemDelegate::setContractPixmap(const QPixmap &pixmap)
{
    d->contractPixmap = pixmap;
}


QPixmap KExtendableItemDelegate::extendPixmap()
{
    return d->extendPixmap;
}


QPixmap KExtendableItemDelegate::contractPixmap()
{
    return d->contractPixmap;
}

#include "moc_kextendableitemdelegate.cpp"
