// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "pieproperty.h"

#include <qlayout.h>

#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>

#include "global.h"
#include "piepropertyui.h"
#include "piepreview.h"


PieProperty::PieProperty( QWidget *parent, const char *name, PieValueCmd::PieValues pieValues )
: QWidget( parent, name )
, m_pieValues( pieValues )
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( m_ui = new PiePropertyUI( this ) );

    m_ui->typeCombo->insertItem( i18n( "Pie" ) );
    m_ui->typeCombo->insertItem( i18n( "Arc" ) );
    m_ui->typeCombo->insertItem( i18n( "Chord" ) );

    connect( m_ui->typeCombo, SIGNAL( activated( int ) ), this, SLOT( slotTypeChanged( int ) ) );

    connect( m_ui->angleInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotAngleChanged( int ) ) );
    connect( m_ui->lengthInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotLengthChanged( int ) ) );

    slotReset();
}


PieProperty::~PieProperty()
{
}


int PieProperty::getPiePropertyChange() const
{
    int flags = 0;

    PieValueCmd::PieValues pieValues = getPieValues();

    if ( pieValues.pieType != m_pieValues.pieType )
        flags |= PieValueCmd::Type;

    if ( pieValues.pieAngle != m_pieValues.pieAngle )
        flags |= PieValueCmd::Angle;

    if ( pieValues.pieLength != m_pieValues.pieLength )
        flags |= PieValueCmd::Length;

    return flags;
}


PieValueCmd::PieValues PieProperty::getPieValues() const
{
    PieValueCmd::PieValues pieValues;
    pieValues.pieType = static_cast<PieType>( m_ui->typeCombo->currentItem() );
    pieValues.pieAngle = m_ui->angleInput->value() * 16;
    pieValues.pieLength = m_ui->lengthInput->value() * 16;
    return pieValues;
}


void PieProperty::setPieValues( const PieValueCmd::PieValues &pieValues )
{
    m_pieValues = pieValues;
    slotReset();
}


void PieProperty::apply()
{
    int flags = getPiePropertyChange();

    PieValueCmd::PieValues pieValues = getPieValues();

    if ( flags & PieValueCmd::Type )
        m_pieValues.pieType = pieValues.pieType;

    if ( flags & PieValueCmd::Angle )
        m_pieValues.pieAngle = pieValues.pieAngle;

    if ( flags & PieValueCmd::Length )
        m_pieValues.pieLength = pieValues.pieLength;
}


void PieProperty::slotReset()
{
    m_ui->typeCombo->setCurrentItem( ( int ) m_pieValues.pieType );
    m_ui->piePreview->setType( m_pieValues.pieType );
    m_ui->angleInput->setValue( m_pieValues.pieAngle / 16 );
    m_ui->piePreview->setAngle( m_pieValues.pieAngle );
    m_ui->lengthInput->setValue( m_pieValues.pieLength / 16 );
    m_ui->piePreview->setLength( m_pieValues.pieLength );
}


void PieProperty::slotTypeChanged( int pos )
{
    m_ui->piePreview->setType( static_cast<PieType>( pos ) );
}


void PieProperty::slotAngleChanged( int num )
{
    m_ui->piePreview->setAngle( num * 16 );
}


void PieProperty::slotLengthChanged( int num )
{
    m_ui->piePreview->setLength( num * 16 );
}


#include "pieproperty.moc"
