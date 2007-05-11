
/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
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
    if (  legend->orientation() == Qt::Horizontal ) {
        horizontalRB->setChecked( true );
        topLeftRB->setText( tr( "Left" ) );
        bottomRightRB->setText( tr( "Right" ) );

    } else {
        verticalRB->setChecked( true );
        topLeftRB->setText( tr( "Top" ) );
        bottomRightRB->setText( tr( "Bottom" ) );
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
    legend->setPosition( Position::fromName( mPositionCombo->itemData( mPositionCombo->currentIndex() ).toByteArray() ) );
    legend->setTitleText( titleTextED->text() );
    if (  horizontalRB->isChecked() ) {
        legend->setOrientation( Qt::Horizontal );
        if (  topLeftRB->isChecked() )
            legend->setAlignment(  Qt::AlignLeft );
        else if ( bottomRightRB->isChecked() )
            legend->setAlignment(  Qt::AlignRight );
        else
            legend->setAlignment(  Qt::AlignHCenter );
    } else {
        legend->setOrientation( Qt::Vertical );
        if (  topLeftRB->isChecked() )
            legend->setAlignment(  Qt::AlignTop );
        else if ( bottomRightRB->isChecked() )
            legend->setAlignment(  Qt::AlignBottom );
        else
            legend->setAlignment(  Qt::AlignVCenter );
    }
     if (  showLinesCB->isChecked() )
            legend->setShowLines( true );
        else
            legend->setShowLines( false );

}

void LegendPropertiesWidget::slotPositionChanged( int idx )
{
    if ( d->legend && d->instantApply ) {
        d->legend->setPosition( Position::fromName( mPositionCombo->itemData( idx ).toByteArray() ) );
    } else {
        emit changed();
    }
}

void LegendPropertiesWidget::slotOrientationChanged( bool toggled )
{
    Q_UNUSED( toggled );

    if ( d->legend && d->instantApply ) {
        if (  horizontalRB->isChecked() ) {
            d->legend->setOrientation( Qt::Horizontal );
            topLeftRB->setText( tr( "Left" ) );
            bottomRightRB->setText( tr( "Right" ) );

        } else {
            d->legend->setOrientation(  Qt::Vertical );
            topLeftRB->setText( tr( "Top" ) );
            bottomRightRB->setText( tr( "Bottom" ) );
        }

    } else
        emit changed();

}

void LegendPropertiesWidget::slotAlignmentChanged( bool toggled )
{
    Q_UNUSED( toggled );

    if ( d->legend && d->instantApply ) {
        if (  horizontalRB->isChecked() ) {
            if (  topLeftRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignLeft );
            else if ( bottomRightRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignRight );
            else
                d->legend->setAlignment(  Qt::AlignHCenter );
        } else {
            if (  topLeftRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignTop );
            else if ( bottomRightRB->isChecked() )
                d->legend->setAlignment(  Qt::AlignBottom );
            else
                d->legend->setAlignment(  Qt::AlignVCenter );
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
