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

#include <klocale.h>
#include <kglobal.h>

namespace KPlato
{

class IntervalItem : public QTreeWidgetItem
{
public:
    explicit IntervalItem(QTreeWidget * parent, QTime start, int length)
    : QTreeWidgetItem(parent)
    {
        setInterval( start, (double)(length) / (1000. * 60. * 60. ) ); // ms -> hours
    }
    explicit IntervalItem(QTreeWidget * parent, QTime start, double length)
    : QTreeWidgetItem(parent)
    {
        setInterval( start, length );
    }
      
    TimeInterval interval() { return TimeInterval(m_start, (int)(m_length * (1000. * 60. * 60. ) ) ); }

    void setInterval( const QTime &time, double length )
    {
        m_start = time;
        m_length = length;
        setText( 0, KGlobal::locale()->formatTime( time ) );
        setText( 1, KGlobal::locale()->formatNumber( length ) );
    }

private:
    QTime m_start;
    double m_length; // In hours
};

}  //KPlato namespace

#endif /* INTERVALITEM_H */

