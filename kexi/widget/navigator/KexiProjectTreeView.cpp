/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiProjectTreeView.h"
#include "KexiProjectModel.h"
#include "KexiProjectModelItem.h"

#include <QHeaderView>
#include <KDebug>

KexiProjectTreeView::KexiProjectTreeView(QWidget *parent)
        : QTreeView(parent)
{
    setObjectName("KexiProjectTreeView");
    setEditTriggers(QAbstractItemView::EditKeyPressed);
    header()->hide();
    setAllColumnsShowFocus(true);
}

KexiProjectTreeView::~KexiProjectTreeView()
{
}

void KexiProjectTreeView::setModel(KexiProjectModel *model)
{
    if (model == this->model()) {
        return;
    }
    if (this->model()) {
        this->model()->disconnect(this);
    }
    QTreeView::setModel(model);
    if (model) {
        connect(model, SIGNAL(highlightSearchedItem(QModelIndex)),
                this, SLOT(slotHighlightSearchedItem(QModelIndex)));
        connect(model, SIGNAL(activateSearchedItem(QModelIndex)),
                this, SLOT(slotActivateSearchedItem(QModelIndex)));
    }
}

void KexiProjectTreeView::slotHighlightSearchedItem(const QModelIndex &index)
{
    scrollTo(index);
    KexiProjectModel* model = qobject_cast<KexiProjectModel*>(this->model());
    update(model->itemWithSearchHighlight());
    update(index);
}

void KexiProjectTreeView::slotActivateSearchedItem(const QModelIndex &index)
{
    setFocus();
    scrollTo(index);
    setCurrentIndex(index);
}

void KexiProjectTreeView::drawBranches(QPainter *painter, const QRect &rect,
                                       const QModelIndex &index) const
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    Q_UNUSED(index);
    // Don't draw.
}
