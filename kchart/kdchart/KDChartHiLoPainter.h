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

#ifndef __KDCHARTHILOPAINTER_H__
#define __KDCHARTHILOPAINTER_H__

#include "KDChartAxesPainter.h"
#include <KDChartTable.h>

#include <qnamespace.h>

class KDChartParams;

class KDChartHiLoPainter : public KDChartAxesPainter, public Qt
{
    friend class KDChartPainter;
protected:
    KDChartHiLoPainter( KDChartParams* params );
    virtual ~KDChartHiLoPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;
};

#endif
