/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KDCHARTDATAREGION__
#define __KDCHARTDATAREGION__

#include <qregion.h>
#include <qlist.h>

/*!
  \internal
*/
struct KDChartDataRegion
{
    KDChartDataRegion( QRegion reg, uint r, uint c )
    {
        region = reg;
        row = r;
        col = c;
    }

    QRegion region;
    uint row;
    uint col;
};


typedef QList < KDChartDataRegion > KDChartDataRegionList;

#endif
