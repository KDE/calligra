
/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#include <QWidget>
#include <QDebug>
#include <QLabel>

#include <KDChartPosition.h>
#include <KDChartLegend.h>

#include <KDChartLegendPropertiesWidget.h>
#include <KDChartLegendPropertiesWidget_p.h>


#define d d_func()

using namespace KDChart;

LegendPropertiesWidget::Private::Private()
    :legend(0), instantApply(true)
{

}

LegendPropertiesWidget::Private::~Private()
{
}

LegendPropertiesWidget::LegendPropertiesWidget( QWidget *parent )
    : QWidget( parent ), _d( new Private )
{
    setupUi( this );

    const QStringList labels = KDChart::Position::printableNames();
    const QList<QByteArray> names = KDChart::Position::names();

    for ( int i = 0, end = qMin( labels.size(), names.size() ) ; i != end ; ++i )
        mPositionCombo->addItem( labels[i], names[i] );

    connect( mPositionCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotPositionChanged( int ) ) );
    connect( horizontalRB,  SIGNAL( toggled( bool ) ),
             this,  SLOT( slotOrientationChanged( bool ) ) );
    connect( verticalRB,  SIGNAL( toggled( bool ) ),
             this,  SLOT( slotOrientationChanged( bool ) ) );
    connect( topLeftRB,  SIGNAL( toggled( bool ) ),
             this,  SLOT( slotAlignmentChanged( bool ) ) );
    connect( bottomRightRB,  SIGNAL( toggled( bool ) ),
             this,  SLOT( slotAlignmentChanged( bool ) ) );
    connect( centerRB,  SIGNAL( toggled( bool ) ),
             this,  SLOT( slotAlignmentChanged( bool ) ) );
    connect( titleTextED,  SIGNAL( textChanged( const QString ) ),
             this,  SLOT( slotTitleTextChanged( const QString  ) ) );
    connect( showLinesCB,  SIGNAL( stateChanged( int ) ),
             this,  SLOT( slotShowLineChanged( int  ) ) );

    setEnabled( false );
}

LegendPropertiesWidget::~LegendPropertiesWidget()
{

}

void LegendPropertiesWidget::setLegend( Legend * legend )
{
    d->legend = legend;
    if ( legend ) {
        readFromLegend( legend );
        setEnabled( true );
    } else {
        setEnabled( false);
    }
}

void LegendPropertiesWidget::setInstantApply( bool value )
{
    d->instantApply = value;
}

void LegendPropertiesWidget::readFromLegend( const Legend * legend )
{
    mPositionCombo->setCurrentIndex( mPositionCombo->findData( QByteArray( legend->position().name() ) ) );
    titleTextED->setText( legend->titleText() );

    Position position = Position::fromName( legend->position().name() );
    d->legend->setPosition( position );

    if( position == Position::North ||
        position == Position::South ) {
        topLeftRB->setEnabled( true );
        centerRB->setEnabled( true );
        bottomRightRB->setEnabled( true );
            
        topLeftRB->setText( tr( "Left" ) );
        bottomRightRB->setText( tr( "Right" ) );
        slotAlignmentChanged( false );
    }
    else if( position == Position::West ||
             position == Position::East ) {
        topLeftRB->setEnabled( true );
        centerRB->setEnabled( true );
        bottomRightRB->setEnabled( true );
            
        topLeftRB->setText( tr( "Top" ) );
        bottomRightRB->setText( tr( "Bottom" ) );
        slotAlignmentChanged( false );
    } else {
        topLeftRB->setEnabled( false );
        centerRB->setEnabled( false );
        bottomRightRB->setEnabled( false );
        slotAlignmentChanged( false );
    }

    if (  legend->alignment() == Qt::AlignLeft || legend->alignment() == Qt::AlignTop )
        topLeftRB->setChecked(  true );
    else if ( legend->alignment() == Qt::AlignRight || legend->alignment() == Qt::AlignBottom )
        bottomRightRB->setChecked( true );
    else
        centerRB->setChecked( true );

    if (  legend->showLines() )
        showLinesCB->setChecked( true );
    else
        showLinesCB->setChecked( false );
}

