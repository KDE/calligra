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

#include <confpolygondia.h>

#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qgroupbox.h>
#include <qpainter.h>
#include <qlayout.h>

#include <knuminput.h>
#include <klocale.h>
#include <kbuttonbox.h>

#include <stdlib.h>
#include <math.h>
#include <qradiobutton.h>

/******************************************************************/
/* class PolygonPreview                                           */
/******************************************************************/

/*==================== constructor ===============================*/
PolygonPreview::PolygonPreview( QWidget* parent, const char* name)
    : QFrame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( white );

    setMinimumSize( 200, 100 );
}

/*====================== draw contents ===========================*/
void PolygonPreview::drawContents( QPainter *painter )
{
    double angle = 2 * M_PI / nCorners;
    double diameter = static_cast<double>( QMAX( width(), height() ) - 10 );
    double radius = diameter * 0.5;

    painter->setWindow( qRound( -radius ), qRound( -radius ), qRound( diameter ), qRound( diameter ) );
    painter->setViewport( 5, 5, width() - 10, height() - 10 );
    painter->setPen( pen );
    painter->setBrush( brush );

    QPointArray points( isConcave ? nCorners * 2 : nCorners );
    points.setPoint( 0, 0, qRound( -radius ) );

    if ( isConcave ) {
        angle = angle / 2.0;
        double a = angle;
        double r = radius - ( sharpness / 100.0 * radius );
        for ( int i = 1; i < nCorners * 2; ++i ) {
            double xp, yp;
            if ( i % 2 ) {
                xp =  r * sin( a );
                yp = -r * cos( a );
            }
            else {
                xp = radius * sin( a );
                yp = -radius * cos( a );
            }
            a += angle;
            points.setPoint( i, (int)xp, (int)yp );
        }
    }
    else {
        double a = angle;
        for ( int i = 1; i < nCorners; ++i ) {
            double xp = radius * sin( a );
            double yp = -radius * cos( a );
            a += angle;
            points.setPoint( i, (int)xp, (int)yp );
        }
    }
    painter->drawPolygon( points );
}

void PolygonPreview::slotConvexPolygon()
{
    isConcave = false;
    repaint();
}

void PolygonPreview::slotConcavePolygon()
{
    isConcave = true;
    repaint();
}

void PolygonPreview::slotCornersValue( int value )
{
    nCorners = value;
    repaint();
}

void PolygonPreview::slotSharpnessValue( int value )
{
    sharpness = value;
    repaint();
}

/******************************************************************/
/* class ConfPolygonDia                                           */
/******************************************************************/

/*==================== constructor ===============================*/
ConfPolygonDia::ConfPolygonDia( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    // ------------------------ layout
    QVBoxLayout *layout = new QVBoxLayout( this, 0 );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );

    QHBoxLayout *hbox = new QHBoxLayout( layout );
    hbox->setSpacing( 5 );

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

/*===================== destructor ===============================*/
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
    m_sharpness->setEnabled( true );
    checkConcavePolygon = true;
}

void ConfPolygonDia::slotCornersValue( int value )
{
    cornersValue = value;
}

void ConfPolygonDia::slotSharpnessValue( int value )
{
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
    polygonPreview->repaint();
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

#include <confpolygondia.moc>

