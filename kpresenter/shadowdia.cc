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

#include <shadowdia.h>

#include <kiconloader.h>
#include <klocale.h>
#include <qspinbox.h>
#include <kcolorbutton.h>
#include <kglobal.h>
#include <kglobalsettings.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qfont.h>
#include <qfontmetrics.h>

#include <stdlib.h>

/******************************************************************/
/* class ShadowPreview                                            */
/******************************************************************/

/*==================== constructor ===============================*/
ShadowPreview::ShadowPreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( white );
}

/*====================== draw contents ===========================*/
void ShadowPreview::drawContents( QPainter* painter )
{
    QFont font(KGlobalSettings::generalFont().family(), 30, QFont::Bold);
    QFontMetrics fm( font );

    QRect br = fm.boundingRect( "KOffice" );
    int x = ( width() - br.width() ) / 2;
    int y = ( height() - br.height() ) / 2 + br.height();
    int sx = 0, sy = 0;

    switch ( shadowDirection )
    {
    case SD_LEFT_UP:
    {
        sx = x - shadowDistance;
        sy = y - shadowDistance;
    } break;
    case SD_UP:
    {
        sx = x;
        sy = y - shadowDistance;
    } break;
    case SD_RIGHT_UP:
    {
        sx = x + shadowDistance;
        sy = y - shadowDistance;
    } break;
    case SD_RIGHT:
    {
        sx = x + shadowDistance;
        sy = y;
    } break;
    case SD_RIGHT_BOTTOM:
    {
        sx = x + shadowDistance;
        sy = y + shadowDistance;
    } break;
    case SD_BOTTOM:
    {
        sx = x;
        sy = y + shadowDistance;
    } break;
    case SD_LEFT_BOTTOM:
    {
        sx = x - shadowDistance;
        sy = y + shadowDistance;
    } break;
    case SD_LEFT:
    {
        sx = x - shadowDistance;
        sy = y;
    } break;
    }

    painter->save();

    painter->setFont( font );
    painter->setPen( shadowColor );
    painter->drawText( sx, sy, "KOffice" );

    painter->setPen( blue );
    painter->drawText( x, y, "KOffice" );

    painter->restore();
}

