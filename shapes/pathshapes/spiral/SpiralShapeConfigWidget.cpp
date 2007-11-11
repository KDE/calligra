/* This file is part of the KDE project
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
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

#include "SpiralShapeConfigWidget.h"
#include "SpiralShapeConfigCommand.h"
#include <klocale.h>

SpiralShapeConfigWidget::SpiralShapeConfigWidget()
{
    widget.setupUi( this );
    layout()->setContentsMargins( 0,0,0,0 );

    widget.spiralType->clear();
    widget.spiralType->addItem( i18n("Curve") );
    widget.spiralType->addItem( i18n("Line") );

    widget.fade->setMinimum( 0.0 );
    widget.fade->setMaximum( 1.0 );

    connect( widget.spiralType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(propertyChanged()));
    connect( widget.fade, SIGNAL(editingFinished()), this, SIGNAL(propertyChanged()));
}

void SpiralShapeConfigWidget::open(KoShape *shape)
{
    m_spiral = dynamic_cast<KoSpiralShape*>( shape );
    if( ! m_spiral )
        return;

    widget.spiralType->blockSignals( true );
    widget.fade->blockSignals( true );

    widget.spiralType->setCurrentIndex( m_spiral->type() );
    widget.fade->setValue( m_spiral->fade() );

    widget.spiralType->blockSignals( false );
    widget.fade->blockSignals( false );
}

void SpiralShapeConfigWidget::save()
{
    if( ! m_spiral )
        return;

    m_spiral->setType( static_cast<KoSpiralShape::KoSpiralType>( widget.spiralType->currentIndex() ) );
    m_spiral->setFade( widget.fade->value() );
}

QUndoCommand * SpiralShapeConfigWidget::createCommand()
{
    if( ! m_spiral )
        return 0;
    else
    {
        KoSpiralShape::KoSpiralType type = static_cast<KoSpiralShape::KoSpiralType>( widget.spiralType->currentIndex() );
        return new SpiralShapeConfigCommand( m_spiral, type, widget.fade->value() );
    }
}

#include "SpiralShapeConfigWidget.moc"
