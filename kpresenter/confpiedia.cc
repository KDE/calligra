/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <confpiedia.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qlayout.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <knuminput.h>
#include <kprcommand.h>

#include <qpen.h>
#include <qbrush.h>
#include <qcombobox.h>
#include <global.h>

#include <stdlib.h>

/******************************************************************/
/* class PiePreview                                               */
/******************************************************************/

/*==================== constructor ===============================*/
PiePreview::PiePreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( white );
    angle = 720; //45 * 16
    len = 1440; //90 * 16
    pen = QPen( black );
    brush = QBrush( white );
    type = PT_PIE;

    setMinimumSize( 200, 100 );
}

/*====================== draw contents ===========================*/
void PiePreview::drawContents( QPainter* painter )
{
    int ow = width() - 8;
    int oh = height() - 8;

    painter->setPen( pen );
    int pw = pen.width();
    painter->setBrush( brush );

    switch ( type )
    {
    case PT_PIE:
        painter->drawPie( pw + 4, pw + 4, ow - 2 * pw, oh - 2 * pw, angle, len );
        break;
    case PT_ARC:
        painter->drawArc( pw + 4, pw + 4, ow - 2 * pw, oh - 2 * pw, angle, len );
        break;
    case PT_CHORD:
        painter->drawChord( pw + 4, pw + 4, ow - 2 * pw, oh - 2 * pw, angle, len );
        break;
    default: break;
    }
}

/******************************************************************/
/* class ConfPieDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
ConfPieDia::ConfPieDia( QWidget* parent, const char* name )
    : QWidget( parent, name ), m_bTypeChanged(false), m_bAngleChanged(false), m_bLengthChanged(false)
{
  // ------------------------ layout
  QVBoxLayout *layout = new QVBoxLayout( this, 0 );
  layout->setMargin( 5 );
  layout->setSpacing( 5 );

  QHBoxLayout *hbox = new QHBoxLayout( layout );
  hbox->setSpacing( 5 );

  // ------------------------ settings
  gSettings = new QGroupBox( 6, Qt::Vertical, i18n( "Settings" ), this );

  lType = new QLabel( i18n( "Type:" ), gSettings );

  cType = new QComboBox( false, gSettings );
  cType->insertItem( i18n( "Pie" ) );
  cType->insertItem( i18n( "Arc" ) );
  cType->insertItem( i18n( "Chord" ) );

  connect( cType, SIGNAL( activated( int ) ), this, SLOT( typeChanged( int ) ) );

  lAngle = new QLabel( i18n( "Angle:" ), gSettings );

  eAngle = new KIntNumInput( gSettings );
  eAngle->setRange(0, 360);
  eAngle->setSuffix(" °");
  connect( eAngle, SIGNAL( valueChanged( int ) ), this, SLOT( angleChanged( int ) ) );

  lLen = new QLabel( i18n( "Length:" ), gSettings );

  eLen = new KIntNumInput( gSettings );
  eLen->setRange(0, 360);
  eLen->setSuffix(" °");

  connect( eLen, SIGNAL( valueChanged( int ) ), this, SLOT( lengthChanged( int ) ) );

  hbox->addWidget( gSettings );

  // ------------------------ preview
  piePreview = new PiePreview( this, "preview" );

  hbox->addWidget( piePreview );
  slotReset();
}

/*===================== destructor ===============================*/
ConfPieDia::~ConfPieDia()
{
    delete piePreview;
}

/*================================================================*/
void ConfPieDia::lengthChanged( int _len )
{
    m_bLengthChanged = true;
    len = _len*16;
    piePreview->setLength( len );
}

/*================================================================*/
void ConfPieDia::angleChanged( int _angle )
{
    m_bAngleChanged = true;
    angle = _angle*16;
    piePreview->setAngle( angle );
}

/*================================================================*/
void ConfPieDia::typeChanged( int _type )
{
    m_bTypeChanged = true;
    type = static_cast<PieType>( _type );
    piePreview->setType( type );
}

void ConfPieDia::slotReset()
{
    eAngle->setValue( oldAngle/16 );
    eLen->setValue( oldLen/16 );
    cType->setCurrentItem( oldType );
    type = static_cast<PieType>( oldType );
    piePreview->setLength( oldLen );
    piePreview->setAngle( oldAngle );
    piePreview->setLength( oldLen );
    piePreview->setType( oldType );
    resetConfigChangedValues();
}

void ConfPieDia::setAngle( int _angle )
{
    angle = _angle;
    oldAngle = _angle;
    eAngle->setValue( angle/16 );
    piePreview->setAngle( angle );
}

void ConfPieDia::setLength( int _len )
{
    len = _len;
    oldLen= _len;
    eLen->setValue( len/16 );
    piePreview->setLength( len );
}

void ConfPieDia::setType( PieType _type )
{
    type = _type;
    oldType = _type;
    cType->setCurrentItem( _type );
    piePreview->setType( type );
}

void ConfPieDia::setPenBrush( const QPen &_pen, const QBrush &_brush )
{
    piePreview->setPenBrush( _pen, _brush );
}

void ConfPieDia::resetConfigChangedValues()
{
    m_bTypeChanged = false;
    m_bAngleChanged = false;
    m_bLengthChanged = false;
}

int ConfPieDia::getPieConfigChange() const
{
    int flags = 0;
    if (m_bTypeChanged)
        flags = flags | PieValueCmd::Type;
    if (m_bAngleChanged)
        flags = flags | PieValueCmd::Angle;
    if (m_bLengthChanged)
        flags = flags | PieValueCmd::Length;

    return flags;
}

#include <confpiedia.moc>
