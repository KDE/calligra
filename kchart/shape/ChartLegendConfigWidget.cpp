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

// Local
#include "ChartLegendConfigWidget.h"

// KDE
#include <KDebug>
#include <KIconLoader>

// KOffice
#include "KoCanvasResourceProvider.h"

// KChart
#include "ui_ChartLegendConfigWidget.h"
#include "ChartShape.h"


using namespace KChart;

class ChartLegendConfigWidget::Private
{
public:
    ChartShape                  *chart;
    Ui::ChartLegendConfigWidget  ui;
};


ChartLegendConfigWidget::ChartLegendConfigWidget()
    : KoShapeConfigWidgetBase()
    , d( new Private )
{
    setupUi();
}

ChartLegendConfigWidget::~ChartLegendConfigWidget()
{
    delete d;
}

void ChartLegendConfigWidget::setupUi()
{
    d->ui.setupUi( this );
    d->ui.positionNorth->setIcon( KIcon( "chart_legend_top" ) );
    d->ui.positionEast->setIcon( KIcon( "chart_legend_right" ) );
    d->ui.positionSouth->setIcon( KIcon( "chart_legend_top" ) );
    d->ui.positionWest->setIcon( KIcon( "chart_legend_left" ) );
    
    d->ui.positionNorthWest->setIcon( KIcon( "chart_legend_topleft" ) );
    d->ui.positionNorthEast->setIcon( KIcon( "chart_legend_topright" ) );
    d->ui.positionSouthWest->setIcon( KIcon( "chart_legend_bottomleft" ) );
    d->ui.positionSouthEast->setIcon( KIcon( "chart_legend_bottomright" ) );

    d->ui.font->setSampleText( i18n( "ABC" ) );
    
    connect( d->ui.title, SIGNAL( textChanged( const QString& ) ),
             this,        SIGNAL( legendTitleChanged( const QString& ) ) );
    connect (d->ui.titleFont, SIGNAL( fontSelected( const QFont& ) ),
             this,       SIGNAL( legendTitleFontChanged( const QFont& ) ) );
    connect (d->ui.font, SIGNAL( fontSelected( const QFont& ) ),
             this,       SIGNAL( legendFontChanged( const QFont& ) ) );
    connect (d->ui.spacing, SIGNAL( valueChanged( int ) ),
             this,       SIGNAL( legendSpacingChanged( int ) ) );
    connect (d->ui.showLines, SIGNAL( toggled( bool ) ),
             this,            SIGNAL( legendShowLinesToggled( bool ) ) );
}

void ChartLegendConfigWidget::open( KoShape* chart )
{
    d->chart = dynamic_cast<ChartShape*>( chart );
}

void ChartLegendConfigWidget::save()
{
}

KAction* ChartLegendConfigWidget::createAction()
{
    return 0;
}

#include "ChartLegendConfigWidget.moc"
