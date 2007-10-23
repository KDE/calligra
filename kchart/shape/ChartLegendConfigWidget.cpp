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

// Qt
#include <QButtonGroup>

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

const KDChart::Position buttonIndexToFixedPosition[] =
{
    KDChart::Position::North,
    KDChart::Position::East,
    KDChart::Position::South,
    KDChart::Position::West,
    KDChart::Position::NorthWest,
    KDChart::Position::NorthEast,
    KDChart::Position::SouthWest,
    KDChart::Position::SouthEast
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

    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->setExclusive( true );

    buttonGroup->addButton( d->ui.positionNorth, 0 );
    d->ui.positionNorth->setIcon( KIcon( "chart_legend_top" ) );
    buttonGroup->addButton( d->ui.positionEast,  1 );
    d->ui.positionEast->setIcon( KIcon( "chart_legend_right" ) );
    buttonGroup->addButton( d->ui.positionSouth, 2 );
    d->ui.positionSouth->setIcon( KIcon( "chart_legend_top" ) );
    buttonGroup->addButton( d->ui.positionWest,  3 );
    d->ui.positionWest->setIcon( KIcon( "chart_legend_left" ) );
    
    buttonGroup->addButton( d->ui.positionNorthWest, 4 );
    d->ui.positionNorthWest->setIcon( KIcon( "chart_legend_topleft" ) );
    buttonGroup->addButton( d->ui.positionNorthEast, 5 );
    d->ui.positionNorthEast->setIcon( KIcon( "chart_legend_topright" ) );
    buttonGroup->addButton( d->ui.positionSouthWest, 6 );
    d->ui.positionSouthWest->setIcon( KIcon( "chart_legend_bottomleft" ) );
    buttonGroup->addButton( d->ui.positionSouthEast, 7 );
    d->ui.positionSouthEast->setIcon( KIcon( "chart_legend_bottomright" ) );

    d->ui.orientation->addItem( i18n( "Horizontal" ), Qt::Horizontal );
    d->ui.orientation->addItem( i18n( "Vertical" ), Qt::Vertical );
    d->ui.orientation->setCurrentIndex( 1 );

    d->ui.alignment->addItem( i18n( "Left" ), Qt::AlignLeft );
    d->ui.alignment->addItem( i18n( "Center" ), Qt::AlignCenter );
    d->ui.alignment->addItem( i18n( "Right" ), Qt::AlignRight );

    d->ui.font->setSampleText( i18n( "ABC" ) );
    
    connect( d->ui.title, SIGNAL( textChanged( const QString& ) ),
             this,        SIGNAL( legendTitleChanged( const QString& ) ) );
    connect( d->ui.titleFont, SIGNAL( fontSelected( const QFont& ) ),
             this,       SIGNAL( legendTitleFontChanged( const QFont& ) ) );
    connect( d->ui.font, SIGNAL( fontSelected( const QFont& ) ),
             this,       SIGNAL( legendFontChanged( const QFont& ) ) );
    connect( d->ui.spacing, SIGNAL( valueChanged( int ) ),
             this,       SIGNAL( legendSpacingChanged( int ) ) );
    connect( d->ui.showLines, SIGNAL( toggled( bool ) ),
             this,            SIGNAL( legendShowLinesToggled( bool ) ) );

    connect( d->ui.orientation, SIGNAL( currentIndexChanged( int ) ),
             this,              SLOT( setLegendOrientation( int ) ) );
    connect( d->ui.alignment, SIGNAL( currentIndexChanged( int ) ),
             this,            SLOT( setLegendAlignment( int ) ) );

    connect ( buttonGroup, SIGNAL( buttonClicked( int ) ),
              this,        SLOT( setLegendFixedPosition( int ) ) );
}

void ChartLegendConfigWidget::setLegendOrientation( int boxEntryIndex )
{
    emit legendOrientationChanged( ( Qt::Orientation ) ( d->ui.orientation->itemData( boxEntryIndex ).toInt() ) );
}

void ChartLegendConfigWidget::setLegendAlignment( int boxEntryIndex )
{
    emit legendAlignmentChanged( ( Qt::Alignment ) ( d->ui.alignment->itemData( boxEntryIndex ).toInt() ) );
}

void ChartLegendConfigWidget::setLegendFixedPosition( int buttonGroupIndex )
{
    emit legendFixedPositionChanged( buttonIndexToFixedPosition[ buttonGroupIndex ] );
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
