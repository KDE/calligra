/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIACTIONSELECTIONDIALOG_P_H
#define KEXIACTIONSELECTIONDIALOG_P_H

#include <k3listview.h>

//! @internal
class ActionsListViewBase : public K3ListView
{
public:
    ActionsListViewBase(QWidget* parent);
    virtual ~ActionsListViewBase();

    //! \return item for action \a actionName
    virtual Q3ListViewItem *itemForAction(const QString& actionName);
    void selectAction(const QString& actionName);
};

//! @internal Used by KActionsListView and CurrentFormActionsListView (in column 2)
class KActionsListViewBase : public ActionsListViewBase
{
    Q_OBJECT
public:
    KActionsListViewBase(QWidget* parent);
    virtual ~KActionsListViewBase();
    void init();
    virtual bool isActionVisible(const char* actionName, int actionCategories) const = 0;

protected:
};

#endif
