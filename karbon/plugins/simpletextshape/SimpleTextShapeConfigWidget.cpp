/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "SimpleTextShapeConfigWidget.h"
#include "SimpleTextShape.h"

SimpleTextShapeConfigWidget::SimpleTextShapeConfigWidget()
    : m_shape(0)
{
    widget.setupUi( this );

    widget.bold->setCheckable( true );
    widget.bold->setIcon( KIcon( "format-text-bold" ) );
    widget.italic->setCheckable( true );
    widget.italic->setIcon( KIcon( "format-text-italic" ) );
    widget.fontSize->setRange( 2, 72 );

    connect( widget.fontFamily, SIGNAL(currentFontChanged(const QFont&)), this, SIGNAL(propertyChanged()));
    connect( widget.fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(propertyChanged()));
    connect( widget.bold, SIGNAL(toggled(bool)), this, SIGNAL(propertyChanged()));
    connect( widget.italic, SIGNAL(toggled(bool)), this, SIGNAL(propertyChanged()));
    connect( widget.text, SIGNAL(textChanged(const QString&)), this, SIGNAL(propertyChanged()));
    connect( widget.startOffset, SIGNAL(valueChanged(int)), this, SIGNAL(propertyChanged()));
}

void SimpleTextShapeConfigWidget::blockChildSignals( bool block )
{
    widget.fontFamily->blockSignals( block );
    widget.fontSize->blockSignals( block );
    widget.text->blockSignals( block );
    widget.bold->blockSignals( block );
    widget.italic->blockSignals( block );
    widget.startOffset->blockSignals( block );
}

void SimpleTextShapeConfigWidget::open(KoShape *shape)
{
    m_shape = dynamic_cast<SimpleTextShape*>( shape );
    if( ! m_shape )
        return;

    blockChildSignals( true );

    QFont font = m_shape->font();

    widget.text->setText( m_shape->text() );
    widget.fontSize->setValue( font.pointSize() );
    font.setPointSize( 8 );

    widget.fontFamily->setFont( font );
    widget.bold->setChecked( font.bold() );
    widget.italic->setChecked( font.italic() );
    widget.startOffset->setValue( static_cast<int>( m_shape->startOffset() * 100.0 ) );
    widget.startOffset->setVisible( m_shape->isAttached() );
    widget.labelStartOffset->setVisible( m_shape->isAttached() );

    blockChildSignals( false );
}

void SimpleTextShapeConfigWidget::save()
{
    if( ! m_shape )
        return;

    QFont font = widget.fontFamily->currentFont();
    font.setBold( widget.bold->isChecked() );
    font.setItalic( widget.italic->isChecked() );
    font.setPointSize( widget.fontSize->value() );

    m_shape->setFont( font );
    m_shape->setText( widget.text->text() );
    m_shape->setStartOffset( static_cast<qreal>(widget.startOffset->value()) / 100.0 );
}

QUndoCommand * SimpleTextShapeConfigWidget::createCommand()
{
    save();

    return 0;
}
