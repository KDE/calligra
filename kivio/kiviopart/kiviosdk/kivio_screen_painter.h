/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
    float m_transX, m_transY;
    int m_rotation;

public:
    KivioScreenPainter();
    KivioScreenPainter(QPainter* painter);
    virtual ~KivioScreenPainter();

    virtual bool start( QPaintDevice * );
    virtual bool stop();

    QPainter *painter() { return m_pPainter; }
    void setPainter(QPainter* p) { m_pPainter = p; }

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

    void drawLineArray( QPtrList<KivioPoint> * );


    void drawPolyline( QPtrList<KivioPoint> * );
    void drawPolygon( QPtrList<KivioPoint> * );

    void drawPolyline( QPointArray & );
    void drawPolygon( QPointArray & );
    void drawLineArray( QPointArray & );

    void drawClosedPath( QPtrList<KivioPoint> * );
    void drawOpenPath( QPtrList<KivioPoint> * );

    void setFont( const QFont & );
    void drawText( int x, int y, int w, int h, int tf,
                const QString &str );
    virtual QRect boundingRect( int, int, int, int, int, const QString & );

    void drawPixmap( float, float, const QPixmap & );

    void drawHandle( float, float, int );
    virtual void drawSelectionBox( const QRect& );

    virtual void saveState();
    virtual void restoreState();
    virtual void setTranslation(float, float);
    virtual void translateBy(float, float);
    virtual void setRotation(int);
    virtual void rotateBy(int);
    virtual int rotation();

    virtual void setWorldMatrix(QWMatrix, bool);
};

#endif


