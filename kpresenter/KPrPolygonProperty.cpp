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

#include "KPrPolygonProperty.h"

#include <QLayout>

#include <kcombobox.h>
#include <klocale.h>
#include <knuminput.h>

#include "polygonpropertyui.h"
#include "KPrPolygonPreview.h"

KPrPolygonProperty::KPrPolygonProperty( QWidget *parent, const char *name, KPrPolygonSettingCmd::PolygonSettings &polygonSettings )
: QWidget( parent, name )
, m_polygonSettings( polygonSettings )
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( m_ui = new PolygonPropertyUI( this ) );

    m_ui->typeCombo->insertItem( i18n( "Polygon" ) );
    m_ui->typeCombo->insertItem( i18n( "Convex/Concave" ) );

    connect( m_ui->typeCombo, SIGNAL( activated( int ) ),
             this, SLOT(slotTypeChanged( int ) ) );

    connect( m_ui->cornersInput, SIGNAL( valueChanged( int ) ),
             m_ui->polygonPreview, SLOT( slotCornersValue( int ) ) );
    connect( m_ui->sharpnessInput, SIGNAL( valueChanged( int ) ),
             m_ui->polygonPreview, SLOT( slotSharpnessValue( int ) ) );

    slotReset();
}


KPrPolygonProperty::~KPrPolygonProperty()
{
}


int KPrPolygonProperty::getPolygonPropertyChange() const
{
    int flags = 0;

    if ( isConvexConcave() != m_polygonSettings.checkConcavePolygon )
        flags |= KPrPolygonSettingCmd::ConcaveConvex;

    if ( m_ui->cornersInput->value() != m_polygonSettings.cornersValue )
        flags |= KPrPolygonSettingCmd::Corners;

    if ( m_ui->sharpnessInput->value() != m_polygonSettings.sharpnessValue )
        flags |= KPrPolygonSettingCmd::Sharpness;

    return flags;
}


KPrPolygonSettingCmd::PolygonSettings KPrPolygonProperty::getPolygonSettings() const
{
    KPrPolygonSettingCmd::PolygonSettings polygonSettings;
    polygonSettings.checkConcavePolygon = isConvexConcave();
    polygonSettings.cornersValue = m_ui->cornersInput->value();;
    polygonSettings.sharpnessValue = m_ui->sharpnessInput->value();;
    return polygonSettings;
}


void KPrPolygonProperty::setPolygonSettings( const KPrPolygonSettingCmd::PolygonSettings &polygonSettings )
{
    m_polygonSettings = polygonSettings;
    slotReset();
}


void KPrPolygonProperty::apply()
{
    int flags = getPolygonPropertyChange();

    if ( flags & KPrPolygonSettingCmd::ConcaveConvex )
        m_polygonSettings.checkConcavePolygon = isConvexConcave();

    if ( flags & KPrPolygonSettingCmd::Corners )
        m_polygonSettings.cornersValue = m_ui->cornersInput->value();

    if ( flags & KPrPolygonSettingCmd::Sharpness )
        m_polygonSettings.sharpnessValue = m_ui->sharpnessInput->value();
}


bool KPrPolygonProperty::isConvexConcave() const
{
    return m_ui->typeCombo->currentItem() == 1;
}


void KPrPolygonProperty::slotTypeChanged( int pos )
{
    m_ui->polygonPreview->slotConvexConcave( pos == 1 );

    m_ui->sharpnessInput->setEnabled( pos == 1 );
}


void KPrPolygonProperty::slotReset()
{
    m_ui->typeCombo->setCurrentItem( m_polygonSettings.checkConcavePolygon ? 1 : 0 );
    m_ui->polygonPreview->slotConvexConcave( m_polygonSettings.checkConcavePolygon );
    m_ui->sharpnessInput->setEnabled( m_polygonSettings.checkConcavePolygon );
    m_ui->cornersInput->setValue( m_polygonSettings.cornersValue );
    m_ui->polygonPreview->slotCornersValue( m_polygonSettings.cornersValue );
    m_ui->sharpnessInput->setValue( m_polygonSettings.sharpnessValue );
    m_ui->polygonPreview->slotSharpnessValue( m_polygonSettings.sharpnessValue );
}


#include "KPrPolygonProperty.moc"
