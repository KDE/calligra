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
#include <qvbuttongroup.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpen.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qwmatrix.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>

#include <klocale.h>
#include <kglobalsettings.h>
#include <kbuttonbox.h>
#include <knuminput.h>

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

    setMinimumSize( 200, 200 );
}

/*====================== draw contents ===========================*/
void RotatePreview::drawContents( QPainter* painter )
{
    QFont font(KGlobalSettings::generalFont().family(), 20, QFont::Bold);
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
    // ------------------------ layout
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );
    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( 5 );
       
    // ------------------------ angles
    angle = new QVButtonGroup( i18n( "Angle" ), this);
    angle->setExclusive(true);
    angle->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding) );

    deg0 = new QRadioButton( "0°", angle );
    deg90 = new QRadioButton( "90°", angle );
    deg180 = new QRadioButton( "180°", angle );
    deg270 = new QRadioButton( "270°", angle );
    degCustom = new QRadioButton( i18n( "Custom: " ), angle );

    custom = new KDoubleNumInput(0.0, angle);
    custom->setRange(0.0, 360.0);
    custom->setSuffix(" °"); 

    hbox->addWidget( angle );

    connect( deg0, SIGNAL( clicked() ), this, SLOT( deg0clicked() ) );
    connect( deg90, SIGNAL( clicked() ), this, SLOT( deg90clicked() ) );
    connect( deg180, SIGNAL( clicked() ), this, SLOT( deg180clicked() ) );
    connect( deg270, SIGNAL( clicked() ), this, SLOT( deg270clicked() ) );
    connect( degCustom, SIGNAL( clicked() ), this, SLOT( degCustomclicked() ) );
    connect( custom, SIGNAL( valueChanged( double ) ), this, SLOT( degCustomChanged( double ) ) );

    // ------------------------ preview
    rPreview = new RotatePreview( this, "preview" );

    hbox->addWidget( rPreview );

    // ------------------------ buttons
    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();

    okBut = bb->addButton( i18n( "OK" ) );
    okBut->setAutoRepeat( false );
    okBut->setAutoResize( false );
    okBut->setAutoDefault( true );
    okBut->setDefault( true );
    applyBut = bb->addButton( i18n( "Apply" ) );
    cancelBut = bb->addButton( i18n( "Cancel" ) );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( Apply() ) );
    connect( applyBut, SIGNAL( clicked() ), this, SLOT( Apply() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    bb->layout();

    layout->addWidget( bb );
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
        custom->setValue( _angle );
    }

    rPreview->setAngle( _angle );
}

/*======================= deg 0 clicked ==========================*/
void RotateDia::deg0clicked()
{
    custom->setEnabled( false );
    rPreview->setAngle( 0 );
    _angle = 0.0;
}

/*======================= deg 90 clicked =========================*/
void RotateDia::deg90clicked()
{
    custom->setEnabled( false );
    rPreview->setAngle( 90 );
    _angle = 90.0;
}

/*======================= deg 180 clicked ========================*/
void RotateDia::deg180clicked()
{
    custom->setEnabled( false );
    rPreview->setAngle( 180 );
    _angle = 180.0;
}

/*======================= deg 270 clicked ========================*/
void RotateDia::deg270clicked()
{
    custom->setEnabled( false );
    rPreview->setAngle( 270 );
    _angle = 270.0;
}

/*======================= deg custom clicked =====================*/
void RotateDia::degCustomclicked()
{
    custom->setEnabled( true );
    rPreview->setAngle( custom->value() );
    _angle = custom->value();
}

/*======================= deg custom changed =====================*/
void RotateDia::degCustomChanged( double t )
{
    rPreview->setAngle( t );
    _angle=t;
}
