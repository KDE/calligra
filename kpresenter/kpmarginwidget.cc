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

#include "kpmarginwidget.h"

#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlayout.h>

#include <klocale.h>
#include <knuminput.h>

#include "marginui.h"


KPMarginWidget::KPMarginWidget( QWidget *parent, const char *name, const KoUnit::Unit unit )
: QWidget( parent, name )
, m_unit( unit )
, m_changed( false )
, m_noSignal( false )
{
    QVBoxLayout *layout = new QVBoxLayout( this );

    m_ui = new MarginUI( this );
    layout->addWidget( m_ui );

    QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    layout->addItem( spacer );

    m_ui->margins->setTitle( i18n( "Margins (%1)" ).arg( KoUnit::unitName( m_unit ) ) );

    m_ui->leftInput->setRange(  0, 9999, 0.5, false );
    m_ui->rightInput->setRange(  0, 9999, 0.5, false );
    m_ui->topInput->setRange(  0, 9999, 0.5, false );
    m_ui->bottomInput->setRange(  0, 9999, 0.5, false );
    connect( m_ui->leftInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
    connect( m_ui->rightInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
    connect( m_ui->topInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
    connect( m_ui->bottomInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
}


KPMarginWidget::~KPMarginWidget()
{
}


void KPMarginWidget::setValues( double left, double right, double top, double bottom )
{
    m_ui->leftInput->setValue( KoUnit::toUserValue( left, m_unit ) );
    m_ui->rightInput->setValue( KoUnit::toUserValue( right, m_unit ) );
    m_ui->topInput->setValue( KoUnit::toUserValue( top, m_unit ) );
    m_ui->bottomInput->setValue( KoUnit::toUserValue( bottom, m_unit ) );
}


double KPMarginWidget::leftValue() const
{
    return KoUnit::fromUserValue( m_ui->leftInput->value(), m_unit );
}


double KPMarginWidget::rightValue() const
{
    return KoUnit::fromUserValue( m_ui->rightInput->value(), m_unit );
}


double KPMarginWidget::topValue() const
{
    return KoUnit::fromUserValue( m_ui->topInput->value(), m_unit );
}


double KPMarginWidget::bottomValue() const
{
    return KoUnit::fromUserValue( m_ui->bottomInput->value(), m_unit );
}


void KPMarginWidget::slotValueChanged( double val )
{
    m_changed = true;
    if ( m_ui->synchronize->isChecked() && !m_noSignal )
    {
        m_noSignal = true;
        m_ui->leftInput->setValue( val );
        m_ui->bottomInput->setValue( val );
        m_ui->rightInput->setValue( val );
        m_ui->topInput->setValue( val );
        m_noSignal = false;
    }
}


#include "kpmarginwidget.moc"
