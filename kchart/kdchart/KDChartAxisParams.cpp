/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
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

