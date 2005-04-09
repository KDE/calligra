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

#include "rectproperty.h"

#include <knuminput.h>

#include "rectpropertyui.h"
#include "rectpreview.h"

RectProperty::RectProperty( QWidget *parent, const char *name, RectValueCmd::RectValues &rectValue )
: QWidget( parent, name )
, m_rectValue( rectValue )
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( m_ui = new RectPropertyUI( this ) );

    connect( m_ui->xRndInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotRndChanged() ) );
    connect( m_ui->yRndInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotRndChanged() ) );

    slotReset();
}


RectProperty::~RectProperty()
{
}


int RectProperty::getRectPropertyChange() const
{
    int flags = 0;

    if ( getXRnd() != m_rectValue.xRnd )
        flags |= RectValueCmd::XRnd;

    if ( getYRnd() != m_rectValue.yRnd )
        flags |= RectValueCmd::YRnd;

    return flags;
}


RectValueCmd::RectValues RectProperty::getRectValues() const
{
    RectValueCmd::RectValues rectValue;
    rectValue.xRnd = getXRnd();
    rectValue.yRnd = getYRnd();

    return rectValue;
}


void RectProperty::setRectValues( const RectValueCmd::RectValues &rectValues )
{
    m_rectValue = rectValues;
    slotReset();
}


void RectProperty::apply()
{
    int flags = getRectPropertyChange();

    if ( flags & RectValueCmd::XRnd )
        m_rectValue.xRnd = getXRnd();

    if ( flags & RectValueCmd::YRnd )
        m_rectValue.yRnd = getYRnd();
}


int RectProperty::getXRnd() const
{
    return m_ui->xRndInput->value();
}


int RectProperty::getYRnd() const
{
    return m_ui->yRndInput->value();
}


void RectProperty::slotRndChanged()
{
    m_ui->rectPreview->setRnds( getXRnd(), getYRnd() );
}


void RectProperty::slotReset()
{
    m_ui->xRndInput->setValue( m_rectValue.xRnd );
    m_ui->yRndInput->setValue( m_rectValue.yRnd );

    m_ui->rectPreview->setRnds( getXRnd(), getYRnd() );
}


#include "rectproperty.moc"
