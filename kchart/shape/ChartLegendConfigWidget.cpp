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
    Private();
    ~Private();

    ChartShape                  *chart;
    Ui::ChartLegendConfigWidget  ui;
    QButtonGroup                *positionButtonGroup;
    int                          lastHorizontalAlignment;
    int                          lastVerticalAlignment;
    KDChart::Position            fixedPosition;
    KDChart::Position            lastFixedPosition;
};

ChartLegendConfigWidget::Private::Private()
{
    lastHorizontalAlignment = 1; // Qt::AlignCenter
    lastVerticalAlignment   = 1; // Qt::AlignCenter
    fixedPosition           = KDChart::Position::East;
    lastFixedPosition       = KDChart::Position::East;
}

ChartLegendConfigWidget::Private::~Private()
{
    delete positionButtonGroup;
}

const int NUM_FIXED_POSITIONS = 8;

const KDChart::Position buttonIndexToFixedPosition[NUM_FIXED_POSITIONS] =
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

    d->positionButtonGroup = new QButtonGroup;
    d->positionButtonGroup->setExclusive( true );

    d->positionButtonGroup->addButton( d->ui.positionNorth, 0 );
    d->ui.positionNorth->setIcon( KIcon( "chart_legend_top" ) );
    d->positionButtonGroup->addButton( d->ui.positionEast,  1 );
    d->ui.positionEast->setIcon( KIcon( "chart_legend_right" ) );
    d->positionButtonGroup->addButton( d->ui.positionSouth, 2 );
    d->ui.positionSouth->setIcon( KIcon( "chart_legend_top" ) );
    d->positionButtonGroup->addButton( d->ui.positionWest,  3 );
    d->ui.positionWest->setIcon( KIcon( "chart_legend_left" ) );
    
    d->positionButtonGroup->addButton( d->ui.positionNorthWest, 4 );
    d->ui.positionNorthWest->setIcon( KIcon( "chart_legend_topleft" ) );
    d->positionButtonGroup->addButton( d->ui.positionNorthEast, 5 );
    d->ui.positionNorthEast->setIcon( KIcon( "chart_legend_topright" ) );
    d->positionButtonGroup->addButton( d->ui.positionSouthWest, 6 );
    d->ui.positionSouthWest->setIcon( KIcon( "chart_legend_bottomleft" ) );
    d->positionButtonGroup->addButton( d->ui.positionSouthEast, 7 );
    d->ui.positionSouthEast->setIcon( KIcon( "chart_legend_bottomright" ) );

    d->ui.orientation->addItem( i18n( "Horizontal" ), Qt::Horizontal );
    d->ui.orientation->addItem( i18n( "Vertical" ), Qt::Vertical );
    d->ui.orientation->setCurrentIndex( 1 );

    d->ui.alignment->addItem( i18n( "Top" ), Qt::AlignTop );
    d->ui.alignment->addItem( i18n( "Center" ), Qt::AlignCenter );
    d->ui.alignment->addItem( i18n( "Bottom" ), Qt::AlignBottom );

    d->ui.alignment->setCurrentIndex( 1 );

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

    connect ( d->positionButtonGroup, SIGNAL( buttonClicked( int ) ),
              this,                   SLOT( setLegendFixedPosition( int ) ) );
}

void ChartLegendConfigWidget::setLegendOrientation( int boxEntryIndex )
{
    emit legendOrientationChanged( ( Qt::Orientation ) ( d->ui.orientation->itemData( boxEntryIndex ).toInt() ) );
}

void ChartLegendConfigWidget::setLegendAlignment( int boxEntryIndex )
{
    if (    d->fixedPosition == KDChart::Position::North
         || d->fixedPosition == KDChart::Position::South ) {
        d->lastHorizontalAlignment = d->ui.alignment->currentIndex();
     } else if (   d->fixedPosition == KDChart::Position::East
                || d->fixedPosition == KDChart::Position::West ) {
        d->lastVerticalAlignment = d->ui.alignment->currentIndex();
    }
    emit legendAlignmentChanged( ( Qt::Alignment ) ( d->ui.alignment->itemData( boxEntryIndex ).toInt() ) );
}

void ChartLegendConfigWidget::setLegendFixedPosition( int buttonGroupIndex )
{
    d->lastFixedPosition = d->fixedPosition;
    d->fixedPosition = buttonIndexToFixedPosition[ buttonGroupIndex ];
    emit legendFixedPositionChanged( buttonIndexToFixedPosition[ buttonGroupIndex ] );
}

void ChartLegendConfigWidget::updateFixedPosition( const KDChart::Position position )
{
    if (    position == KDChart::Position::North
         || position == KDChart::Position::South ) {
        d->ui.alignment->setEnabled( true );
        d->ui.alignment->setItemText( 0, i18n( "Left" ) );
        d->ui.alignment->setItemData( 0, Qt::AlignLeft );
        d->ui.alignment->setItemData( 1, Qt::AlignCenter );
        d->ui.alignment->setItemText( 2, i18n( "Right" ) );
        d->ui.alignment->setItemData( 2, Qt::AlignRight );
        // Set the alignment to the one last used for horizontal legend alignment
        if (    d->lastFixedPosition != KDChart::Position::North
             && d->lastFixedPosition != KDChart::Position::South ) {
            // Make sure that the combobox gets updated. Since we changed the values of the entries,
            // same index doesn't mean same value, though it will think so. Solution: Select no entry first
            d->ui.alignment->blockSignals( true );
            d->ui.alignment->setCurrentIndex( -1 );
            d->ui.alignment->blockSignals( false );

            d->ui.alignment->setCurrentIndex( d->lastHorizontalAlignment );
        }
    } else if (    position == KDChart::Position::East
                || position == KDChart::Position::West ) {
        d->ui.alignment->setEnabled( true );
        d->ui.alignment->setItemText( 0, i18n( "Top" ) );
        d->ui.alignment->setItemData( 0, Qt::AlignTop );
        d->ui.alignment->setItemData( 1, Qt::AlignCenter );
        d->ui.alignment->setItemText( 2, i18n( "Bottom" ) );
        d->ui.alignment->setItemData( 2, Qt::AlignBottom );
        // Set the alignment to the one last used for vertical legend alignment
        if (    d->lastFixedPosition != KDChart::Position::East
             && d->lastFixedPosition != KDChart::Position::West ) {
            // Make sure that the combobox gets updated. Since we changed the values of the entries,
            // same index doesn't mean same value, though it will think so. Solution: Select no entry first
            d->ui.alignment->blockSignals( true );
            d->ui.alignment->setCurrentIndex( -1 );
            d->ui.alignment->blockSignals( false );

            d->ui.alignment->setCurrentIndex( d->lastVerticalAlignment );
        }
    } else {
        d->ui.alignment->setEnabled( false );
    }

    for( int i = 0; i < NUM_FIXED_POSITIONS; i++ ) {
        if( position == buttonIndexToFixedPosition[i] ) {
            if ( d->positionButtonGroup->checkedId() != i )
                d->positionButtonGroup->button( i )->setChecked( true );
            break;
        }
    }
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
