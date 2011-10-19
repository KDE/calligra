/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPROJECTITEMDELEGATE_H
#define KEXIPROJECTITEMDELEGATE_H

#include <QStyledItemDelegate>

class KexiProjectModelItem;
class KexiProject;

//! @short Item delegate for the Kexi navigator tree view
/*! It's currently needed to validate input when renaming items
    (KexiUtils::IdentifierValidator is used for this). 
    @todo In the future custom painting and/or editing may be added through this delegate. */
class KexiProjectItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    KexiProjectItemDelegate(QObject *parent = 0);
    virtual ~KexiProjectItemDelegate();
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;

private:
    class Private;
    Private * const d;
};

#endif // KEXIPROJECTITEMDELEGATE_H
