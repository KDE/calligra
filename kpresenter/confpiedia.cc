// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "confpiedia.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <knuminput.h>
#include "kprcommand.h"

#include <qpen.h>
#include <qbrush.h>
#include <qcombobox.h>
#include "global.h"
#include "piepreview.h"

ConfPieDia::ConfPieDia( QWidget* parent, const char* name )
    : QWidget( parent, name ), m_bTypeChanged(false), m_bAngleChanged(false), m_bLengthChanged(false)
{
    QGridLayout *grid = new QGridLayout( this, 7, 2, KDialog::marginHint(), KDialog::spacingHint() );

    lType = new QLabel( i18n( "Type:" ), this );
    grid->addWidget(lType, 0, 0);

    cType = new QComboBox( false, this );
    cType->insertItem( i18n( "Pie" ) );
    cType->insertItem( i18n( "Arc" ) );
    cType->insertItem( i18n( "Chord" ) );
    grid->addWidget(cType, 1, 0);

    connect( cType, SIGNAL( activated( int ) ), this, SLOT( typeChanged( int ) ) );

    lAngle = new QLabel( i18n( "Angle:" ), this );
    grid->addWidget(lAngle, 2, 0);

    eAngle = new KIntNumInput( this );
    eAngle->setRange(0, 360);
    eAngle->setSuffix(" °");
    grid->addWidget(eAngle, 3, 0);

    connect( eAngle, SIGNAL( valueChanged( int ) ), this, SLOT( angleChanged( int ) ) );

    lLen = new QLabel( i18n( "Length:" ), this );
    grid->addWidget(lLen, 4, 0);

    eLen = new KIntNumInput( this );
    eLen->setRange(0, 360);
    eLen->setSuffix(" °");
    grid->addWidget(eLen, 5, 0);

    connect( eLen, SIGNAL( valueChanged( int ) ), this, SLOT( lengthChanged( int ) ) );

    QSpacerItem* spacer = new QSpacerItem( 10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
    grid->addItem( spacer, 6, 0 );

    // ------------------------ preview
    piePreview = new PiePreview( this, "preview" );
    grid->addMultiCellWidget( piePreview, 0, 6, 1, 1 );

    slotReset();
}

ConfPieDia::~ConfPieDia()
{
    delete piePreview;
}

void ConfPieDia::lengthChanged( int _len )
{
    m_bLengthChanged = true;
    len = _len*16;
    piePreview->setLength( len );
}

void ConfPieDia::angleChanged( int _angle )
{
    m_bAngleChanged = true;
    angle = _angle*16;
    piePreview->setAngle( angle );
}

void ConfPieDia::typeChanged( int _type )
{
    m_bTypeChanged = true;
    type = static_cast<PieType>( _type );
    piePreview->setType( type );
}

void ConfPieDia::slotReset()
{
    eAngle->setValue( oldAngle/16 );
    eLen->setValue( oldLen/16 );
    cType->setCurrentItem( oldType );
    type = static_cast<PieType>( oldType );
    piePreview->setLength( oldLen );
    piePreview->setAngle( oldAngle );
    piePreview->setLength( oldLen );
    piePreview->setType( oldType );
    resetConfigChangedValues();
}

void ConfPieDia::setAngle( int _angle )
{
    angle = _angle;
    oldAngle = _angle;
    eAngle->setValue( angle/16 );
    piePreview->setAngle( angle );
}

void ConfPieDia::setLength( int _len )
{
    len = _len;
    oldLen= _len;
    eLen->setValue( len/16 );
    piePreview->setLength( len );
}

void ConfPieDia::setType( PieType _type )
{
    type = _type;
    oldType = _type;
    cType->setCurrentItem( _type );
    piePreview->setType( type );
}

void ConfPieDia::setPenBrush( const QPen &_pen, const QBrush &_brush )
{
    piePreview->setPenBrush( _pen, _brush );
}

void ConfPieDia::resetConfigChangedValues()
{
    m_bTypeChanged = false;
    m_bAngleChanged = false;
    m_bLengthChanged = false;
}

int ConfPieDia::getPieConfigChange() const
{
    int flags = 0;
    if (m_bTypeChanged)
        flags = flags | PieValueCmd::Type;
    if (m_bAngleChanged)
        flags = flags | PieValueCmd::Angle;
    if (m_bLengthChanged)
        flags = flags | PieValueCmd::Length;

    return flags;
}

#include "confpiedia.moc"
