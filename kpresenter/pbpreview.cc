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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "pbpreview.h"

#include <qpainter.h>

#include <kozoomhandler.h>
#include "kpgradient.h"
#include "kpresenter_utils.h"

PBPreview::PBPreview( QWidget* parent, const char* name, PaintType _paintType )
    : QFrame( parent, name )
{
    //FIXME zoom
    _zoomHandler=new KoZoomHandler();
    paintType = _paintType;
    pen = QPen( black, 1, SolidLine );
    brush = QBrush( white, SolidPattern );
    gradient = 0;

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

void PBPreview::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    if ( gradient )
    {
        gradient->setSize( contentsRect().size() );
        repaint();
    }
}

void PBPreview::drawContents( QPainter *painter )
{
    painter->save();
    painter->translate( contentsRect().x(), contentsRect().y() );

    if ( paintType == Pen ) {
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
                           colorGroup().base() );
        KoSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

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

        painter->setPen( pen );
        painter->drawLine( - (int)diff1.width() + 8,
                           contentsRect().height() / 2,
                           contentsRect().width() - (int)diff2.width() - 7,
                           contentsRect().height() / 2 );

    } else if ( paintType == Brush ) {
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(),
                           colorGroup().base() );
        painter->fillRect( 0, 0, contentsRect().width(), contentsRect().height(), brush );
    } else if ( paintType == Gradient && gradient )
        painter->drawPixmap( 0, 0, gradient->pixmap());

    painter->restore();
}

PBPreview::~PBPreview()
{
    delete _zoomHandler;
}


