/*
    Copyright (c) 2006 Frans Englich <frans.englich@telia.com>

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or(at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#ifndef KXQTS_TreeSortFilter_H
#define KXQTS_TreeSortFilter_H

#include <QSortFilterProxyModel>

/**
  * @short A sort & filter model for hierarchical item models.
  *
  * The current feature is that, when sorting, numbers are treated as
  * a whole instead of on a character-per-character basis. For example, "myFile-10" is sorted
  * after "myFile-9".
  *
  * @ingroup KXQTS
  * @author Frans Englich <frans.englich@telia.com>
  */
class TreeSortFilter : public QSortFilterProxyModel
{
public:
    /**
      * Creates a TreeSortFilter.
      *
      * @param parent the parent. Must not be @c null.
      */
    explicit TreeSortFilter(QObject *parent);
protected:

    /**
      * Compares @p left and @p right. They are treated as QStrings.
      */
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    inline bool numericLessThan(const QString &l, const QString &r) const;
};

#endif
