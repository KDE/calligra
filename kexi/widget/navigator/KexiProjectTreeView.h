/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPROJECTTREEVIEW_H
#define KEXIPROJECTTREEVIEW_H

#include <QTreeView>
#include <QModelIndex>

namespace KexiPart
{
    class Item;
    class Part;
}
class KexiProjectModel;

/*! @internal A tree view for project navigator widget */
class KexiProjectTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit KexiProjectTreeView(QWidget *parent = 0);
    virtual ~KexiProjectTreeView();

    using QTreeView::setModel;
    void setModel(KexiProjectModel *model);

protected:
    virtual void drawBranches(QPainter *painter,
                              const QRect &rect,
                              const QModelIndex &index) const;
protected Q_SLOTS:
    void slotHighlightSearchedItem(const QModelIndex &index);
    void slotActivateSearchedItem(const QModelIndex &index);
};

#endif
