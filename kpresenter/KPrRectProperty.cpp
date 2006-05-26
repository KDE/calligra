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

#include "KPrRectProperty.h"

#include "rectpropertyui.h"
#include "KPrRectPreview.h"

#include <KoImageResource.h>

#include <QSpinBox>
#include <qtoolbutton.h>
#include <QLayout>
//Added by qt3to4:
#include <Q3VBoxLayout>

KPrRectProperty::KPrRectProperty( QWidget *parent, const char *name, KPrRectValueCmd::RectValues &rectValue )
: QWidget( parent, name )
, m_rectValue( rectValue )
{
    formerVerticalValue = 0;
    Q3VBoxLayout *layout = new Q3VBoxLayout( this );
    layout->addWidget( m_ui = new RectPropertyUI( this ) );
    KoImageResource kir;
    m_ui->combineButton->setPixmap(kir.chain());

    connect( m_ui->xRndInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotRndChanged() ) );
    connect( m_ui->yRndInput, SIGNAL( valueChanged( int ) ), this, SLOT( slotRndChanged() ) );
    connect( m_ui->combineButton, SIGNAL( toggled( bool ) ), this, SLOT( combineToggled( bool ) ) );

    slotReset();
}

KPrRectProperty::~KPrRectProperty()
{
}


int KPrRectProperty::getRectPropertyChange() const
{
    int flags = 0;

    if ( getXRnd() != m_rectValue.xRnd )
        flags |= KPrRectValueCmd::XRnd;

    if ( getYRnd() != m_rectValue.yRnd )
        flags |= KPrRectValueCmd::YRnd;

    return flags;
}


KPrRectValueCmd::RectValues KPrRectProperty::getRectValues() const
{
    KPrRectValueCmd::RectValues rectValue;
    rectValue.xRnd = getXRnd();
    rectValue.yRnd = getYRnd();

    return rectValue;
}


void KPrRectProperty::setRectValues( const KPrRectValueCmd::RectValues &rectValues )
{
    m_rectValue = rectValues;
    slotReset();
}


void KPrRectProperty::apply()
{
    int flags = getRectPropertyChange();

    if ( flags & KPrRectValueCmd::XRnd )
        m_rectValue.xRnd = getXRnd();

    if ( flags & KPrRectValueCmd::YRnd )
        m_rectValue.yRnd = getYRnd();
}


int KPrRectProperty::getXRnd() const
{
    return m_ui->xRndInput->value();
}


int KPrRectProperty::getYRnd() const
{
    return m_ui->yRndInput->value();
}


void KPrRectProperty::slotRndChanged()
{
    m_ui->rectPreview->setRnds( getXRnd(), getYRnd() );
}

void KPrRectProperty::slotReset()
{
    m_ui->xRndInput->setValue( m_rectValue.xRnd );
    m_ui->yRndInput->setValue( m_rectValue.yRnd );
    if(m_rectValue.xRnd == m_rectValue.yRnd)
        combineToggled(true);

    m_ui->rectPreview->setRnds( getXRnd(), getYRnd() );
}

void KPrRectProperty::combineToggled( bool on)
{
    KoImageResource kir;
    if( on ) {
        formerVerticalValue = getYRnd();
        m_ui->yRndInput->setValue( getXRnd() );
        connect(m_ui->yRndInput, SIGNAL( valueChanged ( int ) ),
                m_ui->xRndInput, SLOT( setValue ( int ) ));
        connect(m_ui->xRndInput, SIGNAL( valueChanged ( int ) ),
                m_ui->yRndInput, SLOT( setValue ( int ) ));
        m_ui->combineButton->setPixmap(kir.chain());
    }
    else {
        disconnect(m_ui->yRndInput, SIGNAL( valueChanged ( int ) ),
                m_ui->xRndInput, SLOT( setValue ( int ) ));
        disconnect(m_ui->xRndInput, SIGNAL( valueChanged ( int ) ),
                m_ui->yRndInput, SLOT( setValue ( int ) ));
        if(formerVerticalValue != 0)
            m_ui->yRndInput->setValue( formerVerticalValue );
        m_ui->combineButton->setPixmap(kir.chainBroken());
    }
}

#include "KPrRectProperty.moc"
