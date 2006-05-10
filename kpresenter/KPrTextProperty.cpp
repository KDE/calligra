// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   The code is mostly a copy from kword/framedia.cc

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

#include "KPrTextProperty.h"

#include <QCheckBox>
#include <QLayout>

#include <klocale.h>
#include <kdebug.h>

#include "KPrMarginWidget.h"


KPrTextProperty::KPrTextProperty( QWidget *parent, const char *name, const MarginsStruct &marginsStruct,
                            const KoUnit::Unit unit, PropValue protectContent )
: QWidget( parent, name )
, m_unit( unit )
, m_protectContent( protectContent )
{
    QGridLayout *layout = new QGridLayout( this, 1, 1, 11, 6 );

    layout->addWidget( m_protectContentCheck = new QCheckBox( i18n( "Protect content" ), this ), 0, 0 );
    layout->addWidget( m_margins = new KPrMarginWidget( this, name, m_unit ), 1, 0 );

    connect( m_protectContentCheck, SIGNAL( toggled ( bool ) ),
             this, SLOT( slotProtectContentChanged( bool ) ) );

    resize( QSize( 301, 217 ).expandedTo( minimumSizeHint() ) );

    m_margins->setValues( marginsStruct.leftMargin, marginsStruct.rightMargin,
                          marginsStruct.topMargin, marginsStruct.bottomMargin );

    slotReset();
}


KPrTextProperty::~KPrTextProperty()
{
}


int KPrTextProperty::getTextPropertyChange() const
{
    int flags = 0;

    if ( m_protectContentCheck->state() != QCheckBox::NoChange )
    {
        if ( ( m_protectContentCheck->isOn() ? STATE_ON : STATE_OFF ) != m_protectContent )
        {
            flags |= ProtectContent;
        }

        if ( ! m_protectContentCheck->isOn() && m_margins->changed() )
        {
            flags |= Margins;
        }
    }

    return flags;
}


MarginsStruct KPrTextProperty::getMarginsStruct() const
{
    MarginsStruct marginsStruct;
    marginsStruct.leftMargin = m_margins->leftValue();
    marginsStruct.rightMargin = m_margins->rightValue();
    marginsStruct.topMargin = m_margins->topValue();
    marginsStruct.bottomMargin = m_margins->bottomValue();
    return marginsStruct;
}


bool KPrTextProperty::getProtectContent() const
{
    return m_protectContentCheck->isOn();
}


void KPrTextProperty::apply()
{
    int flags = getTextPropertyChange();

    if ( flags & ProtectContent )
        m_protectContent = m_protectContentCheck->isOn() ? STATE_ON : STATE_OFF;

    if ( flags & Margins )
        m_margins->resetChanged();
}


void KPrTextProperty::slotProtectContentChanged( bool b )
{
    m_margins->setEnabled( !b );
}


void KPrTextProperty::slotReset()
{
    switch ( m_protectContent )
    {
        case STATE_ON:
            m_protectContentCheck->setChecked( true );
            break;
        case STATE_OFF:
            m_protectContentCheck->setChecked( false );
            break;
        case STATE_UNDEF:
            m_protectContentCheck->setTristate( true );
            m_protectContentCheck->setNoChange();
            break;
        default:
            m_protectContentCheck->setChecked( false );
            break;
    }
}


#include "KPrTextProperty.moc"
