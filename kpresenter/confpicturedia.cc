// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project

Copyright (C) 2002 Toshitaka Fujioka <fujioka@kde.org>

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

#include "confpicturedia.h"

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>

#include <knuminput.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kdialog.h>

#include "picturepreview.h"

ConfPictureDia::ConfPictureDia( QWidget *parent, const char *name)
    : QWidget( parent, name )
{
    // ------------------------ layout
    QVBoxLayout *layout = new QVBoxLayout( this, 0 );
    layout->setMargin( KDialog::marginHint() );
    layout->setSpacing( KDialog::spacingHint() );

    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( KDialog::spacingHint() );

    // ------------------------ settings
    gSettings = new QGroupBox( 1, Qt::Horizontal, i18n( "Settings" ), this );

    QButtonGroup *mirrorGroup = new QVButtonGroup( i18n( "Mirror" ), gSettings );

    m_normalPicture = new QRadioButton( i18n( "Normal" ), mirrorGroup );
    connect( m_normalPicture, SIGNAL( clicked() ), this, SLOT( slotNormalPicture() ) );

    m_horizontalMirrorPicture = new QRadioButton( i18n( "Horizontal mirror" ), mirrorGroup );
    connect( m_horizontalMirrorPicture, SIGNAL( clicked() ), this, SLOT( slotHorizontalMirrorPicture() ) );

    m_verticalMirrorPicture = new QRadioButton( i18n( "Vertical mirror" ), mirrorGroup );
    connect( m_verticalMirrorPicture, SIGNAL( clicked() ), this, SLOT( slotVerticalMirrorPicture() ) );

    m_horizontalAndVerticalMirrorPicture = new QRadioButton( i18n( "Horizontal and vertical mirror" ), mirrorGroup );
    connect( m_horizontalAndVerticalMirrorPicture, SIGNAL( clicked() ), this, SLOT( slotHorizontalAndVerticalMirrorPicture() ) );

    QButtonGroup *depthGroup = new QVButtonGroup( i18n( "Depth" ), gSettings );

    m_depth0 = new QRadioButton( i18n( "Default color mode" ), depthGroup );
    connect( m_depth0, SIGNAL( clicked() ), this, SLOT( slotPictureDepth0() ) );

    m_depth1 = new QRadioButton( i18n( "1 bit color mode" ), depthGroup );
    connect( m_depth1, SIGNAL( clicked() ), this, SLOT( slotPictureDepth1() ) );

    m_depth8 = new QRadioButton( i18n( "8 bit color mode" ), depthGroup );
    connect( m_depth8, SIGNAL( clicked() ), this, SLOT( slotPictureDepth8() ) );

    m_depth16 = new QRadioButton( i18n( "16 bit color mode" ), depthGroup );
    connect( m_depth16, SIGNAL( clicked() ), this, SLOT( slotPictureDepth16() ) );

    m_depth32 = new QRadioButton( i18n( "32 bit color mode" ), depthGroup );
    connect( m_depth32, SIGNAL( clicked() ), this, SLOT( slotPictureDepth32() ) );

    m_swapRGBCheck = new QCheckBox( i18n( "Convert from RGB image to BGR image" ), gSettings );
    connect( m_swapRGBCheck, SIGNAL( toggled( bool ) ), this, SLOT( slotSwapRGBPicture( bool ) ) );


    m_grayscalCheck = new QCheckBox( i18n( "Grayscale" ), gSettings );
    connect( m_grayscalCheck, SIGNAL( toggled( bool ) ), this, SLOT( slotGrayscalPicture( bool ) ) );


    m_brightValue = new KIntNumInput( bright, gSettings );
    m_brightValue->setRange( -1000, 1000, 10 );
    m_brightValue->setLabel( i18n( "Brightness:" ) );
    m_brightValue->setSuffix( i18n( " %" ) );
    connect( m_brightValue, SIGNAL( valueChanged( int ) ), this, SLOT( slotBrightValue( int ) ) );

    hbox->addWidget( gSettings );

    // ------------------------ preview
    picturePreview = new PicturePreview(this, "preview");
    hbox->addWidget( picturePreview );

    connect( m_normalPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotNormalPicture() ) );

    connect( m_horizontalMirrorPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotHorizontalMirrorPicture() ) );

    connect( m_verticalMirrorPicture, SIGNAL( clicked() ), picturePreview, SLOT( slotVerticalMirrorPicture() ) );

    connect( m_horizontalAndVerticalMirrorPicture, SIGNAL( clicked() ),
             picturePreview, SLOT( slotHorizontalAndVerticalMirrorPicture() ) );

    connect( m_depth0, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth0() ) );

    connect( m_depth1, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth1() ) );

    connect( m_depth8, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth8() ) );

    connect( m_depth16, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth16() ) );

    connect( m_depth32, SIGNAL( clicked() ), picturePreview, SLOT( slotPictureDepth32() ) );

    connect( m_swapRGBCheck, SIGNAL( toggled( bool ) ), picturePreview, SLOT( slotSwapRGBPicture( bool ) ) );

    connect( m_grayscalCheck, SIGNAL( toggled( bool ) ), picturePreview, SLOT( slotGrayscalPicture( bool ) ) );

    connect( m_brightValue, SIGNAL( valueChanged( int ) ), picturePreview, SLOT( slotBrightValue( int ) ) );
    slotReset();
}

