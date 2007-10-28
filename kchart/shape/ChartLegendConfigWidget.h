/* This file is part of the KDE project
 *
 * Copyright 2007 Johannes Simon <johannes.simon@gmail.com>
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

#ifndef KCHART_CHART_LEGEND_CONFIG_WIDGET
#define KCHART_CHART_LEGEND_CONFIG_WIDGET

// KOffice
#include <KoShapeConfigWidgetBase.h>

// KDChart
#include "kchart_export.h"
#include <KDChartPosition.h>


namespace KChart
{

class CHARTSHAPELIB_EXPORT ChartLegendConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    /// constructor
    explicit ChartLegendConfigWidget( );
    ~ChartLegendConfigWidget();
    
    void setupUi();

    /// reimplemented
    void open( KoShape *shape );
    /// reimplemented
    void save();
    /// reimplemented
    KAction *createAction();

    /// reimplemented 
    virtual bool showOnShapeCreate() { return true; }

public slots:
    void setLegendOrientation( int boxEntryIndex );
    void setLegendAlignment( int boxEntryIndex );
    void setLegendFixedPosition( int buttonGroupIndex );
    void setLegendShowTitle( bool toggled );
    void updateFixedPosition( const KDChart::Position );
    
signals:
    void legendTitleChanged( const QString& );
    void legendFontChanged( const QFont& font );
    void legendTitleFontChanged( const QFont& font );
    void legendFontSizeChanged( int size );
    void legendSpacingChanged( int spacing );
    void legendShowLinesToggled( bool toggled );
    void legendOrientationChanged( Qt::Orientation orientation );
    void legendAlignmentChanged( Qt::Alignment alignment );
    void legendFixedPositionChanged( KDChart::Position position );

private:
    class Private;
    Private * const d;
};

} // namespace KChart

#endif // KCHART_CHART_LEGEND_CONFIG_WIDGET