void LegendPropertiesWidget::writeToLegend( Legend * legend )
{
    if ( !legend ) return;
    Position position = Position::fromName( mPositionCombo->itemData( mPositionCombo->currentIndex() ).toByteArray() );
    legend->setPosition( position );
    legend->setTitleText( titleTextED->text() );
    if (  horizontalRB->isChecked() )
        legend->setOrientation( Qt::Horizontal );
    else
        legend->setOrientation( Qt::Vertical );
    
    if( position == Position::North ||
        position == Position::South ) {
        if (  topLeftRB->isChecked() )
            legend->setAlignment(  Qt::AlignLeft );
        else if ( bottomRightRB->isChecked() )
            legend->setAlignment(  Qt::AlignRight );
        else
            legend->setAlignment(  Qt::AlignHCenter );
    }
    else if( position == Position::West ||
             position == Position::East ) {
        if (  topLeftRB->isChecked() )
            legend->setAlignment(  Qt::AlignTop );
        else if ( bottomRightRB->isChecked() )
            legend->setAlignment(  Qt::AlignBottom );
        else
            legend->setAlignment(  Qt::AlignVCenter );
    } else {
        legend->setAlignment( Qt::AlignCenter );
    }
    if (  showLinesCB->isChecked() )
        legend->setShowLines( true );
    else
        legend->setShowLines( false );
}

void LegendPropertiesWidget::slotPositionChanged( int idx )
{
    if ( d->legend && d->instantApply ) {
        Position position = Position::fromName( mPositionCombo->itemData( idx ).toByteArray() );
        d->legend->setPosition( position );

        if( position == Position::North ||
            position == Position::South ) {
            topLeftRB->setEnabled( true );
            centerRB->setEnabled( true );
            bottomRightRB->setEnabled( true );
            
            topLeftRB->setText( tr( "Left" ) );
            bottomRightRB->setText( tr( "Right" ) );
            slotAlignmentChanged( false );
        }
        else if( position == Position::West ||
                 position == Position::East ) {
            topLeftRB->setEnabled( true );
            centerRB->setEnabled( true );
            bottomRightRB->setEnabled( true );
            
            topLeftRB->setText( tr( "Top" ) );
            bottomRightRB->setText( tr( "Bottom" ) );
            slotAlignmentChanged( false );
        } else {
            topLeftRB->setEnabled( false );
            centerRB->setEnabled( false );
            bottomRightRB->setEnabled( false );
            slotAlignmentChanged( false );
        }
    } else {
        emit changed();
    }
}

void LegendPropertiesWidget::slotOrientationChanged( bool toggled )
{
    Q_UNUSED( toggled );

    if ( d->legend && d->instantApply ) {
        if (  horizontalRB->isChecked() )
            d->legend->setOrientation( Qt::Horizontal );
        else
            d->legend->setOrientation(  Qt::Vertical );
    } else
        emit changed();
}

void LegendPropertiesWidget::slotAlignmentChanged( bool toggled )
{
    Q_UNUSED( toggled );

    if ( d->legend && d->instantApply ) {
        Position position = Position::fromName( mPositionCombo->currentText().toLatin1().constData() );
        if (  position == Position::North ||
              position == Position::South ) {
            if ( topLeftRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignLeft );
            else if ( bottomRightRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignRight );
            else
                d->legend->setAlignment(  Qt::AlignHCenter );
        } else if( position == Position::West ||
                   position == Position::East ) {
            if ( topLeftRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignTop );
            else if ( bottomRightRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignBottom );
            else
                d->legend->setAlignment(  Qt::AlignVCenter );
        } else {
            d->legend->setAlignment( Qt::AlignCenter );
        }

    } else
        emit changed();

}


void LegendPropertiesWidget::slotTitleTextChanged( const QString& text )
{
    Q_UNUSED( text );

    if ( d->legend && d->instantApply ) {
        d->legend->setTitleText( titleTextED->text() );
    } else {
        emit changed();
    }
}


void LegendPropertiesWidget::slotShowLineChanged( int state )
{
    Q_UNUSED( state );

    if ( d->legend && d->instantApply ) {
        if (  showLinesCB->isChecked() )
            d->legend->setShowLines( true );
        else
            d->legend->setShowLines( false );
    } else {
        emit changed();
    }
}
