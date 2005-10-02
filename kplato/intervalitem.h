/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef INTERVALITEM_H
#define INTERVALITEM_H

#include <qlistview.h>
#include <qdatetime.h>
#include <qpair.h>

namespace KPlato
{

class IntervalItem : public QListViewItem
{
public:
    IntervalItem(QListView * parent, QTime start, QTime end)
    : QListViewItem(parent, QString("%1  -  %2").arg(start.toString(), end.toString())),
      m_start(start),
      m_end(end)
    {}
    QPair<QTime, QTime> interval() { return QPair<QTime, QTime>(m_start, m_end); }

private:
    QTime m_start;
    QTime m_end;
};

}  //KPlato namespace

#endif /* INTERVALITEM_H */

