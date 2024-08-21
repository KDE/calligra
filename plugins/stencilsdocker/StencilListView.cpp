/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2010-2014 Yue Liu <yue.liu@mail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "StencilListView.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

StencilListView::StencilListView(QWidget *parent)
    : QListView(parent)
{
    int iconSize = style()->pixelMetric(QStyle::PM_IconViewIconSize, nullptr, this);
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

void StencilListView::dragEnterEvent(QDragEnterEvent *event)
{
    // debugStencilBox << event->mimeData()->formats();
    //  TODO
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void StencilListView::dropEvent(QDropEvent *event)
{
    // TODO
    event->acceptProposedAction();
}
