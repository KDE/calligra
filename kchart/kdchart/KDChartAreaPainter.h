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

#ifndef __KDCHARTAREAPAINTER_H__
#define __KDCHARTAREAPAINTER_H__

#include "KDChartLinesPainter.h"
#include <KDChartTable.h>

class KDChartParams;

class KDChartAreaPainter : public KDChartLinesPainter
{
    friend class KDChartPainter;
protected:
    KDChartAreaPainter( KDChartParams* params );
    virtual ~KDChartAreaPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
};

#endif
