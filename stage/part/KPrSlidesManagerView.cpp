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
#include <qmath.h>
#include <QPaintEvent>
#include <QScrollBar>
#include <QPainter>
#include <QPen>
#include <QDrag>

//KF5 headers
#include <klocalizedstring.h>
#include <kiconloader.h>

KPrSlidesManagerView::KPrSlidesManagerView(QWidget *parent)
    : QListView(parent)
    , m_draggingFlag(false)
    , margin(23)
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(true);
    setResizeMode(QListView::Adjust);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSpacing(m_itemSize.width()/10);
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
    if (isDragging()) {
        QSize size(itemSize().width() + spacing(), itemSize().height() + spacing());
        QPair <int, int> m_pair = cursorRowAndColumn();
        int numberColumn = m_pair.first;
        int numberRow = m_pair.second;
        int scrollBarValue = verticalScrollBar()->value();

        QPoint point1(numberColumn * size.width() + spacing() / 2, numberRow * size.height() + spacing() - scrollBarValue);
        QPoint point2(numberColumn * size.width() + spacing() / 2, (numberRow + 1) * size.height() - scrollBarValue);
        QLineF line(point1, point2);

        QPainter painter(this->viewport());
        QPen pen = QPen(palette().brush(QPalette::Highlight), spacing() / 4);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        painter.setOpacity(0.8);
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
    // do not call QListView::mouseDoubleClickEvent(event); here as this triggers a rename command to be added
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
    setDraggingFlag(false);
    ev->setDropAction(Qt::IgnoreAction);
    ev->accept();

    clearSelection();

    if (!model()) {
        return;
    }

    int newIndex = cursorSlideIndex();

    if (newIndex >= model()->rowCount(QModelIndex())) {
        newIndex = -1;
    }

    model()->dropMimeData(ev->mimeData(), Qt::MoveAction, newIndex, -1, QModelIndex());
}

void KPrSlidesManagerView::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
    if (!model()) {
        return;
    }
    QListView::dragMoveEvent(ev);
    setDraggingFlag();
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
    setDraggingFlag(false);
}

void KPrSlidesManagerView::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    emit focusLost();
}

void KPrSlidesManagerView::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    emit focusGot();
}

void KPrSlidesManagerView::wheelEvent(QWheelEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
        if (event->delta() > 0) {
            emit zoomIn();
        }
        else {
            emit zoomOut();
        }
    }
    else {
        QListView::wheelEvent(event);
    }
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
    if (model()) {
        return (this->visualRect(model()->index(0,0,QModelIndex())));
    }
    else {
        return QRect();
    }
}

void KPrSlidesManagerView::setDraggingFlag(bool flag)
{
    m_draggingFlag = flag;
}

bool KPrSlidesManagerView::isDragging() const
{
    return m_draggingFlag;
}

bool KPrSlidesManagerView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport() && model()) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QModelIndex item = indexAt(QWidget::mapFromGlobal(QCursor::pos()));
            QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);

            //Left button is used to deselect, but right button needs a selected item for
            //context menu actions
            if ((item.row() < 0) && (mouseEv->button() != Qt::LeftButton)) {
                // Selects the last item of the row
                QModelIndex last_index = model()->index(qMin(cursorSlideIndex(), model()->rowCount(QModelIndex()) - 1),
                                                        0, QModelIndex());
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
     int size = (KIconLoader::SizeHuge + KIconLoader::SizeEnormous) / 2;
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
    int slidesNumber = qFloor((contentsRect().width() - (margin + spacing() - contentsMargins().right())) /
                              (itemSize().width() + spacing()));
    slidesNumber = qMax(slidesNumber, 1);
    return (m_pair.second * slidesNumber + qMin(slidesNumber, m_pair.first));
}

QPair<int, int> KPrSlidesManagerView::cursorRowAndColumn() const
{
    QSize size(itemSize().width() + spacing(), itemSize().height() + spacing());
    int slidesNumber = qFloor((contentsRect().width() - (margin + spacing() - contentsMargins().right())) / size.width());
    slidesNumber = qMax(slidesNumber, 1);
    int scrollBarValue = verticalScrollBar()->value();
    QPoint cursorPosition = QWidget::mapFromGlobal(QCursor::pos());
    int numberColumn = qFloor(cursorPosition.x() / size.width());
    int numberRow = qCeil((cursorPosition.y() + scrollBarValue) / (qreal)size.height()) - 1;
    int numberMod = model()->rowCount(QModelIndex()) > 0 ?
                (numberColumn + slidesNumber * numberRow) % (model()->rowCount(QModelIndex())) : 0;

     int totalRows = qCeil((model()->rowCount(QModelIndex())) / (qreal)slidesNumber);

    if (numberColumn > slidesNumber) {
        numberColumn = slidesNumber;
    }

    if ((numberColumn > numberMod) & (model()->rowCount(QModelIndex()) % slidesNumber != 0)) {
        numberColumn = model()->rowCount(QModelIndex()) % slidesNumber;
    }

    if (numberRow > totalRows - 1) {
        numberRow = totalRows - 1;
        numberColumn = model()->rowCount(QModelIndex()) % slidesNumber != 0 ?
                    model()->rowCount(QModelIndex()) % slidesNumber : slidesNumber;
    }

    return QPair<int,int>(numberColumn, numberRow);
}
