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

#include "TreeSortFilter.h"

TreeSortFilter::TreeSortFilter(QObject *p) : QSortFilterProxyModel(p)
{
    Q_ASSERT(p);
}

bool TreeSortFilter::lessThan(const QModelIndex &left,
                              const QModelIndex &right) const
{
    const QVariant leftData(sourceModel()->data(left));
    const QVariant rightData(sourceModel()->data(right));

    return numericLessThan(leftData.toString(), rightData.toString());
}

bool TreeSortFilter::numericLessThan(const QString &l, const QString &r) const
{
    QString ls(l);
    QString rs(r);
    const int len = (l.length() > r.length() ? r.length() : l.length());

    for(int i = 0; i < len; ++i) {
        const QChar li(l.at(i));
        const QChar ri(r.at(i));

        if(li >= QLatin1Char('0') &&
                li <= QLatin1Char('9') &&
                ri >= QLatin1Char('0') &&
                ri <= QLatin1Char('9')) {
            ls = l.mid(i);
            rs = r.mid(i);
            break;
        } else if(li != ri)
            break;
    }

    const int ld = ls.toInt();
    const int rd = rs.toInt();

    if(ld == rd)
        return ls.localeAwareCompare(rs) < 0;
    else
        return ld < rd;
}

bool TreeSortFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(filterRegExp().isEmpty())
        return true;

    QModelIndex current(sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent));

    if(sourceModel()->hasChildren(current)) {
        bool atLeastOneValidChild = false;
        int i = 0;
        while(!atLeastOneValidChild) {
            const QModelIndex child(current.child(i, current.column()));
            if(!child.isValid())
                // No valid child
                break;

            atLeastOneValidChild = filterAcceptsRow(i, current);
            i++;
        }
        return atLeastOneValidChild;
    }

    return sourceModel()->data(current).toString().contains(filterRegExp());
}

// vim: et:ts=4:sw=4:sts=4