ConfPictureDia::~ConfPictureDia()
{
    delete picturePreview;
}

void ConfPictureDia::slotReset()
{
    m_depth0->setChecked( depth == 0 );
    m_depth1->setChecked( depth == 1 );
    m_depth8->setChecked( depth == 8 );
    m_depth16->setChecked( depth == 16 );
    m_depth32->setChecked( depth == 32 );

    picturePreview->setDepth( depth );

    m_normalPicture->setChecked( mirrorType == PM_NORMAL );
    m_horizontalMirrorPicture->setChecked( mirrorType == PM_HORIZONTAL );
    m_verticalMirrorPicture->setChecked( mirrorType == PM_VERTICAL );
    m_horizontalAndVerticalMirrorPicture->setChecked( mirrorType == PM_HORIZONTALANDVERTICAL );
    picturePreview->setMirrorType (mirrorType);
    m_brightValue->setValue(bright );
    m_swapRGBCheck->setChecked( swapRGB );
    m_grayscalCheck->setChecked( grayscal );
}

void ConfPictureDia::slotNormalPicture()
{
    mirrorType = PM_NORMAL;
}

void ConfPictureDia::slotHorizontalMirrorPicture()
{
    mirrorType = PM_HORIZONTAL;
}

void ConfPictureDia::slotVerticalMirrorPicture()
{
    mirrorType = PM_VERTICAL;
}

void ConfPictureDia::slotHorizontalAndVerticalMirrorPicture()
{
    mirrorType = PM_HORIZONTALANDVERTICAL;
}

void ConfPictureDia::slotPictureDepth0()
{
    depth = 0;
}

void ConfPictureDia::slotPictureDepth1()
{
    depth = 1;
}

void ConfPictureDia::slotPictureDepth8()
{
    depth = 8;
}

void ConfPictureDia::slotPictureDepth16()
{
    depth = 16;
}

void ConfPictureDia::slotPictureDepth32()
{
    depth = 32;
}

void ConfPictureDia::slotSwapRGBPicture( bool _on )
{
    swapRGB = _on;
}

void ConfPictureDia::slotGrayscalPicture( bool _on )
{
    grayscal = _on;
}

void ConfPictureDia::slotBrightValue( int _value )
{
    bright = _value;
}

void ConfPictureDia::setPictureMirrorType(const PictureMirrorType &_mirrorType)
{
    mirrorType = _mirrorType;
    picturePreview->setMirrorType(mirrorType);
    m_normalPicture->setChecked(mirrorType == PM_NORMAL);
    m_horizontalMirrorPicture->setChecked(mirrorType == PM_HORIZONTAL);
    m_verticalMirrorPicture->setChecked(mirrorType == PM_VERTICAL);
    m_horizontalAndVerticalMirrorPicture->setChecked(mirrorType == PM_HORIZONTALANDVERTICAL);
}

void ConfPictureDia::setPictureDepth(int _depth)
{
    depth = _depth;
    picturePreview->setDepth(depth);
    m_depth0->setChecked(depth == 0);
    m_depth1->setChecked(depth == 1);
    m_depth8->setChecked(depth == 8);
    m_depth16->setChecked(depth == 16);
    m_depth32->setChecked(depth == 32);
}

void ConfPictureDia::setPictureSwapRGB(bool _swapRGB)
{
    swapRGB = _swapRGB;
    picturePreview->slotSwapRGBPicture(swapRGB);
    m_swapRGBCheck->setChecked(swapRGB);
}

void ConfPictureDia::setPictureGrayscal(bool _grayscal)
{
    grayscal = _grayscal;
    picturePreview->slotGrayscalPicture(grayscal);
    m_grayscalCheck->setChecked( grayscal );
}

void ConfPictureDia::setPictureBright(int _bright)
{
    bright = _bright;
    picturePreview->slotBrightValue(bright);
    m_brightValue->setValue( bright );
}

void ConfPictureDia::setPicturePixmap(QPixmap _pixmap)
{
    origPixmap = _pixmap;
    picturePreview->setPicturePixmap(origPixmap);
}

#include "confpicturedia.moc"
