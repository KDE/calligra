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
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <KDChartAxisParams.h>

const double KDChartAxisParams::AXIS_LABELS_AUTO_DELTA = 140319.64;
const int KDChartAxisParams::AXIS_LABELS_AUTO_DIGITS = 14364;
const QColor KDChartAxisParams::DEFAULT_AXIS_GRID_COLOR = QColor( 0xA0, 0xA0, 0xA0 );
const int KDChartAxisParams::AXIS_GRID_AUTO_LINEWIDTH = 14364;
const int KDChartAxisParams::AXIS_IGNORE_EMPTY_INNER_SPAN = -1;
const int KDChartAxisParams::DONT_CHANGE_EMPTY_INNER_SPAN_NOW = -2;
KDChartData KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT( 140319.64 );


KDChartAxisParams::~KDChartAxisParams()
{
    // Intentionally left blank for now.
}

