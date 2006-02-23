// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrPBPreview.h"

#include <qpainter.h>

#include <KoTextZoomHandler.h>
#include "KPrUtils.h"

KPrPBPreview::KPrPBPreview( QWidget* parent, const char* name, PaintType _paintType )
    : QFrame( parent, name )
{
    //FIXME zoom
    _zoomHandler=new KoTextZoomHandler();
    paintType = _paintType;
    pen = KoPen( black, 1, SolidLine );
    brush = QBrush( white, SolidPattern );
    gradient = new KPrGradient( Qt::red, Qt::green, BCT_GHORZ, false, 100, 100 );
    savedGradient = gradient;

    setFrameStyle( WinPanel | Sunken );

    switch ( paintType ) {
    case Pen:
        setFixedHeight( 40 );
        break;
    default:
        setMinimumWidth( 230 );
        break;
    }
}

void KPrPBPreview::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    if ( gradient )
    {
#if 1
        gradient->setSize( contentsRect().size() );
        repaint();
#endif
    }
}

void KPrPBPreview::drawContents( QPainter *painter )
{
    painter->save();
    painter->translate( contentsRect().x(), contentsRect().y() );

    if ( paintType == Pen ) {
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
                           colorGroup().base() );
        KoSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = int( pen.pointWidth() );

        if ( lineBegin != L_NORMAL )
            diff1 = getOffset( lineBegin, _w, + 180.0 );

        if ( lineEnd != L_NORMAL )
            diff2 = getOffset( lineEnd, _w, 0 );

        if ( lineBegin != L_NORMAL )
            drawFigure( lineBegin, painter, KoPoint( 5, _zoomHandler->unzoomItY( contentsRect().height() ) / 2 ),
                        pen.color(), _w, 180.0, _zoomHandler );

        if ( lineEnd != L_NORMAL )
            drawFigure( lineEnd, painter, KoPoint( _zoomHandler->unzoomItX( contentsRect().width() - 7 ) ,
                                                   _zoomHandler->unzoomItY( contentsRect().height() ) / 2 ),
                        pen.color(), _w, 0.0, _zoomHandler );

        painter->setPen( pen.zoomedPen( _zoomHandler ) );
        painter->drawLine( - (int)diff1.width() + 8,
                           contentsRect().height() / 2,
                           contentsRect().width() - (int)diff2.width() - 7,
                           contentsRect().height() / 2 );

    } else if ( paintType == Brush ) {
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
                           colorGroup().base() );
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(), brush );
    } else if ( paintType == Gradient )
        painter->drawPixmap( 0, 0, gradient->pixmap());

    painter->restore();
}

KPrPBPreview::~KPrPBPreview()
{
    delete _zoomHandler;
    delete savedGradient;
}


void KPrPBPreview::setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                             bool _unbalanced, int _xfactor, int _yfactor )
{
    gradient->setColor1( _c1 );
    gradient->setColor2( _c2 );
    gradient->setBackColorType( _t );
    gradient->setUnbalanced( _unbalanced );
    gradient->setXFactor( _xfactor );
    gradient->setYFactor( _yfactor );
    repaint( false );
}


#include "KPrPBPreview.moc"
