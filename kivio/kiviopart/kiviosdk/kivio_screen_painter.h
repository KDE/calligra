/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_SCREEN_PAINTER_H
#define KIVIO_SCREEN_PAINTER_H

#include "kivio_painter.h"

#include <qfont.h>
#include <qpainter.h>
#include <qpixmap.h>

class KivioPoint;


class KivioScreenPainter : public KivioPainter
{
protected:
    QPainter *m_pPainter;

public:
    KivioScreenPainter();
    virtual ~KivioScreenPainter();
    
    virtual bool start( QPaintDevice * );
    virtual bool stop();

    QPainter *painter() { return m_pPainter; }
    
    /*\
    |*|
    |*| PRIMITIVE IMPLEMENTATIONS
    |*|
    |*|
    \*/
    void drawLine( float, float, float, float );
    void drawArc( float, float, float, float, float, float );

    void drawBezier( QPointArray & );
    
    void drawRect( float, float, float, float );
    void fillRect( float, float, float, float );

    void drawRoundRect( float, float, float, float, float, float );
    void fillRoundRect( float, float, float, float, float, float );

    void drawPie( float, float, float, float, float, float );
    void fillPie( float, float, float, float, float, float );
    
    void drawChord( float, float, float, float, float, float );
    void fillChord( float, float, float, float, float, float );

    void drawEllipse( float, float, float, float );
    void fillEllipse( float, float, float, float );
    
    void drawLineArray( QList<KivioPoint> * );
    
    
    void drawPolyline( QList<KivioPoint> * );
    void drawPolygon( QList<KivioPoint> * );
    
    void drawPolyline( QPointArray & );
    void drawPolygon( QPointArray & );
    void drawLineArray( QPointArray & );
    
    void drawClosedPath( QList<KivioPoint> * );
    void drawOpenPath( QList<KivioPoint> * );

    void setFont( const QFont & );
    void drawText( int x, int y, int w, int h, int tf,
                const QString &str );

    void drawPixmap( float, float, const QPixmap & );    

    void drawHandle( float, float, int );
};

#endif


