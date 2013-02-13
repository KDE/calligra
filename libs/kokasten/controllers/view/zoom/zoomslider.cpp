/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "zoomslider.h"

// Kasten gui
#include <zoomable.h>
// Kasten core
#include <abstractmodel.h>
// KDE
#include <KIcon>
#include <KLocale>
// Qt
#include <QtGui/QSlider>
#include <QtGui/QToolButton>
#include <QtGui/QLayout>
#include <QtGui/QApplication>
#include <QtGui/QHelpEvent>


namespace Kasten2
{

static const int ZoomSliderWidth = 150;

// TODO: look at Dolphin/Krita/KOffice zoom tool

// TODO: different zoom strategies: fixed step size, relative step size
// where to put this, behind interface? or better into a zoomtool?

ZoomSlider::ZoomSlider( QWidget* parent )
  : QWidget( parent ), mModel( 0 ), mZoomControl( 0 )
{
    mZoomOutButton = new QToolButton( this );
    mZoomOutButton->setIcon( KIcon( QLatin1String("zoom-out") ) );
    mZoomOutButton->setAutoRaise( true );

    mSlider = new QSlider( Qt::Horizontal, this );

    mZoomInButton = new QToolButton( this );
    mZoomInButton->setIcon( KIcon( QLatin1String("zoom-in") ) );
    mZoomInButton->setAutoRaise( true );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );
    layout->addWidget( mZoomOutButton );
    layout->addWidget( mSlider );
    layout->addWidget( mZoomInButton );

    connect( mZoomOutButton, SIGNAL(clicked()), SLOT(zoomOut()) );
    connect( mZoomInButton, SIGNAL(clicked()), SLOT(zoomIn()) );
    connect( mSlider, SIGNAL(valueChanged(int)), SLOT(onSliderValueChanged(int)) );
    connect( mSlider, SIGNAL(sliderMoved(int)), SLOT(onSliderMoved(int)) );

    setFixedWidth( ZoomSliderWidth );

    setTargetModel( 0 );
}


void ZoomSlider::setTargetModel( AbstractModel* model )
{
    if( mModel ) mModel->disconnect( this );

    mModel = model ? model->findBaseModelWithInterface<If::Zoomable*>() : 0;
    mZoomControl = mModel ? qobject_cast<If::Zoomable *>( mModel ) : 0;

    const bool hasView = ( mZoomControl != 0 );
    if( hasView )
    {
        mSlider->setSingleStep( 1 ); // mZoomControl->zoomLevelSingleStep()?
        mSlider->setPageStep( 5 ); // mZoomControl->zoomLevelPageStep()?

        const int min = 0; //mZoomControl->minimumZoomLevel();
        const int max = 99; //mZoomControl->maximumZoomLevel();
        mSlider->setRange( min, max );

        onZoomLevelChange( mZoomControl->zoomLevel() );
        const int sliderValue = mSlider->value();
        mZoomOutButton->setEnabled( sliderValue > mSlider->minimum() );
        mZoomInButton->setEnabled( sliderValue < mSlider->maximum() );
        connect( mModel, SIGNAL(zoomLevelChanged(double)), SLOT(onZoomLevelChange(double)) );
    }
    else
    {
        mZoomOutButton->setEnabled( false );
        mZoomInButton->setEnabled( false );
        // put slider in the middle
        mSlider->setRange( 0, 99 );
        mSlider->setValue( 50 );
    }

    mSlider->setEnabled( hasView );
}

void ZoomSlider::updateToolTip( int sliderValue )
{
    const float zoomLevel = 50.0 / (100-sliderValue);
    const int zoomPercent = static_cast<int>( zoomLevel*100 + 0.5 );
    mSlider->setToolTip( i18nc("@info:tooltip", "Zoom: %1%", zoomPercent) );
// TODO: get the text by a signal toolTipNeeded( int zoomLevel, QString* toolTipText ); ?
}

void ZoomSlider::zoomOut()
{
    const int newValue = mSlider->value() - mSlider->singleStep();
    mSlider->setValue( newValue );
}

void ZoomSlider::zoomIn()
{
    const int newValue = mSlider->value() + mSlider->singleStep();
    mSlider->setValue( newValue );
}


void ZoomSlider::onSliderValueChanged( int sliderValue )
{
    updateToolTip( sliderValue );
    mZoomOutButton->setEnabled( sliderValue > mSlider->minimum() );
    mZoomInButton->setEnabled( sliderValue < mSlider->maximum() );

    if( mZoomControl )
        mZoomControl->setZoomLevel( 50.0 / (100-sliderValue) );
}

// TODO: which signal comes first, valueChanged or sliderMoved?
// ensure correct calculation of zoomLevel, best by model
// but can be timeconsuming?
// use timer to delay resize, so that sliding is not delayed by resizing
void ZoomSlider::onSliderMoved( int sliderValue )
{
Q_UNUSED( sliderValue )

    QPoint toolTipPoint = mSlider->rect().topLeft();
    toolTipPoint.ry() += mSlider->height() / 2;
    toolTipPoint = mSlider->mapToGlobal( toolTipPoint );

    QHelpEvent toolTipEvent( QEvent::ToolTip, QPoint(0, 0), toolTipPoint );
    QApplication::sendEvent( mSlider, &toolTipEvent );
}

void ZoomSlider::onZoomLevelChange( double level )
{
    mZoomLevel = level;
    const int newSliderValue = 100-static_cast<int>( 50.0 / mZoomLevel + 0.5 );
    if( newSliderValue != mSlider->value() )
    {
        disconnect( mSlider, SIGNAL(valueChanged(int)), this, 0 );
        mSlider->setSliderPosition( newSliderValue );
        updateToolTip( mSlider->value() );
        connect( mSlider, SIGNAL(valueChanged(int)), SLOT(onSliderValueChanged(int)) );
    }
}


ZoomSlider::~ZoomSlider()
{
}

}
