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
#include <qvalidator.h>

#include <klocale.h>

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
    angle = 45 * 16;
    len = 90 * 16;
    pen = QPen( black );
    brush = QBrush( white );
    type = PT_PIE;
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
    : QDialog( parent, name, true )
{
    gSettings = new QGroupBox( i18n( "Settings" ), this );
    gSettings->move( 20, 20 );

    lType = new QLabel( i18n( "Type:" ), gSettings );
    lType->resize( lType->sizeHint() );
    lType->move( 20, 25 );

    cType = new QComboBox( false, gSettings );
    cType->insertItem( i18n( "Pie" ), -1 );
    cType->insertItem( i18n( "Arc" ), -1 );
    cType->insertItem( i18n( "Chord" ), -1 );
    cType->resize( cType->sizeHint() );
    cType->move( lType->x() + lType->width() + 10, lType->y() );
    connect( cType, SIGNAL( activated( int ) ), this, SLOT( typeChanged( int ) ) );

    lType->move( lType->x(), lType->y() + ( cType->height() - lType->height() ) / 2 );

    lAngle = new QLabel( i18n( "Angle ( 0 .. 5760 = ( 0 * 16 ) .. ( 360 * 16 ) ):" ), gSettings );
    lAngle->resize( lAngle->sizeHint() );
    lAngle->move( lType->x(), cType->y() + cType->height() + 20 );

    eAngle = new QLineEdit( gSettings );
    eAngle->setValidator( new QIntValidator( eAngle ) );
    eAngle->resize( eAngle->sizeHint() );
    eAngle->move( lAngle->x(), lAngle->y() + lAngle->height() + 5 );
    connect( eAngle, SIGNAL( textChanged( const QString & ) ), this, SLOT( angleChanged( const QString & ) ) );

    lLen = new QLabel( i18n( "Length ( 0 .. 5760 = ( 0 * 16 ) .. ( 360 * 16 ) ):" ), gSettings );
    lLen->resize( lLen->sizeHint() );
    lLen->move( eAngle->x(), eAngle->y() + eAngle->height() + 20 );

    eLen = new QLineEdit( gSettings );
    eLen->setValidator( new QIntValidator( eLen ) );
    eLen->resize( eLen->sizeHint() );
    eLen->move( lLen->x(), lLen->y() + lLen->height() + 5 );
    connect( eLen, SIGNAL( textChanged( const QString & ) ), this, SLOT( lengthChanged( const QString & ) ) );

    gSettings->resize(QMAX(QMAX(QMAX(QMAX(cType->x() + cType->width(),lAngle->x() + lAngle->width()),eAngle->x() + eAngle->width()),
                              lLen->x() + lLen->width() ), eLen->x() + eLen->width() ) + 20,
                      eLen->y() + eLen->height() + 20 );

    gPreview = new QGroupBox( i18n( "Preview" ), this );
    gPreview->move( gSettings->x() + gSettings->width() + 20, 20 );
    gPreview->resize( gSettings->size() );

    piePreview = new PiePreview( gPreview, "preview" );
    piePreview->setGeometry( 10, 20, gPreview->width() - 20, gPreview->height() - 30 );

    cancelBut = new QPushButton( this, "BCancel" );
    cancelBut->setText( i18n( "Cancel" ) );

    applyBut = new QPushButton( this, "BApply" );
    applyBut->setText( i18n( "Apply" ) );

    okBut = new QPushButton( this, "BOK" );
    okBut->setText( i18n( "OK" ) );
    okBut->setAutoRepeat( false );
    okBut->setAutoResize( false );
    okBut->setAutoDefault( true );
    okBut->setDefault( true );

    int butW = QMAX(cancelBut->sizeHint().width(),
                   QMAX(applyBut->sizeHint().width(),okBut->sizeHint().width()));
    int butH = cancelBut->sizeHint().height();

    cancelBut->resize( butW, butH );
    applyBut->resize( butW, butH );
    okBut->resize( butW, butH );

    cancelBut->move( gPreview->x() + gPreview->width() - butW, gPreview->y() + gPreview->height() + 25 );
    applyBut->move( cancelBut->x() - 5 - applyBut->width(), cancelBut->y() );
    okBut->move( applyBut->x() - 10 - okBut->width(), cancelBut->y() );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( Apply() ) );
    connect( applyBut, SIGNAL( clicked() ), this, SLOT( Apply() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    resize( gPreview->x() + gPreview->width() + 20, gPreview->y() + gPreview->height() + 20 + butH + 20 );
}

/*===================== destructor ===============================*/
ConfPieDia::~ConfPieDia()
{
}

/*================================================================*/
void ConfPieDia::lengthChanged( const QString & _len )
{
    len = _len.toInt();
    piePreview->setLength( len );
}

/*================================================================*/
void ConfPieDia::angleChanged( const QString & _angle )
{
    angle = _angle.toInt();
    piePreview->setAngle( angle );
}

/*================================================================*/
void ConfPieDia::typeChanged( int _type )
{
    type = static_cast<PieType>( _type );
    piePreview->setType( type );
}

#include <confpiedia.moc>
