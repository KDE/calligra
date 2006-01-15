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

#ifndef PBPREVIEW_H
#define PBPREVIEW_H

#include "global.h"

#include <qframe.h>

#include "KPrGradient.h"
#include <KoBrush.h>
#include <KoPen.h>

class KoTextZoomHandler;

/******************************************************************/
/* class Pen and Brush preview                                    */
/******************************************************************/

class KPrPBPreview : public QFrame
{
    Q_OBJECT

public:
    enum PaintType {
        Pen,
        Brush,
        Gradient
    };

    KPrPBPreview( QWidget* parent, const char* name, PaintType _paintType = Pen );
    ~KPrPBPreview();
    void setPen( const KoPen &_pen ) { pen = _pen; repaint( true ); }
    void setBrush( const QBrush &_brush ) { brush = _brush; repaint( true ); }
    void setLineBegin( LineEnd lb ) { lineBegin = lb; repaint( true ); }
    void setLineEnd( LineEnd le ) { lineEnd = le; repaint( true ); }
    void setGradient( KPrGradient *g ) { if ( g ) { gradient = g; } repaint( true ); }
    void setPaintType( PaintType pt ) { paintType = pt; repaint(true); }

    void setGradient( const QColor &_c1, const QColor &_c2, BCType _t,
                      bool _unbalanced, int _xfactor, int _yfactor );
    void setColor1( const QColor &_color ) { gradient->setColor1( _color ); repaint( false ); }
    void setColor2( const QColor &_color ) { gradient->setColor2( _color ); repaint( false ); }
    void setBackColorType( BCType _type ) { gradient->setBackColorType( _type ); repaint( false ); }
    void setUnbalanced( bool b ) { gradient->setUnbalanced( b ); repaint( false ); }
    void setXFactor( int i ) { gradient->setXFactor( i ); repaint( false ); }
    void setYFactor( int i ) { gradient->setYFactor( i ); repaint( false ); }

protected:
    void drawContents( QPainter *p );
    void resizeEvent( QResizeEvent *e );

private:
    PaintType paintType;
    KoPen pen;
    QBrush brush;
    LineEnd lineBegin, lineEnd;
    KPrGradient *gradient;
    KPrGradient *savedGradient;
    KoTextZoomHandler *_zoomHandler;
};


#endif  /* PBPREVIEW_H */
