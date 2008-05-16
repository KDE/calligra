/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "PngExportOptionsWidget.h"

#include <KoUnit.h> // for POINT_TO_INCH
#include <KoGlobal.h>

PngExportOptionsWidget::PngExportOptionsWidget( QSizeF pointSize, QWidget * parent )
    : QWidget( parent ), m_pointSize( pointSize )
{
    KoUnit unit;

    widget.setupUi(this);

    widget.pxWidth->setRange( 1, 10000 );
    widget.pxWidth->setAlignment( Qt::AlignRight );
    widget.pxWidth->setSuffix( " px" );
    widget.pxHeight->setRange( 1, 10000 );
    widget.pxHeight->setAlignment( Qt::AlignRight );
    widget.pxHeight->setSuffix( " px" );
    widget.unitWidth->setMinMaxStep( 0, 10000, 1 );
    widget.unitHeight->setMinMaxStep( 0, 10000, 1 );
    widget.dpi->setRange( 1, 10000 );
    widget.dpi->setValue( KoGlobal::dpiX() );
    widget.dpi->setAlignment( Qt::AlignRight );
    widget.dpi->setSuffix( " DPI" );
    widget.keepAspect->setCheckState( Qt::Checked );
    widget.unit->addItems( KoUnit::listOfUnitName() );
    widget.unit->setCurrentIndex( unit.indexInList() );

    widget.unitWidth->changeValue( pointSize.width() );
    widget.unitHeight->changeValue( pointSize.height() );
    updateFromPointSize( pointSize );

    connect( widget.unitWidth, SIGNAL(valueChangedPt(double)), this, SLOT(unitWidthChanged(double)));
    connect( widget.unitHeight, SIGNAL(valueChangedPt(double)), this, SLOT(unitHeightChanged(double)));
    connect( widget.pxWidth, SIGNAL(valueChanged(int)), this, SLOT(pxWidthChanged(int)));
    connect( widget.pxHeight, SIGNAL(valueChanged(int)), this, SLOT(pxHeightChanged(int)));
    connect( widget.dpi, SIGNAL(valueChanged(int)), this, SLOT(dpiChanged(int)));
    connect( widget.unit, SIGNAL(activated(int)), this, SLOT(unitChanged(int)));
    connect( widget.keepAspect,SIGNAL(toggled(bool)), this, SLOT(aspectChanged(bool)));
}

void PngExportOptionsWidget::setUnit( const KoUnit &unit )
{
    widget.unitWidth->setUnit( unit );
    widget.unitHeight->setUnit( unit );
    widget.unit->setCurrentIndex( unit.indexInList() );
}

QSize PngExportOptionsWidget::exportSize()
{
    return QSize( widget.pxWidth->value(), widget.pxHeight->value() );
}

void PngExportOptionsWidget::updateFromPointSize( const QSizeF &pointSize )
{
    blockChildSignals( true );
    widget.pxWidth->setValue( qRound( POINT_TO_INCH(pointSize.width()) * widget.dpi->value() ) );
    widget.pxHeight->setValue( qRound( POINT_TO_INCH(pointSize.height()) * widget.dpi->value() ) );
    blockChildSignals( false );
}

void PngExportOptionsWidget::updateFromPixelSize( const QSize &pixelSize )
{
    blockChildSignals( true );
    double inchWidth = static_cast<double>(pixelSize.width()) / static_cast<double>(widget.dpi->value());
    double inchHeight = static_cast<double>(pixelSize.height()) / static_cast<double>(widget.dpi->value());
    widget.unitWidth->changeValue( INCH_TO_POINT( inchWidth ) );
    widget.unitHeight->changeValue( INCH_TO_POINT( inchHeight ) );
    blockChildSignals( false );
}

void PngExportOptionsWidget::blockChildSignals( bool block )
{
    widget.pxWidth->blockSignals( block );
    widget.pxHeight->blockSignals( block );
    widget.unitWidth->blockSignals( block );
    widget.unitHeight->blockSignals( block );
    widget.dpi->blockSignals( block );
}

void PngExportOptionsWidget::unitWidthChanged( double newWidth )
{
    blockChildSignals( true );

    double newHeight = widget.unitHeight->value();
    if( widget.keepAspect->checkState() == Qt::Checked )
    {
        newHeight = newWidth * m_pointSize.height() / m_pointSize.width();
        widget.unitHeight->changeValue( newHeight );
    }
    updateFromPointSize( QSizeF( newWidth, newHeight ) );

    blockChildSignals( false );
}

void PngExportOptionsWidget::unitHeightChanged( double newHeight )
{
    blockChildSignals( true );

    double newWidth = widget.unitWidth->value();
    if( widget.keepAspect->checkState() == Qt::Checked )
    {
        newWidth = newHeight * m_pointSize.width() / m_pointSize.height();
        widget.unitWidth->changeValue( newWidth );
    }
    updateFromPointSize( QSizeF( newWidth, newHeight ) );

    blockChildSignals( false );
}

void PngExportOptionsWidget::pxWidthChanged( int newWidth )
{
    blockChildSignals( true );

    int newHeight = widget.pxHeight->value();
    if( widget.keepAspect->checkState() == Qt::Checked )
    {
        newHeight = qRound( newWidth * m_pointSize.height() / m_pointSize.width() );
        widget.pxHeight->setValue( newHeight );
    }
    updateFromPixelSize( QSize( newWidth, newHeight ) );

    blockChildSignals( false );
}

void PngExportOptionsWidget::pxHeightChanged( int newHeight )
{
    blockChildSignals( true );

    int newWidth = widget.pxWidth->value();
    if( widget.keepAspect->checkState() == Qt::Checked )
    {
        newWidth = qRound( newHeight * m_pointSize.width() / m_pointSize.height() );
        widget.pxWidth->setValue( newWidth );
    }
    updateFromPixelSize( QSize( newWidth, newHeight ) );

    blockChildSignals( false );
}

void PngExportOptionsWidget::dpiChanged( int )
{
    blockChildSignals( true );

    updateFromPointSize( QSizeF( widget.unitWidth->value(), widget.unitHeight->value() ) );

    blockChildSignals( false );
}

void PngExportOptionsWidget::unitChanged( int newUnit )
{
    KoUnit unit((KoUnit::Unit) newUnit );
    blockChildSignals( true );
    widget.unitWidth->setUnit( unit );
    widget.unitHeight->setUnit( unit );
    blockChildSignals( false );
}

void PngExportOptionsWidget::aspectChanged( bool checked )
{
    if( checked )
        unitWidthChanged( widget.unitWidth->value() );
}
#include "PngExportOptionsWidget.moc"
