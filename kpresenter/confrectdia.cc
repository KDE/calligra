// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
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

#include "confrectdia.h"

#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include <knuminput.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include "kprcommand.h"
#include "rectpreview.h"

ConfRectDia::ConfRectDia( QWidget* parent, const char* name )
    : QWidget( parent, name ), m_bRndXChanged(false), m_bRndYChanged(false)
{

    // ------------------------ layout
    QVBoxLayout *layout = new QVBoxLayout( this, 0 );
    layout->setMargin( KDialog::marginHint() );
    layout->setSpacing( KDialog::spacingHint() );

    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( KDialog::spacingHint() );

    QVBoxLayout *left = new QVBoxLayout(hbox);
    left->setSpacing( KDialog::spacingHint() );

    // ------------------------ settings

    lRndX = new QLabel( i18n( "Vertical declination:" ), this );
    left->addWidget( lRndX );
    
    eRndX = new KIntNumInput( this );
    eRndX->setRange(0, 100);
    eRndX->setSuffix(" %");
    left->addWidget( eRndX );
    connect( eRndX, SIGNAL( valueChanged( int ) ), this, SLOT( rndXChanged( int ) ) );

    lRndY = new QLabel( i18n( "Horizontal declination:" ), this );
    left->addWidget( lRndY );
    
    eRndY = new KIntNumInput( this );
    eRndY->setRange(0, 100);
    eRndY->setSuffix(" %");
    left->addWidget( eRndY );
    connect( eRndY, SIGNAL( valueChanged( int ) ), this, SLOT( rndYChanged( int ) ) );

    QSpacerItem *spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    left->addItem(spacer);

    // ------------------------ preview
    rectPreview = new RectPreview( this, "preview" );

    hbox->addWidget( rectPreview );
    slotReset();
}

ConfRectDia::~ConfRectDia()
{
    delete rectPreview;
}

void ConfRectDia::rndXChanged( int _rx )
{
    xRnd = _rx;
    m_bRndXChanged = true;
    rectPreview->setRnds( xRnd, yRnd );
}

void ConfRectDia::rndYChanged( int _ry )
{
    yRnd = _ry;
    m_bRndYChanged = true;
    rectPreview->setRnds( xRnd, yRnd );
}

void ConfRectDia::setRnds( int _rx, int _ry )
{
    xRnd = _rx;
    yRnd = _ry;
    oldXRnd  = _rx;
    oldYRnd = _ry;

    rectPreview->setRnds( xRnd, yRnd );

    eRndX->setValue( xRnd );
    eRndY->setValue( yRnd );
}

void ConfRectDia::slotReset()
{
    rectPreview->setRnds( oldXRnd, oldYRnd );

    eRndX->setValue( oldXRnd );
    eRndY->setValue( oldYRnd );
    resetConfigChangedValues();
}

void ConfRectDia::setPenBrush( const QPen &_pen, const QBrush &_brush )
{
    rectPreview->setPenBrush( _pen, _brush );
}

void ConfRectDia::resetConfigChangedValues()
{
    m_bRndXChanged = false;
    m_bRndYChanged = false;
}

int ConfRectDia::getRectangleConfigChange() const
{
    int flags = 0;
    if (m_bRndXChanged)
        flags = flags | RectValueCmd::XRnd;
    if (m_bRndYChanged)
        flags = flags | RectValueCmd::YRnd;

    return flags;
}

#include "confrectdia.moc"
