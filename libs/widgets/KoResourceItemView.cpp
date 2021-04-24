/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoResourceItemView.h"
#include <QEvent>
#include <QHelpEvent>

#include <QDebug>

KoResourceItemView::KoResourceItemView(QWidget *parent)
    : KoTableView(parent)
{
}

bool KoResourceItemView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::ToolTip && model()) {
        QHelpEvent *he = static_cast<QHelpEvent *>(event);
        QStyleOptionViewItem option = viewOptions();
        QModelIndex index = model()->buddy(indexAt(he->pos()));
        if (index.isValid()) {
            option.rect = visualRect(index);
            m_tip.showTip(this, he->pos(), option, index);
            return true;
        }
    }

    return QTableView::viewportEvent(event);
}


void KoResourceItemView::selectionChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/)
{
    if (!selected.isEmpty())
        emit currentResourceChanged(selected.indexes().first());
}

void KoResourceItemView::contextMenuEvent(QContextMenuEvent *event)
{
    QTableView::contextMenuEvent(event);
    emit contextMenuRequested(event->globalPos());
}
