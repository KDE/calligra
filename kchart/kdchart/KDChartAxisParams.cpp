/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KDChart library.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid commercial KDChart licenses may use this file in
** accordance with the KDChart Commercial License Agreement provided with
** the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#include <KDChartAxisParams.h>
#ifndef KDCHART_MASTER_CVS
#include "KDChartAxisParams.moc"
#endif

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
