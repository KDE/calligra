/* This file is part of the KDE project
   Copyright (C) 2004, 2007 Dag Andersen <danders@get2net.dk>

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

#include <QTreeWidgetItem>

namespace KPlato
{

class IntervalItem : public QTreeWidgetItem
{
public:
    explicit IntervalItem(QTreeWidget * parent, QTime start, int length)
    : QTreeWidgetItem(parent),
      m_start(start)
    {
        m_length = (double)(length) / (1000 * 60 * 60 ); // ms -> hours
        setText( 0, start.toString() );
        setText( 1, QString("%1" ).arg( m_length ) );
    }
    explicit IntervalItem(QTreeWidget * parent, QTime start, double length)
    : QTreeWidgetItem(parent),
      m_start(start),
      m_length(length)
    {
        setText( 0, start.toString() );
        setText( 1, QString("%1" ).arg( length ) );
    }
      
    TimeInterval interval() { return TimeInterval(m_start, ( (int)(m_length) * 1000 * 60 * 60 ) ); }

private:
    QTime m_start;
    double m_length; // In hours
};

}  //KPlato namespace

#endif /* INTERVALITEM_H */

