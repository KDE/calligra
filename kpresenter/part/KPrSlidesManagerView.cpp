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
#include <QScrollBar>
#include <QMenu>
#include <QtCore/qmath.h>
#include <QPaintEvent>
#include <QtGui>

//Kde headers
#include <klocale.h>
#include <KIconLoader>

//Calligra headers
#include <KoToolProxy.h>
#include "KPrViewModeSlidesSorter.h"

KPrSlidesManagerView::KPrSlidesManagerView(KoToolProxy *toolProxy, QWidget *parent)
    : QListView(parent)
    , m_dragingFlag(false)
    , m_toolProxy(toolProxy)
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(TRUE);
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
    int lastItem = lastItemNumber();
    int currentItemNumber = lastItem;

    //20 is for the rigth margin
    int slidesNumber = qFloor((contentsRect().width() - 20)/itemSize().width());

    if (isDraging() && currentItemNumber >= 0) {

        QSize size(itemSize().width(), itemSize().height());

        int numberMod = currentItemNumber % slidesNumber;

        //put line after last slide.
        if (numberMod == 0 && currentItemNumber == model()->rowCount(QModelIndex())) {
            numberMod = slidesNumber;
        }
        int verticalValue = (currentItemNumber - numberMod) / slidesNumber * size.height() - verticalScrollBar()->value();

        QPoint point1(numberMod * size.width(), verticalValue);
        QPoint point2(numberMod * size.width(), verticalValue + size.height());

        qDebug("NumberMod: %d", numberMod);
        qDebug("VerticalValue: %d", verticalValue);
        qDebug("SlidesNumber: %d", slidesNumber);
        qDebug("size: %d, %d", size.width(), size.height());
        qDebug ("point 1: %d, %d", point1.x(), point1.y());
        qDebug ("point 2: %d, %d", point2.x(), point2.y());
        qDebug("Mouse pointer: %d, %d", QWidget::mapFromGlobal(QCursor::pos()).x(), QWidget::mapFromGlobal(QCursor::pos()).y());

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

        QDrag* drag = new QDrag(this);
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

    int newIndex;
    QModelIndex itemNew = indexAt(ev->pos());
    newIndex = itemNew.row();
    model()->dropMimeData (ev->mimeData (), Qt::MoveAction, newIndex, -1, QModelIndex());
}

void KPrSlidesManagerView::dragMoveEvent(QDragMoveEvent *ev)
{
    ev->accept();
    setDragingFlag ();
    pageBefore(ev->pos());
    viewport()->update();
}

void KPrSlidesManagerView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-koffice-sliderssorter")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

int KPrSlidesManagerView::pageBefore(QPoint point)
{
    QModelIndex item = indexAt(point);
    int pageBeforeNumber = -1;
    if (item.row()>=0) {
        //normal case
        pageBeforeNumber = item.row();
    } else {
        //after the last slide
        pageBeforeNumber = model()->rowCount(QModelIndex());
    }
    setLastItemNumber(pageBeforeNumber);
    return pageBeforeNumber;
}

void KPrSlidesManagerView::setLastItemNumber(int number)
{
    m_lastItemNumber = number;
}

int KPrSlidesManagerView::lastItemNumber()
{
    return m_lastItemNumber;
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

bool KPrSlidesManagerView::isDraging()
{
    return m_dragingFlag;
}

bool KPrSlidesManagerView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport()) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QModelIndex item = indexAt(QWidget::mapFromGlobal(QCursor::pos()));
            QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);

            //Left button is used to deselect, but rigth button needs a selected item for
            //context menu actions
            if ((item.row() < 0) & (mouseEv->button() != Qt::LeftButton) ) {
                // Selects the last item
                QModelIndex last_index = model()->index(model()->rowCount(QModelIndex()) - 1, 0, QModelIndex());
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
     int size = KIconLoader::SizeEnormous;
     if (itemCount > 9) {
         xCount = 4;
         size = KIconLoader::SizeLarge;
     } else if (itemCount > 5) {
         xCount = 3;
         size = KIconLoader::SizeHuge;
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
     foreach (const QModelIndex& selectedIndex, selectedIndexes) {
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
