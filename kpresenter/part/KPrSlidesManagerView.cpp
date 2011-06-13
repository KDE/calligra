/* This file is part of the KDE project
*
* Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
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

#include "KPrSlidesManagerView.h"

//Qt headers
#include <QtCore/qmath.h>
#include <QPaintEvent>
#include <QScrollBar>

//Kde headers
#include <klocale.h>
#include <KIconLoader>

//Calligra headers
#include <KoToolProxy.h>

KPrSlidesManagerView::KPrSlidesManagerView(QWidget *parent)
    : QListView(parent)
    , m_dragingFlag(false)
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    viewport()->installEventFilter(this);
}

KPrSlidesManagerView::~KPrSlidesManagerView()
{
}

void KPrSlidesManagerView::paintEvent(QPaintEvent *event)
{
    event->accept();
    QListView::paintEvent(event);

    // Paint the line where the slide should go
    if (isDraging()) {

        QSize size(itemSize().width(), itemSize().height());

        QPair <int, int> m_pair = cursorRowAndColumn();
        int numberColumn = m_pair.first;
        int numberRow = m_pair.second;
        int scrollBarValue = verticalScrollBar()->value();

        QPoint point1(numberColumn * size.width(), numberRow * size.height() - scrollBarValue);
        QPoint point2(numberColumn * size.width(), (numberRow + 1) * size.height() - scrollBarValue);

        QLineF line(point1, point2);

        QPainter painter(this->viewport());
        painter.drawLine(line);
    }
}

void KPrSlidesManagerView::contextMenuEvent(QContextMenuEvent *event)
{
    emit requestContextMenu(event);
}

void KPrSlidesManagerView::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->accept();
    QListView::mouseDoubleClickEvent(event);
    emit slideDblClick();
}

void KPrSlidesManagerView::startDrag(Qt::DropActions supportedActions)
{
    const QModelIndexList indexes = selectionModel()->selectedIndexes();
    if (!indexes.isEmpty()) {
        QMimeData *data = model()->mimeData(indexes);
        if (!data) {
            return;
        }

        QDrag *drag = new QDrag(this);
        drag->setPixmap(createDragPixmap());
        drag->setMimeData(data);

        //m_dragSource = this;
        drag->exec(supportedActions, Qt::CopyAction);
    }
}

void KPrSlidesManagerView::dropEvent(QDropEvent *ev)
{
    setDragingFlag(false);
    ev->setDropAction(Qt::IgnoreAction);
    ev->accept();

    clearSelection();

    if (!model())
        return;

    int newIndex = cursorSlideIndex();

    if (newIndex >= model()->rowCount(QModelIndex())) {
        newIndex = -1;
    }

    model()->dropMimeData(ev->mimeData(), Qt::MoveAction, newIndex, -1, QModelIndex());
}

void KPrSlidesManagerView::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
    if (!model())
        return;

    setDragingFlag();
    viewport()->update();

}

void KPrSlidesManagerView::dragEnterEvent(QDragEnterEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
}

void KPrSlidesManagerView::dragLeaveEvent(QDragLeaveEvent *e)
{
    Q_UNUSED(e);
    setDragingFlag(false);
}

void KPrSlidesManagerView::focusOutEvent(QFocusEvent *event)
{
    emit focusLost();
}

void KPrSlidesManagerView::focusInEvent(QFocusEvent *event)
{
    emit focusGot();
}

void KPrSlidesManagerView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (!this->selectionModel()->selection().isEmpty()) {
        emit itemSelected();
    }
    else {
        emit selectionCleared();
    }
    QListView::selectionChanged(selected, deselected);
}

QRect KPrSlidesManagerView::itemSize() const
{
    return m_itemSize;
}

void KPrSlidesManagerView::setItemSize(QRect size)
{
    m_itemSize = size;
}

void KPrSlidesManagerView::setDragingFlag(bool flag)
{
    m_dragingFlag = flag;
}

bool KPrSlidesManagerView::isDraging() const
{
    return m_dragingFlag;
}

bool KPrSlidesManagerView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport() && model()) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QModelIndex item = indexAt(QWidget::mapFromGlobal(QCursor::pos()));
            QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);

            //Left button is used to deselect, but rigth button needs a selected item for
            //context menu actions
            if ((item.row() < 0) & (mouseEv->button() != Qt::LeftButton)) {
                // Selects the last item of the row
                QModelIndex last_index = model()->index(cursorSlideIndex() - 1, 0, QModelIndex());
                setCurrentIndex(last_index);
                emit indexChanged(last_index);
            }
            break;
        }

        default:
            break;
        }
    }

    return QObject::eventFilter(watched, event);
}

QPixmap KPrSlidesManagerView::createDragPixmap() const
{
     const QModelIndexList selectedIndexes = selectionModel()->selectedIndexes();
     Q_ASSERT(!selectedIndexes.isEmpty());

     const int itemCount = selectedIndexes.count();

     // If more than one item is dragged, align the items inside a
     // rectangular grid. The maximum grid size is limited to 4 x 4 items.
     int xCount = 2;
     int size = KIconLoader::SizeHuge;
     if (itemCount > 9) {
         xCount = 4;
         size = KIconLoader::SizeMedium;
     }
     else if (itemCount > 4) {
         xCount = 3;
         size = KIconLoader::SizeLarge;
     }

     if (itemCount < xCount) {
         xCount = itemCount;
     }

     int yCount = itemCount / xCount;
     if (itemCount % xCount != 0) {
         ++yCount;
     }

     if (yCount > xCount) {
         yCount = xCount;
     }

     // Draw the selected items into the grid cells
     QPixmap dragPixmap(xCount * size + xCount - 1, yCount * size + yCount - 1);
     dragPixmap.fill(Qt::transparent);

     QPainter painter(&dragPixmap);
     int x = 0;
     int y = 0;
     foreach (const QModelIndex &selectedIndex, selectedIndexes) {
         const QIcon icon = (model()->data(selectedIndex, Qt::DecorationRole)).value<QIcon>();
         painter.drawPixmap(x, y, icon.pixmap(size, size));

         x += size + 1;
         if (x >= dragPixmap.width()) {
             x = 0;
             y += size + 1;
         }
         if (y >= dragPixmap.height()) {
             break;
         }
     }

     return dragPixmap;
}

int KPrSlidesManagerView::cursorSlideIndex() const
{
    QPair <int, int> m_pair = cursorRowAndColumn();
    int slidesNumber = qFloor((contentsRect().width() - 20) / itemSize().width());
    return (m_pair.first + m_pair.second * slidesNumber);
}

QPair<int, int> KPrSlidesManagerView::cursorRowAndColumn() const
{
    //20 is for the margin.
    int slidesNumber = qFloor((contentsRect().width() - 20) / itemSize().width());
    int scrollBarValue = verticalScrollBar()->value();

    QSize size(itemSize().width(), itemSize().height());
    QPoint cursorPosition = QWidget::mapFromGlobal(QCursor::pos());

    int numberColumn = qFloor(cursorPosition.x() / size.width());
    int numberRow = qFloor((cursorPosition.y() + scrollBarValue) / size.height());
    int numberMod = (numberColumn + slidesNumber * numberRow) % (model()->rowCount(QModelIndex()) + 1);

     int totalRows = qCeil((model()->rowCount(QModelIndex())) / slidesNumber);

    if (numberColumn > slidesNumber) {
        numberColumn = slidesNumber;
    }

    if (numberColumn > numberMod) {
        numberColumn = numberColumn - (numberMod + 1);
    }

    if (numberRow > totalRows) {
        numberRow = totalRows;
        numberColumn = model()->rowCount(QModelIndex()) % slidesNumber;
    }

    return QPair<int,int>(numberColumn, numberRow);
}