/******************************************************************/
/* class ShadowDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
ShadowDia::ShadowDia( QWidget* parent, const char* name )
    : QDialog( parent, name, true )
{
    shadow = new QGroupBox( i18n( "Shadow" ), this );
    shadow->move( 20, 20 );

    lcolor = new QLabel( i18n( "Color:" ), shadow );
    lcolor->resize( lcolor->sizeHint() );
    lcolor->move( 20, 30 );

    color = new KColorButton( shadow );
    color->resize( color->sizeHint() );
    color->move( lcolor->x(), lcolor->y() + lcolor->height() + 10 );
    connect( color, SIGNAL( changed( const QColor& ) ), this, SLOT( colorChanged( const QColor& ) ) );

    ldistance = new QLabel( i18n( "Distance:" ), shadow );
    ldistance->resize( ldistance->sizeHint() );
    ldistance->move( color->x(), color->y() + color->height() + 20 );

    distance = new QSpinBox( 0, 20, 1, shadow );
    distance->resize( color->width(), distance->sizeHint().height() );
    distance->move( ldistance->x(), ldistance->y() + ldistance->height() + 10 );
    connect( distance, SIGNAL( valueChanged( int ) ), this, SLOT( distanceChanged( int ) ) );

    ldirection = new QLabel( i18n( "Direction:" ), shadow );
    ldirection->resize( ldirection->sizeHint() );
    ldirection->move( color->x() + color->width() + 20, lcolor->y() );

    lu = new QPushButton( shadow );
    lu->resize( 26, 26 );
    lu->setToggleButton( true );
    u = new QPushButton( shadow );
    u->resize( 26, 26 );
    u->setToggleButton( true );
    ru = new QPushButton( shadow );
    ru->resize( 26, 26 );
    ru->setToggleButton( true );
    r = new QPushButton( shadow );
    r->resize( 26, 26 );
    r->setToggleButton( true );
    rb = new QPushButton( shadow );
    rb->resize( 26, 26 );
    rb->setToggleButton( true );
    b = new QPushButton( shadow );
    b->resize( 26, 26 );
    b->setToggleButton( true );
    lb = new QPushButton( shadow );
    lb->resize( 26, 26 );
    lb->setToggleButton( true );
    l = new QPushButton( shadow );
    l->resize( 26, 26 );
    l->setToggleButton( true );

    lu->move( ldirection->x(), ldirection->y() + ldirection->height() + 10 );
    u->move( lu->x() + lu->width(), lu->y() );
    ru->move( u->x() + u->width(), lu->y() );
    r->move( ru->x(), lu->y() + lu->height() );
    rb->move( ru->x(), r->y() + r->height() );
    b->move( u->x(), rb->y() );
    lb->move( lu->x(), b->y() );
    l->move( lu->x(), r->y() );

    lu->setPixmap( KPBarIcon( "shadowLU" ) );
    u->setPixmap( KPBarIcon( "shadowU" ) );
    ru->setPixmap( KPBarIcon( "shadowRU" ) );
    r->setPixmap( KPBarIcon( "shadowR" ) );
    rb->setPixmap( KPBarIcon( "shadowRB" ) );
    b->setPixmap( KPBarIcon( "shadowB" ) );
    lb->setPixmap( KPBarIcon( "shadowLB" ) );
    l->setPixmap( KPBarIcon( "shadowL" ) );

    connect( lu, SIGNAL( clicked() ), this, SLOT( luChanged() ) );
    connect( u, SIGNAL( clicked() ), this, SLOT( uChanged() ) );
    connect( ru, SIGNAL( clicked() ), this, SLOT( ruChanged() ) );
    connect( r, SIGNAL( clicked() ), this, SLOT( rChanged() ) );
    connect( rb, SIGNAL( clicked() ), this, SLOT( rbChanged() ) );
    connect( b, SIGNAL( clicked() ), this, SLOT( bChanged() ) );
    connect( lb, SIGNAL( clicked() ), this, SLOT( lbChanged() ) );
    connect( l, SIGNAL( clicked() ), this, SLOT( lChanged() ) );

    shadow->resize( rb->x() + rb->width() + 20, distance->y() + distance->height() + 20 );

    preview = new QGroupBox( i18n( "Preview" ), this );
    preview->move( shadow->x() + shadow->width() + 20, shadow->y() );
    preview->resize( shadow->size() );

    sPreview = new ShadowPreview( preview, "" );
    sPreview->setGeometry( 10, 20, preview->width() - 20, preview->height() - 30 );

    okBut = new QPushButton( i18n( "OK" ), this, "BOK" );
    okBut->setAutoRepeat( false );
    okBut->setAutoResize( false );	//lukas: obsolete
    okBut->setAutoDefault( true );
    okBut->setDefault( true );

    applyBut = new QPushButton( i18n( "Apply" ), this, "BApply" );

    cancelBut = new QPushButton( i18n( "Cancel" ), this, "BCancel" );

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
ShadowDia::~ShadowDia()
{
}

/*====================== set shadow direction ====================*/
void ShadowDia::setShadowDirection( ShadowDirection sd )
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = sd;
    sPreview->setShadowDirection( shadowDirection );

    switch ( shadowDirection )
    {
    case SD_LEFT_UP: lu->setOn( true ); break;
    case SD_UP: u->setOn( true ); break;
    case SD_RIGHT_UP: ru->setOn( true ); break;
    case SD_RIGHT: r->setOn( true ); break;
    case SD_RIGHT_BOTTOM: rb->setOn( true ); break;
    case SD_BOTTOM: b->setOn( true ); break;
    case SD_LEFT_BOTTOM: lb->setOn( true ); break;
    case SD_LEFT: l->setOn( true ); break;
    }
}

/*====================== set shadow distance =====================*/
void ShadowDia::setShadowDistance( int sd )
{
    shadowDistance = sd;
    sPreview->setShadowDistance( shadowDistance );

    distance->setValue( shadowDistance );
}

/*====================== set shadow color ========================*/
void ShadowDia::setShadowColor( QColor sc )
{
    shadowColor = sc;
    sPreview->setShadowColor( shadowColor );

    color->setColor( shadowColor );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::luChanged()
{
    lu->setOn( true );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = SD_LEFT_UP;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::uChanged()
{
    lu->setOn( false );
    u->setOn( true );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = SD_UP;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::ruChanged()
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( true );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = SD_RIGHT_UP;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::rChanged()
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( true );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = SD_RIGHT;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::rbChanged()
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( true );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = SD_RIGHT_BOTTOM;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::bChanged()
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( true );
    lb->setOn( false );
    l->setOn( false );

    shadowDirection = SD_BOTTOM;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::lbChanged()
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( true );
    l->setOn( false );

    shadowDirection = SD_LEFT_BOTTOM;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= shadow direction changed ===============*/
void ShadowDia::lChanged()
{
    lu->setOn( false );
    u->setOn( false );
    ru->setOn( false );
    r->setOn( false );
    rb->setOn( false );
    b->setOn( false );
    lb->setOn( false );
    l->setOn( true );

    shadowDirection = SD_LEFT;
    sPreview->setShadowDirection( shadowDirection );
}

/*======================= color changed ==========================*/
void ShadowDia::colorChanged( const QColor& col )
{
    shadowColor = col;
    sPreview->setShadowColor( col );
}

/*======================= distance changed =======================*/
void ShadowDia::distanceChanged( int _val )
{
    shadowDistance = _val;
    sPreview->setShadowDistance( shadowDistance );
}

#include <shadowdia.moc>
