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

#include "rotatedia.h"
#include "rotatedia.moc"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpen.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qwmatrix.h>
#include <qvalidator.h>
#include <qlineedit.h>

#include <qpoint.h>
#include <qrect.h>
#include <kapp.h>
#include <klocale.h>

#include <stdlib.h>

/******************************************************************/
/* class RotatePreview                                            */
/******************************************************************/

/*==================== constructor ===============================*/
RotatePreview::RotatePreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( white );
}

/*====================== draw contents ===========================*/
void RotatePreview::drawContents( QPainter* painter )
{
    QFont font( "utopia", 20 );
    font.setBold( true );
    QFontMetrics fm( font );

    QRect br = fm.boundingRect( "KOffice" );
    int pw = br.width();
    int ph = br.height();
    QRect r = br;
    int textYPos = -r.y();
    int textXPos = -r.x();
    br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
    r.moveTopLeft( QPoint( -r.width() / 2, -r.height() / 2 ) );

    painter->save();
    painter->setViewport( ( width() - pw ) / 2, ( height() - ph ) / 2, width(), height() );

    QWMatrix m, mtx;
    mtx.rotate( _angle );
    m.translate( pw / 2, ph / 2 );
    m = mtx * m;

    painter->setWorldMatrix( m );

    painter->setPen( QPen( blue ) );
    painter->setFont( font );

    painter->drawText( r.left() + textXPos, r.top() + textYPos, "KOffice" );

    painter->restore();
}

/******************************************************************/
/* class RotateDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
RotateDia::RotateDia( QWidget* parent, const char* name )
    : QDialog( parent, name, true )
{
    angle = new QGroupBox( i18n( "Angle" ), this );
    angle->move( 20, 20 );

    deg0 = new QRadioButton( "0°", angle );
    deg90 = new QRadioButton( "90°", angle );
    deg180 = new QRadioButton( "180°", angle );
    deg270 = new QRadioButton( "270°", angle );
    degCustom = new QRadioButton( i18n( "Custom: " ), angle );

    deg270->resize( deg270->sizeHint() );
    deg0->resize( deg270->size() );
    deg90->resize( deg270->size() );
    deg180->resize( deg270->size() );
    degCustom->resize( degCustom->sizeHint() );

    custom = new QLineEdit( angle );
    custom->setValidator( new QDoubleValidator( custom ) );
    custom->resize( custom->sizeHint().width() / 2, custom->sizeHint().height() );
    custom->setText( "0.0" );

    int w = degCustom->width() + custom->width() + 10;

    deg0->move( 20, 30 );
    deg90->move( 20 + w - deg90->width(), 30 );
    deg180->move( 20, deg0->y() + deg0->height() + 10 );
    deg270->move( 20 + w - deg90->width(), deg180->y() );

    degCustom->move( 20, deg180->y() + deg180->height() + 10 );
    custom->move( degCustom->x() + degCustom->width() + 10, degCustom->y() );

    angle->resize( custom->x() + custom->width() + 20, custom->y() + custom->height() + 20 );

    connect( deg0, SIGNAL( clicked() ), this, SLOT( deg0clicked() ) );
    connect( deg90, SIGNAL( clicked() ), this, SLOT( deg90clicked() ) );
    connect( deg180, SIGNAL( clicked() ), this, SLOT( deg180clicked() ) );
    connect( deg270, SIGNAL( clicked() ), this, SLOT( deg270clicked() ) );
    connect( degCustom, SIGNAL( clicked() ), this, SLOT( degCustomclicked() ) );
    connect( custom, SIGNAL( textChanged( const QString & ) ), this, SLOT( degCustomChanged( const QString & ) ) );

    preview = new QGroupBox( i18n( "Preview" ), this );
    preview->move( angle->x() + angle->width() + 20, 20 );
    preview->resize( angle->size() );

    rPreview = new RotatePreview( preview, "preview" );
    rPreview->setGeometry( 10, 20, preview->width() - 20, preview->height() - 30 );

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

    cancelBut->move( preview->x() + preview->width() - butW, preview->y() + preview->height() + 25 );
    applyBut->move( cancelBut->x() - 5 - applyBut->width(), cancelBut->y() );
    okBut->move( applyBut->x() - 10 - okBut->width(), cancelBut->y() );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( Apply() ) );
    connect( applyBut, SIGNAL( clicked() ), this, SLOT( Apply() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    resize( preview->x() + preview->width() + 20, preview->y() + preview->height() + 20 + butH + 20 );
}

/*===================== destructor ===============================*/
RotateDia::~RotateDia()
{
}

/*========================== set angle ===========================*/
void RotateDia::setAngle( float __angle )
{
    _angle = __angle;

    deg0->setChecked( false );
    deg90->setChecked( false );
    deg180->setChecked( false );
    deg270->setChecked( false );
    degCustom->setChecked( false );
    custom->setEnabled( false );

    if ( _angle == 0.0 ) deg0->setChecked( true );
    else if ( _angle == 90.0 ) deg90->setChecked( true );
    else if ( _angle == 180.0 ) deg180->setChecked( true );
    else if ( _angle == 270.0 ) deg270->setChecked( true );
    else {
        degCustom->setChecked( true );
        custom->setEnabled( true );
        QString str=QString::number( _angle );
        custom->setText( str );
    }

    rPreview->setAngle( _angle );
}

/*======================= deg 0 clicked ==========================*/
void RotateDia::deg0clicked()
{
    deg0->setChecked( true );
    deg90->setChecked( false );
    deg180->setChecked( false );
    deg270->setChecked( false );
    degCustom->setChecked( false );
    custom->setEnabled( false );
    rPreview->setAngle( 0 );
    _angle = 0.0;
}

/*======================= deg 90 clicked =========================*/
void RotateDia::deg90clicked()
{
    deg0->setChecked( false );
    deg90->setChecked( true );
    deg180->setChecked( false );
    deg270->setChecked( false );
    degCustom->setChecked( false );
    custom->setEnabled( false );
    rPreview->setAngle( 90 );
    _angle = 90.0;
}

/*======================= deg 180 clicked ========================*/
void RotateDia::deg180clicked()
{
    deg0->setChecked( false );
    deg90->setChecked( false );
    deg180->setChecked( true );
    deg270->setChecked( false );
    degCustom->setChecked( false );
    custom->setEnabled( false );
    rPreview->setAngle( 180 );
    _angle = 180.0;
}

/*======================= deg 270 clicked ========================*/
void RotateDia::deg270clicked()
{
    deg0->setChecked( false );
    deg90->setChecked( false );
    deg180->setChecked( false );
    deg270->setChecked( true );
    degCustom->setChecked( false );
    custom->setEnabled( false );
    rPreview->setAngle( 270 );
    _angle = 270.0;
}

/*======================= deg custom clicked =====================*/
void RotateDia::degCustomclicked()
{
    deg0->setChecked( false );
    deg90->setChecked( false );
    deg180->setChecked( false );
    deg270->setChecked( false );
    degCustom->setChecked( true );
    custom->setEnabled( true );

    QString str = custom->text();
    rPreview->setAngle( str.toDouble() );
    _angle = str.toDouble();
}

/*======================= deg custom changed =====================*/
void RotateDia::degCustomChanged( const QString & t )
{
    _angle=t.toDouble();
    rPreview->setAngle( _angle );
}











