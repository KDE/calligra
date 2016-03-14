/* This file is part of the Calligra project
 * Copyright (C) 2010-2014 Yue Liu <yue.liu@mail.com>
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

#include "StencilListView.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>
#include <QMimeData>

StencilListView::StencilListView(QWidget* parent) :
    QListView(parent)
{
    int iconSize = style()->pixelMetric(QStyle::PM_IconViewIconSize, 0, this);
    setFocusPolicy(Qt::NoFocus);
    setFrameShape(QFrame::NoFrame);
    setIconSize(QSize(iconSize, iconSize));
    setSpacing(1);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDropIndicatorShown(true);
    setAcceptDrops(true);

    setEditTriggers(QAbstractItemView::AnyKeyPressed);
}

void StencilListView::dragEnterEvent(QDragEnterEvent* event)
{
    //debugStencilBox << event->mimeData()->formats();
    // TODO
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void StencilListView::dropEvent(QDropEvent* event)
{
    // TODO
    event->acceptProposedAction();
}
