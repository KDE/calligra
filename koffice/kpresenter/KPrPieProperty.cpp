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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPieProperty.h"

#include <qlayout.h>

#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>

#include "global.h"
#include "piepropertyui.h"
#include "KPrPiePreview.h"


KPrPieProperty::KPrPieProperty( QWidget *parent, const char *name, KPrPieValueCmd::PieValues pieValues )
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


KPrPieProperty::~KPrPieProperty()
{
}


int KPrPieProperty::getPiePropertyChange() const
{
    int flags = 0;

    KPrPieValueCmd::PieValues pieValues = getPieValues();

    if ( pieValues.pieType != m_pieValues.pieType )
        flags |= KPrPieValueCmd::Type;

    if ( pieValues.pieAngle != m_pieValues.pieAngle )
        flags |= KPrPieValueCmd::Angle;

    if ( pieValues.pieLength != m_pieValues.pieLength )
        flags |= KPrPieValueCmd::Length;

    return flags;
}


KPrPieValueCmd::PieValues KPrPieProperty::getPieValues() const
{
    KPrPieValueCmd::PieValues pieValues;
    pieValues.pieType = static_cast<PieType>( m_ui->typeCombo->currentItem() );
    pieValues.pieAngle = m_ui->angleInput->value() * 16;
    pieValues.pieLength = m_ui->lengthInput->value() * 16;
    return pieValues;
}


void KPrPieProperty::setPieValues( const KPrPieValueCmd::PieValues &pieValues )
{
    m_pieValues = pieValues;
    slotReset();
}


void KPrPieProperty::apply()
{
    int flags = getPiePropertyChange();

    KPrPieValueCmd::PieValues pieValues = getPieValues();

    if ( flags & KPrPieValueCmd::Type )
        m_pieValues.pieType = pieValues.pieType;

    if ( flags & KPrPieValueCmd::Angle )
        m_pieValues.pieAngle = pieValues.pieAngle;

    if ( flags & KPrPieValueCmd::Length )
        m_pieValues.pieLength = pieValues.pieLength;
}


void KPrPieProperty::slotReset()
{
    m_ui->typeCombo->setCurrentItem( ( int ) m_pieValues.pieType );
    m_ui->piePreview->setType( m_pieValues.pieType );
    m_ui->angleInput->setValue( m_pieValues.pieAngle / 16 );
    m_ui->piePreview->setAngle( m_pieValues.pieAngle );
    m_ui->lengthInput->setValue( m_pieValues.pieLength / 16 );
    m_ui->piePreview->setLength( m_pieValues.pieLength );
}


void KPrPieProperty::slotTypeChanged( int pos )
{
    m_ui->piePreview->setType( static_cast<PieType>( pos ) );
}


void KPrPieProperty::slotAngleChanged( int num )
{
    m_ui->piePreview->setAngle( num * 16 );
}


void KPrPieProperty::slotLengthChanged( int num )
{
    m_ui->piePreview->setLength( num * 16 );
}


#include "KPrPieProperty.moc"
