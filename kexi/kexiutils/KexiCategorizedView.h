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

#ifndef KEXICATEGORIZEDVIEW_H
#define KEXICATEGORIZEDVIEW_H

#include "kexiutils_export.h"

#include <KCategoryDrawer>
#include <KCategorizedView>
#include <kdeversion.h>

class QModelIndex;
class QAbstractItemModel;
class QRect;
class QMouseEvent;

typedef KCategoryDrawerV3 KexiCategoryDrawerBase;

//! Modification of category drawer for KCategorizedView: disallows clicking on the drawer
class KEXIUTILS_EXPORT KexiCategoryDrawer : public KexiCategoryDrawerBase
{
public:
    KexiCategoryDrawer();

protected:
#if 0
    void mouseButtonPressed(const QModelIndex&, const QRect&, QMouseEvent *event);

    void mouseButtonReleased(const QModelIndex&, const QRect&, QMouseEvent *event);
#endif
};

//! Single selection model for the drawer.
class KEXIUTILS_EXPORT KexiCategorySingleSelectionModel : public QItemSelectionModel
{
public:
    KexiCategorySingleSelectionModel(QAbstractItemModel* model);
    
    using QItemSelectionModel::select;
    
    //! Reimplemented to disable full category selections.
    //! Shouldn't be needed in KDElibs >= 4.5,
    //! where KexiTemplatesCategoryDrawer::mouseButtonPressed() works.
    void select(const QItemSelection& selection,
                QItemSelectionModel::SelectionFlags command);
};

//! Single selection categorized view.
class KEXIUTILS_EXPORT KexiCategorizedView : public KCategorizedView
{
Q_OBJECT
public:
    explicit KexiCategorizedView(QWidget *parent = 0);

    virtual void setModel(QAbstractItemModel *model);
};

#endif
