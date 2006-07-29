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

#include "KPrPictureProperty.h"

#include "picturepropertyui.h"
#include "KPrPicturePreview.h"

#include <knuminput.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlayout.h>

KPrPictureProperty::KPrPictureProperty( QWidget *parent, const char *name, const QPixmap &pixmap,
                                  KPrPictureSettingCmd::PictureSettings pictureSettings )
: QWidget( parent, name )
, m_pictureSettings( pictureSettings )
{
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( m_ui = new PicturePropertyUI( this ) );

    connect( m_ui->depth0, SIGNAL( clicked() ), m_ui->picturePreview, SLOT( slotPictureDepth0() ) );
    connect( m_ui->depth1, SIGNAL( clicked() ), m_ui->picturePreview, SLOT( slotPictureDepth1() ) );
    connect( m_ui->depth8, SIGNAL( clicked() ), m_ui->picturePreview, SLOT( slotPictureDepth8() ) );
    connect( m_ui->depth16, SIGNAL( clicked() ), m_ui->picturePreview, SLOT( slotPictureDepth16() ) );
    connect( m_ui->depth32, SIGNAL( clicked() ), m_ui->picturePreview, SLOT( slotPictureDepth32() ) );

    connect( m_ui->swapRGB, SIGNAL( toggled( bool ) ), m_ui->picturePreview, SLOT( slotSwapRGBPicture( bool ) ) );

    connect( m_ui->grayscale, SIGNAL( toggled( bool ) ), m_ui->picturePreview, SLOT( slotGrayscalPicture( bool ) ) );

    connect( m_ui->brightnessInput, SIGNAL( valueChanged( int ) ), m_ui->picturePreview, SLOT( slotBrightValue( int ) ) );

    m_ui->picturePreview->setPicturePixmap( pixmap );

    slotReset();
}


KPrPictureProperty::~KPrPictureProperty()
{
}


int KPrPictureProperty::getPicturePropertyChange() const
{
    int flags = 0;

    KPrPictureSettingCmd::PictureSettings pictureSettings = getPictureSettings();

    if ( pictureSettings.depth != m_pictureSettings.depth )
        flags |= KPrPictureSettingCmd::Depth;

    if ( pictureSettings.swapRGB != m_pictureSettings.swapRGB )
        flags |= KPrPictureSettingCmd::SwapRGB;

    if ( pictureSettings.grayscal != m_pictureSettings.grayscal )
        flags |= KPrPictureSettingCmd::Grayscal;

    if ( pictureSettings.bright != m_pictureSettings.bright )
        flags |= KPrPictureSettingCmd::Bright;

    return flags;
}


KPrPictureSettingCmd::PictureSettings KPrPictureProperty::getPictureSettings() const
{
    KPrPictureSettingCmd::PictureSettings pictureSettings;
    pictureSettings.mirrorType = m_pictureSettings.mirrorType;
    pictureSettings.depth = m_ui->picturePreview->getDepth();
    pictureSettings.swapRGB = m_ui->swapRGB->isOn();
    pictureSettings.grayscal = m_ui->grayscale->isOn();
    pictureSettings.bright = m_ui->brightnessInput->value();
    return pictureSettings;
}


void KPrPictureProperty::apply()
{
    int flags = getPicturePropertyChange();

    KPrPictureSettingCmd::PictureSettings pictureSettings = getPictureSettings();

    if ( flags & KPrPictureSettingCmd::Depth )
        m_pictureSettings.depth = pictureSettings.depth;

    if ( flags & KPrPictureSettingCmd::SwapRGB )
        m_pictureSettings.swapRGB = pictureSettings.swapRGB;

    if ( flags & KPrPictureSettingCmd::Grayscal )
        m_pictureSettings.grayscal = pictureSettings.grayscal;

    if ( flags & KPrPictureSettingCmd::Bright )
        m_pictureSettings.bright = pictureSettings.bright;
}


void KPrPictureProperty::slotReset()
{
    m_ui->depth0->setChecked( m_pictureSettings.depth == 0 );
    m_ui->depth1->setChecked( m_pictureSettings.depth == 1 );
    m_ui->depth8->setChecked( m_pictureSettings.depth == 8 );
    m_ui->depth16->setChecked( m_pictureSettings.depth == 16 );
    m_ui->depth32->setChecked( m_pictureSettings.depth == 32 );
    m_ui->picturePreview->setDepth( m_pictureSettings.depth );

    m_ui->swapRGB->setChecked( m_pictureSettings.swapRGB );
    m_ui->grayscale->setChecked( m_pictureSettings.grayscal );

    m_ui->brightnessInput->setValue( m_pictureSettings.bright );
}


#include "KPrPictureProperty.moc"
