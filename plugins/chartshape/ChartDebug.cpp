/*
   Copyright (c) 2017 Dag Andersen <danders@get2net.dk>
   Copyright (c) 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "ChartDebug.h"

const QLoggingCategory &CHART_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.shape");
    return category;
}

const QLoggingCategory &CHARTLAYOUT_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.layout");
    return category;
}

const QLoggingCategory &CHARTODF_LOG()
{
    static const QLoggingCategory category("calligra.plugin.chart.odf");
    return category;
}
