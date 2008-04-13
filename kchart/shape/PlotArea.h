/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>

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

#ifndef KCHART_PLOTAREA_H
#define KCHART_PLOTAREA_H

// Local
#include "ChartShape.h"

// Qt
#include <QObject>
#include <QList>

namespace KChart {

class CHARTSHAPELIB_EXPORT PlotArea : public QObject
{
    Q_OBJECT
    
public:
    PlotArea( ChartShape *parent );
    ~PlotArea();
    
    ProxyModel *proxyModel() const;

    QPointF position() const;
    QSizeF  size() const;
    QList<Axis*> axes() const;
    QList<DataSet*> dataSets() const;
    int dataSetCount() const;
    Surface *wall() const;
    Surface *floor() const;
    ThreeDScene *threeDScene() const;
    Axis *xAxis() const;
    Axis *yAxis() const;
    Axis *secondaryXAxis() const;
    Axis *secondaryYAxis() const;
    bool isThreeD() const;
    ChartType chartType() const;
    ChartSubtype chartSubType() const;
    
    int gapBetweenBars() const;
    int gapBetweenSets() const;

    void setPosition( const QPointF &position );
    void setSize( const QSizeF &size );
    bool addAxis( Axis *axis );
    bool removeAxis( Axis *axis );
    
    void setGapBetweenBars( int percent );
    void setGapBetweenSets( int percent );
    
    bool loadOdf( const KoXmlElement &plotAreaElement, KoShapeLoadingContext &context );
    void saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const;
    void saveOdfSubType( KoXmlWriter &bodyWriter, KoGenStyle &plotAreaStyle ) const;
    
    void setChartType( ChartType type );
    void setChartSubType( ChartSubtype subType );
    
    void setThreeD( bool threeD );
    
    ChartShape *parent() const;
    
    void paint( QPainter &painter );
    
    KDChart::AbstractCoordinatePlane *kdPlane() const;
    KDChart::Chart *kdChart() const;
    
    bool registerKdDiagram( KDChart::AbstractDiagram *diagram );
    bool deregisterKdDiagram( KDChart::AbstractDiagram *diagram );
    
public slots:
    void requestRepaint() const;
    void dataSetCountChanged();
    void update() const;

private:
    class Private;
    Private *const d;
};

} // Namespace KChart

#endif // KCHART_PLOTAREA_H

