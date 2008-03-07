/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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

#ifndef KCHART_AXIS_H
#define KCHART_AXIS_H

// Local
#include "ChartShape.h"

namespace KChart {

class Axis
{
public:
    Axis( PlotArea *parent );
    ~Axis();
	
    AxisPosition position() const;
    TextLabel *title() const;
    QString titleText() const;
    QString id() const;
    AxisDimension dimension() const;
    QList<DataSet*> dataSets() const;
    double majorInterval() const;
    double minorInterval() const;
    int minorIntervalDevisor() const;
    bool showInnerMinorTicks() const;
    bool showOuterMinorTicks() const;
    bool showInnerMajorTicks() const;
    bool showOuterMajorTicks() const;
    bool scalingIsLogarithmic() const;
    bool showGrid() const;
    Qt::Orientation orientation();
	
    void setPosition( AxisPosition position );
    void setTitleText( const QString &text );
    bool attachDataSet( DataSet *dataSet );
    bool detachDataSet( DataSet *dataSet );
    void setMajorInterval( double interval );
    void setMinorInterval( double interval );
    void setMinorIntervalDevisor( int devisor );
    void setShowInnerMinorTicks( bool showTicks );
    void setShowOuterMinorTicks( bool showTicks );
    void setShowInnerMajorTicks( bool showTicks );
    void setShowOuterMajorTicks( bool showTicks );
    void setScalingLogarithmic( bool logarithmicScaling );
    void setShowGrid( bool showGrid );
    void setThreeD( bool threeD );
    
    bool loadOdf( const KoXmlElement &axisElement, KoShapeLoadingContext &context );
    void saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles );
    
    KDChart::CartesianAxis *kdAxis() const;
    KDChart::AbstractCoordinatePlane *kdPlane() const;
    
    void plotAreaChartTypeChanged( ChartType chartType );
    void plotAreaChartSubTypeChanged( ChartSubtype chartSubType );
    
    void update() const;
    
private:
    
    class Private;
    Private *const d;
};

} // Namespace KChart

#endif
