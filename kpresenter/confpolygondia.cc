// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Base code from Kontour.
   Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

   Copyright (C) 2001 Toshitaka Fujioka <fujioka@kde.org>

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

#include "confpolygondia.h"

#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qgroupbox.h>
#include <qlayout.h>

#include <knuminput.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include "kprcommand.h"

#include <qradiobutton.h>

#include "polygonpreview.h"

ConfPolygonDia::ConfPolygonDia( QWidget *parent, const char *name )
    : QWidget( parent, name ), m_bCheckConcaveChanged(false), m_bCornersChanged(false),
      m_bSharpnessChanged(false)
{
    // ------------------------ layout
    QVBoxLayout *layout = new QVBoxLayout( this, 0 );
    layout->setMargin( KDialog::marginHint() );
    layout->setSpacing( KDialog::spacingHint() );

    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( KDialog::spacingHint() );

    // ------------------------ settings
    gSettings = new QGroupBox( 1, Qt::Horizontal, i18n( "Settings" ), this );

    QButtonGroup *group = new QVButtonGroup( i18n( "Convex/Concave" ), gSettings );

    m_convexPolygon = new QRadioButton( i18n( "Polygon" ), group );


    connect( m_convexPolygon, SIGNAL( clicked() ), this, SLOT( slotConvexPolygon() ) );

    m_concavePolygon = new QRadioButton( i18n( "Concave polygon" ), group );
    connect( m_concavePolygon, SIGNAL( clicked() ), this, SLOT( slotConcavePolygon() ) );

    m_corners = new KIntNumInput( 0 , gSettings );
    m_corners->setRange( 3, 100, 1 );
    m_corners->setLabel( i18n( "Corners:" ) );
    connect( m_corners, SIGNAL( valueChanged( int ) ), this, SLOT( slotCornersValue( int ) ) );

    m_sharpness = new KIntNumInput( 0 , gSettings );
    m_sharpness->setRange( 0, 100, 1 );
    m_sharpness->setLabel( i18n( "Sharpness:" ) );
    connect( m_sharpness, SIGNAL( valueChanged( int ) ), this, SLOT( slotSharpnessValue( int ) ) );

    hbox->addWidget( gSettings );

    // ------------------------ preview
    polygonPreview = new PolygonPreview( this, "preview");
    hbox->addWidget( polygonPreview );

    connect ( m_convexPolygon, SIGNAL( clicked() ), polygonPreview,
              SLOT( slotConvexPolygon() ) );
    connect ( m_concavePolygon, SIGNAL( clicked() ), polygonPreview,
              SLOT( slotConcavePolygon() ) );
    connect( m_corners, SIGNAL( valueChanged( int ) ), polygonPreview,
             SLOT( slotCornersValue( int ) ) );
    connect( m_sharpness, SIGNAL( valueChanged( int ) ), polygonPreview,
             SLOT( slotSharpnessValue( int ) ) );
    slotReset();
}

ConfPolygonDia::~ConfPolygonDia()
{
    delete polygonPreview;
}

void ConfPolygonDia::slotConvexPolygon()
{
    m_sharpness->setEnabled( false );
    checkConcavePolygon = false;
}

void ConfPolygonDia::slotConcavePolygon()
{
    m_bCheckConcaveChanged = true;
    m_sharpness->setEnabled( true );
    checkConcavePolygon = true;
}

void ConfPolygonDia::slotCornersValue( int value )
{
    m_bCornersChanged = true;
    cornersValue = value;
}

void ConfPolygonDia::slotSharpnessValue( int value )
{
    m_bSharpnessChanged = true;
    sharpnessValue = value;
}

void ConfPolygonDia::slotReset()
{
    if ( oldCheckConcavePolygon )
    {
        m_convexPolygon->setChecked( false );
        polygonPreview->slotConcavePolygon();
    }
    else
    {
        m_convexPolygon->setChecked( true );
        polygonPreview->slotConvexPolygon();
    }
    checkConcavePolygon = oldCheckConcavePolygon;

    m_concavePolygon->setChecked( oldCheckConcavePolygon );
    m_sharpness->setEnabled( oldCheckConcavePolygon );
    m_sharpness->setValue( oldSharpnessValue );
    m_corners->setValue( oldCornersValue );
    resetConfigChangedValues();
    polygonPreview->repaint();
}

int ConfPolygonDia::getPolygonConfigChange() const
{
    int flags = 0;
    if (m_bCheckConcaveChanged)
        flags = flags | PolygonSettingCmd::ConcaveConvex;
    if (m_bCornersChanged)
        flags = flags | PolygonSettingCmd::Corners;
    if (m_bSharpnessChanged)
        flags = flags | PolygonSettingCmd::Sharpness;

    return flags;
}

void ConfPolygonDia::setCheckConcavePolygon(bool _concavePolygon)
{
    checkConcavePolygon = _concavePolygon;
    oldCheckConcavePolygon = _concavePolygon;
    if (checkConcavePolygon)
    {
        m_concavePolygon->setChecked(true);
        m_sharpness->setEnabled(true);
        polygonPreview->slotConcavePolygon();
    }
    else
    {
        m_convexPolygon->setChecked(true);
        m_sharpness->setEnabled(false);
        polygonPreview->slotConvexPolygon();
    }
}

void ConfPolygonDia::setCornersValue(int _cornersValue)
{
    cornersValue = _cornersValue;
    oldCornersValue = _cornersValue;
    polygonPreview->slotCornersValue(cornersValue);
    m_corners->setValue(cornersValue);
}

void ConfPolygonDia::setSharpnessValue(int _sharpnessValue)
{
    sharpnessValue = _sharpnessValue;
    oldSharpnessValue = _sharpnessValue;
    polygonPreview->slotSharpnessValue(sharpnessValue);
    m_sharpness->setValue(sharpnessValue);
}

void ConfPolygonDia::setPenBrush( const QPen &_pen, const QBrush &_brush )
{
    polygonPreview->setPenBrush( _pen, _brush );
}

void ConfPolygonDia::resetConfigChangedValues()
{
    m_bCheckConcaveChanged = false;
    m_bCornersChanged = false;
    m_bSharpnessChanged = false;
}

#include "confpolygondia.moc"
