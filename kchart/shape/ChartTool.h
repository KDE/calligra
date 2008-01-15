/* This file is part of the KDE project
 * Copyright (C) 2007      Inge Wallin  <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#ifndef CHARTTYPETOOL_H
#define CHARTTYPETOOL_H


#include <KoTool.h>

#include "ChartShape.h"


class QAction;


namespace KChart
{


/**
 * This is the tool for the text-shape (which is a flake-based plugin).
 */
class ChartTool : public KoTool
{
    Q_OBJECT
public:
    explicit ChartTool(KoCanvasBase *canvas);
    ~ChartTool();

    /// reimplemented from superclass
    virtual void paint( QPainter &painter, const KoViewConverter &converter );

    /// reimplemented from superclass
    virtual void mousePressEvent( KoPointerEvent *event ) ;
    /// reimplemented from superclass
    virtual void mouseMoveEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void mouseReleaseEvent( KoPointerEvent *event );
    /// reimplemented from superclass
    virtual void activate (bool temporary=false);
    /// reimplemented from superclass
    virtual void deactivate();
    /// reimplemented from superclass
    virtual QWidget *createOptionWidget();

private slots:
    void setChartType( OdfChartType type, OdfChartSubtype subtype = NoChartSubtype );
    void setChartSubtype( OdfChartSubtype subtype );
    void setThreeDMode( bool );
    void setDataDirection( Qt::Orientation );
    void setFirstRowIsLabel( bool b );
    void setFirstColumnIsLabel( bool b );

    // Datasets
    void setDatasetColor( int dataset, const QColor& color );
    void setGapBetweenBars( int percent );
    void setGapBetweenSets( int percent );
    
    // Axes
    void setAxisTitle( KDChart::CartesianAxis *axis, const QString& title );
    void setAxisShowGridLines( KDChart::CartesianAxis *axis, bool b );

    // Legend
    void setShowLegend( bool b );
    void setLegendTitle( const QString& title );
    void setLegendTitleFont( const QFont& font );
    void setLegendFont( const QFont& font );
    void setLegendFontSize( int size );
    void setLegendSpacing( int spacing );
    void setLegendShowLines( bool b );
    void setLegendOrientation( Qt::Orientation );
    void setLegendAlignment( Qt::Alignment );
    void setLegendFixedPosition( KDChart::Position position );
    void setLegendBackgroundColor( const QColor& color );
    void setLegendFrameColor( const QColor& color );
    void setLegendShowFrame( bool show );

private:
    void updateActions();

    class Private;
    Private * const d;
};

} // namespace KChart


#endif
