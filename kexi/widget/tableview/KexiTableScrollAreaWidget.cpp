/* This file is part of the KDE project
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiTableScrollAreaWidget.h"
#include "KexiTableScrollArea.h"

#include <QPainter>

KexiTableScrollAreaWidget::KexiTableScrollAreaWidget(KexiTableScrollArea *area)
    : QWidget(), scrollArea(area)
{
    setMouseTracking(true);
    setAutoFillBackground(true);
}

KexiTableScrollAreaWidget::~KexiTableScrollAreaWidget()
{
}

void KexiTableScrollAreaWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setClipRect(e->rect());
    //kDebug() << e->rect() << rect() << scrollArea->viewportGeometry();
    scrollArea->drawContents(&p);
}

void KexiTableScrollAreaWidget::mousePressEvent(QMouseEvent *e)
{
    scrollArea->contentsMousePressEvent(e);
}

void KexiTableScrollAreaWidget::mouseReleaseEvent(QMouseEvent *e)
{
    scrollArea->contentsMouseReleaseEvent(e);
}

void KexiTableScrollAreaWidget::mouseMoveEvent(QMouseEvent *e)
{
    scrollArea->contentsMouseMoveEvent(e);
}

void KexiTableScrollAreaWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    scrollArea->contentsMouseDoubleClickEvent(e);
}

void KexiTableScrollAreaWidget::contextMenuEvent(QContextMenuEvent *e)
{
    scrollArea->contentsContextMenuEvent(e);
}

void KexiTableScrollAreaWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
}
