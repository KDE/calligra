
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
#include <KDChartCartesianAxis.h>

#include <KDChartAxisPropertiesWidget.h>
#include <KDChartAxisPropertiesWidget_p.h>



#define d d_func()

using namespace KDChart;

AxisPropertiesWidget::Private::Private()
    :axis(0), instantApply(true)
{

}

AxisPropertiesWidget::Private::~Private()
{
}


AxisPropertiesWidget::AxisPropertiesWidget( QWidget *parent )
    :QWidget( parent ),  _d( new Private )
{
    setupUi( this ),

    connect( mPositionCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotPositionChanged( int ) ) );
    connect( titleED,  SIGNAL( textChanged( const QString ) ),
             this,  SLOT( slotTitleChanged( const QString  ) ) );

    setEnabled( false );
}

AxisPropertiesWidget::~AxisPropertiesWidget()
{

}

void AxisPropertiesWidget::setAxis( CartesianAxis * a )
{
    d->axis = a;
    if ( a ) {
        readFromAxis( a );
        setEnabled( true );
    } else {
        setEnabled( false);
    }
}

void AxisPropertiesWidget::setInstantApply( bool value )
{
    d->instantApply = value;
}

void AxisPropertiesWidget::readFromAxis( const CartesianAxis * a  )
{
    mPositionCombo->setCurrentIndex( a->position() );
    titleED->setText( a->titleText() );
}


void AxisPropertiesWidget::writeToAxis( CartesianAxis * a )
{
    KDChart::CartesianAxis::Position pos =
        static_cast<KDChart::CartesianAxis::Position>(mPositionCombo->currentIndex());

    if ( !a ) return;
    a->setPosition( pos );
    a->setTitleText( titleED->text() );
}

void AxisPropertiesWidget::slotPositionChanged( int idx )
{
     KDChart::CartesianAxis::Position pos =
         static_cast<KDChart::CartesianAxis::Position>(idx);

    if ( d->axis && d->instantApply ) {
        d->axis->setPosition( pos );
    } else {
        emit changed();
    }
}


void AxisPropertiesWidget::slotTitleChanged( const QString& text )
{
    Q_UNUSED( text );

    if ( d->axis && d->instantApply ) {
        d->axis->setTitleText( titleED->text() );
    } else {
        emit changed();
    }
}
