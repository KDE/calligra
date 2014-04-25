/* This file is part of the KDE project
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiFormScrollAreaWidget.h"
#include "kexiformscrollview.h"

#include <formeditor/form.h>

#include <QPainter>
#include <QScrollBar>

const int RECT_MARGIN = 300;

KexiFormScrollAreaWidget::KexiFormScrollAreaWidget(KexiFormScrollView *area)
    : QWidget(), scrollArea(area), m_resizing(false)
{
    setMouseTracking(true);
}

KexiFormScrollAreaWidget::~KexiFormScrollAreaWidget()
{
}

void KexiFormScrollAreaWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    if (scrollArea->isPreviewing() || !scrollArea->isOuterAreaVisible()) {
        return;
    }

    //draw right and bottom borders
    QPainter p(this);
    p.setPen(QPen(palette().brush(QPalette::Active, QPalette::Foreground), 1));
    p.drawLine(width() - RECT_MARGIN, 0,
               width() - RECT_MARGIN, height() - RECT_MARGIN);
    p.drawLine(0, height() - RECT_MARGIN,
               width() - RECT_MARGIN, height() - RECT_MARGIN);
}

void KexiFormScrollAreaWidget::mouseMoveEvent(QMouseEvent *e)
{
    //kDebug() << e->pos() << scrollArea->widget()->size();
    if (!scrollArea->isResizingEnabled()) {
        return;
    }

    QWidget *mainAreaWidget = scrollArea->mainAreaWidget();
    if (m_resizing) { // resize widget
        int tmpx = e->x();
        int tmpy = e->y();
        const int exceeds_x = tmpx + 5 - scrollArea->widget()->width();
        const int exceeds_y = tmpy + 5 - scrollArea->widget()->height();
        if (exceeds_x > 0) {
            tmpx -= exceeds_x;
        }
        if (exceeds_y > 0) {
            tmpy -= exceeds_y;
        }
        if ((tmpx - scrollArea->horizontalScrollBar()->value()) < 0) {
            tmpx = scrollArea->horizontalScrollBar()->value();
        }
        if ((tmpy - scrollArea->verticalScrollBar()->value()) < 0) {
            tmpy = scrollArea->verticalScrollBar()->value();
        }
        // we look for the max widget right() (or bottom()),
        // which would be the limit for form resizing (not to hide widgets)
        const QList<QWidget*> list(mainAreaWidget->findChildren<QWidget*>()); /* not recursive*/
        foreach(QWidget *w, list) {
            tmpx = qMax(tmpx, (w->geometry().right() + 10));
            tmpy = qMax(tmpy, (w->geometry().bottom() + 10));
        }

        int neww = -1, newh;
        const int gridSize = (scrollArea->form()->isSnapToGridEnabled()
                              && scrollArea->form()->gridSize() > 1)
                             ? scrollArea->form()->gridSize() : 0;
        if (cursor().shape() == Qt::SizeHorCursor) {
            if (gridSize > 0) {
                neww = int(float(tmpx) / float(gridSize) + 0.5) * gridSize;
            }
            else {
                neww = tmpx;
            }
            newh = mainAreaWidget->height();
        } else if (cursor().shape() == Qt::SizeVerCursor) {
            neww = mainAreaWidget->width();
            if (gridSize > 0) {
                newh = int(float(tmpy) / float(gridSize) + 0.5) * gridSize;
            }
            else {
                newh = tmpy;
            }
        } else if (cursor().shape() == Qt::SizeFDiagCursor) {
            if (gridSize > 0) {
                neww = int(float(tmpx) / float(gridSize) + 0.5) * gridSize;
                newh = int(float(tmpy) / float(gridSize) + 0.5) * gridSize;
            } else {
                neww = tmpx;
                newh = tmpy;
            }
        }
        // needs update?
        if (neww != -1 && mainAreaWidget->size() != QSize(neww, newh)) {
            kDebug() << "mainAreaWidget->size():" << mainAreaWidget->size()
                     << "neww, newh:" << neww << newh;
            mainAreaWidget->resize(neww, newh);
            scrollArea->refreshContentsSize();
            emit resized();
        }
    }
    else { // update mouse cursor
        QRect rightRect(mainAreaWidget->width(),  0, 4, mainAreaWidget->height());
        QRect bottomRect(0, mainAreaWidget->height(), mainAreaWidget->width(), 4);
        QRect bottomRightRect(mainAreaWidget->width(), mainAreaWidget->height(), 4, 4);
        if (rightRect.contains(e->pos())) {
            setCursor(Qt::SizeHorCursor);
        }
        else if (bottomRect.contains(e->pos())) {
            setCursor(Qt::SizeVerCursor);
        }
        else if (bottomRightRect.contains(e->pos())) {
            setCursor(Qt::SizeFDiagCursor);
        }
        else {
            unsetCursor();
        }
    }
}

void KexiFormScrollAreaWidget::mousePressEvent(QMouseEvent *e)
{
    QWidget *mainAreaWidget = scrollArea->mainAreaWidget();
    QRect mainAreaRectPlusDelta(0, 0, mainAreaWidget->width() + 4, mainAreaWidget->height() + 4);
    if (!mainAreaRectPlusDelta.contains(e->pos())) { // clicked outside main area rect
        // unused for now: emit outerAreaClicked();
    }

    if (!scrollArea->isResizingEnabled()) {
        return;
    }

    QRect rightRect(mainAreaWidget->width(),  0, 4, mainAreaWidget->height() + 4); // right limit
    QRect bottomRect(0, mainAreaWidget->height(), mainAreaWidget->width() + 4, 4); // bottom limit
    if (rightRect.contains(e->pos()) || bottomRect.contains(e->pos())) {
        m_resizing = true;
    }
}

void KexiFormScrollAreaWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    if (m_resizing) {
        m_resizing = false;
    }
    unsetCursor();
}

void KexiFormScrollAreaWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    //widget()->update(); //update form elements on too fast mouse move
}
