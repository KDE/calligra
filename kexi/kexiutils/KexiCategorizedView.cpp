/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiCategorizedView.h"
 
KexiCategoryDrawer::KexiCategoryDrawer()
    : KexiCategoryDrawerBase(0)
{
}

#if 0
void KexiCategoryDrawer::mouseButtonPressed(const QModelIndex&, const QRect&, QMouseEvent *event)
{
    event->accept();
}

void KexiCategoryDrawer::mouseButtonReleased(const QModelIndex&, const QRect&, QMouseEvent *event)
{
    event->accept();
}
#endif

// ----

KexiCategorySingleSelectionModel::KexiCategorySingleSelectionModel(QAbstractItemModel* model)
    : QItemSelectionModel(model)
{
}

void KexiCategorySingleSelectionModel::select(const QItemSelection& selection,
                                              QItemSelectionModel::SelectionFlags command)
{
    // kDebug() << selection.indexes().count() << command;
    if ((command & QItemSelectionModel::Select) && 
        !(command & QItemSelectionModel::Clear) &&
        (selection.indexes().count() > 1 || !this->selection().indexes().isEmpty()))
    {
        return;
    }
    QItemSelectionModel::select(selection, command);
}

// ----

KexiCategorizedView::KexiCategorizedView(QWidget *parent)
 : KCategorizedView(parent)
{
    //setWordWrap(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setMouseTracking(true);
    setViewMode(QListView::IconMode);
    KexiCategoryDrawer* categoryDrawer = new KexiCategoryDrawer;
    setCategoryDrawer(categoryDrawer);
}

void KexiCategorizedView::setModel(QAbstractItemModel *model)
{
    KCategorizedView::setModel(model);
    setSelectionModel(new KexiCategorySingleSelectionModel(model));
}

#include "KexiCategorizedView.moc"
