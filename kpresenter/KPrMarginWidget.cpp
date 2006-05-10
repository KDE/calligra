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

#include "KPrMarginWidget.h"

#include <qgroupbox.h>
#include <QCheckBox>
#include <QLayout>

#include <klocale.h>
#include <knuminput.h>

#include "marginui.h"
#include <KoUnitWidgets.h>//for KoUnitDoubleSpinBox

KPrMarginWidget::KPrMarginWidget( QWidget *parent, const char *name, const KoUnit::Unit unit )
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

    m_ui->margins->setTitle( i18n( "Margins" ) );

    double dStep = KoUnit::fromUserValue( 0.5, unit );
    double dMax = KoUnit::fromUserValue( 9999, unit );
    m_ui->leftInput->setUnit( unit );
    m_ui->leftInput->setMinMaxStep( 0, dMax, dStep );

    m_ui->rightInput->setUnit( unit );
    m_ui->rightInput->setMinMaxStep( 0, dMax, dStep );

    m_ui->topInput->setUnit( unit );
    m_ui->topInput->setMinMaxStep( 0, dMax, dStep );

    m_ui->bottomInput->setUnit( unit );
    m_ui->bottomInput->setMinMaxStep( 0, dMax, dStep );

    connect( m_ui->leftInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
    connect( m_ui->rightInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
    connect( m_ui->topInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
    connect( m_ui->bottomInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotValueChanged( double ) ) );
}


KPrMarginWidget::~KPrMarginWidget()
{
}


void KPrMarginWidget::setValues( double left, double right, double top, double bottom )
{
    m_ui->leftInput->changeValue( left );
    m_ui->rightInput->changeValue( right );
    m_ui->topInput->changeValue( top );
    m_ui->bottomInput->changeValue( bottom );
}


double KPrMarginWidget::leftValue() const
{
    return m_ui->leftInput->value();
}


double KPrMarginWidget::rightValue() const
{
    return m_ui->rightInput->value();
}


double KPrMarginWidget::topValue() const
{
    return m_ui->topInput->value();
}


double KPrMarginWidget::bottomValue() const
{
    return m_ui->bottomInput->value();
}


void KPrMarginWidget::slotValueChanged( double val )
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


#include "KPrMarginWidget.moc"
