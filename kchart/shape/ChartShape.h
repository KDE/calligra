/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KCHART_CHART_SHAPE
#define KCHART_CHART_SHAPE

#include <KoShape.h>

#include "koChart.h"

#include "kchart_global.h"

#define ChartShapeId "ChartShape"


class QAbstractItemModel;


namespace KDChart
{
    class Chart;
}

namespace KChart
{

class ChartShape : public KoShape, public KoChart::ChartInterface
{
public:
    ChartShape();
    virtual ~ChartShape();

    KDChart::Chart* chart() const;

    /// Set new chart type and subtype.
    void setChartType( OdfChartType newType, OdfChartSubtype newSubType );

    /// reimplemented
    virtual void setModel( QAbstractItemModel* model );

    /// reimplemented
    virtual void paint( QPainter& painter, const KoViewConverter& converter );
    /// reimplemented
    virtual void saveOdf( KoShapeSavingContext & context ) const;
    /// reimplemented
    virtual bool loadOdf( const KoXmlElement & element, KoShapeLoadingContext &context );

private:
    Q_DISABLE_COPY( ChartShape )

    class Private;
    Private * const d;
};

} // namespace KChart


#endif // KCHART_CHART_SHAPE
