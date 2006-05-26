// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPolygonPreview.h"

#include <qpainter.h>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3PointArray>

#include <math.h>

KPrPolygonPreview::KPrPolygonPreview( QWidget* parent, const char* name)
    : Q3Frame( parent, name )
{
    setFrameStyle( WinPanel | Sunken );
    setBackgroundColor( Qt::white );

    setMinimumSize( 200, 100 );
}


void KPrPolygonPreview::drawContents( QPainter *painter )
{
    double angle = 2 * M_PI / nCorners;
    double diameter = static_cast<double>( qMax( width(), height() ) - 10 );
    double radius = diameter * 0.5;

    painter->setWindow( qRound( -radius ), qRound( -radius ), qRound( diameter ), qRound( diameter ) );
    painter->setViewport( 5, 5, width() - 10, height() - 10 );
    painter->setPen( pen );
    painter->setBrush( brush );

    Q3PointArray points( isConcave ? nCorners * 2 : nCorners );
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


void KPrPolygonPreview::slotConvexConcave( bool convexConcave )
{
    isConcave = convexConcave;
    repaint();
}


void KPrPolygonPreview::slotConvexPolygon()
{
    isConcave = false;
    repaint();
}


void KPrPolygonPreview::slotConcavePolygon()
{
    isConcave = true;
    repaint();
}


void KPrPolygonPreview::slotCornersValue( int value )
{
    nCorners = value;
    repaint();
}


void KPrPolygonPreview::slotSharpnessValue( int value )
{
    sharpness = value;
    repaint();
}


#include "KPrPolygonPreview.moc"
