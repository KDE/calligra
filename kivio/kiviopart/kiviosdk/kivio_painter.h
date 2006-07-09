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
#ifndef KIVIO_PAINTER_H
#define KIVIO_PAINTER_H

#include <qfont.h>
#include <qptrlist.h>
#include <qpaintdevice.h>
#include <qpixmap.h>
#include <qpointarray.h>
#include <qwmatrix.h>

#include "kivio_fill_style.h"

class KivioPoint;
class KivioFillStyle;
class KivioLineStyle;

class KivioPainter
{
protected:
    KivioFillStyle *m_pFillStyle;
    KivioLineStyle *m_pLineStyle;

    QColor m_textColor;

public:
    KivioPainter();
    virtual ~KivioPainter();

    virtual bool start( QPaintDevice * ) { return false;}
    virtual bool stop() { return false;}


    /*\
    |*|
    |*|
    |*| PROPERTY SETTINGS
    |*|
    |*| (probably don't need to reimplement -- unless you are optimizing)
    \*/
    virtual float lineWidth() const;
    virtual void setLineWidth( const float &f );

    // virtual void setClipRect( QRect * );
    // virtual QRect *clipRect();

    virtual QColor fgColor() const;
    virtual void setFGColor( const QColor &c );

    virtual QColor bgColor() const { return m_pFillStyle->color(); }
    virtual void setBGColor( const QColor &c ) { m_pFillStyle->setColor(c); }

    virtual QColor textColor() const { return m_textColor; }
    virtual void setTextColor( const QColor &c ) { m_textColor = c; }

    virtual KivioFillStyle *fillStyle() { return m_pFillStyle; }

    virtual void setLineStyle( KivioLineStyle * );
    virtual void setFillStyle( KivioFillStyle * );

    /*\
    |*|
    |*|
    |*| PRIMITIVE FUNCTIONS
    |*|
    |*| (all descendants *must* reimplement these)
    \*/
    virtual void drawLine( float, float, float, float ) {;}
    virtual void drawArc( float, float, float, float, float, float ) {;}

    virtual void drawBezier( QPtrList<KivioPoint> * ) {;}
    virtual void drawBezier( QPointArray & ) {;}

    virtual void drawRect( float, float, float, float ) {;}
    virtual void fillRect( float, float, float, float ) {;}

    virtual void drawRoundRect( float, float, float, float, float, float ) {;}
    virtual void fillRoundRect( float, float, float, float, float, float ) {;}

    virtual void drawEllipse( float, float, float, float ) {;}
    virtual void fillEllipse( float, float, float, float ) {;}

    virtual void drawPie( float, float, float, float, float, float ) {;}
    virtual void fillPie( float, float, float, float, float, float ) {;}

    virtual void drawChord( float, float, float, float, float, float ) {;}
    virtual void fillChord( float, float, float, float, float, float ) {;}

    virtual void drawOpenPath( QPtrList<KivioPoint> * ) {;}
    virtual void drawClosedPath( QPtrList<KivioPoint> * ) {;}

    virtual void drawLineArray( QPtrList<KivioPoint> * ) {;}
    virtual void drawPolyline( QPtrList<KivioPoint> * ) {;}
    virtual void drawPolygon( QPtrList<KivioPoint> * ) {;}

    virtual void drawLineArray( QPointArray & ) {;}
    virtual void drawPolyline( QPointArray & ) {;}
    virtual void drawPolygon( QPointArray & ) {;}

    virtual void setFont( const QFont & ) {;}
    virtual void drawText( int, int, int, int, int, const QString & ) {;}
    virtual QRect boundingRect( int, int, int, int, int, const QString & ) { return QRect(0,0,100,100); }


    // These shouldn't be used for anything except the screen painter right now...
    virtual void drawPixmap( float, float, const QPixmap & ) {;}

    /*\
    |*| Miscellaneous Functions
    |*| note: These don't need to be implemented by everything
    \*/
    // connector point flags
    enum {
       cpfConnectable=0x1,
       cpfStart=0x2,
       cpfEnd=0x4,
       cpfConnected=0x8,
       cpfLock=0x10
    };
    virtual void drawHandle( float, float, int ) { ; }
    virtual void drawSelectionBox( const QRect& ) { ; }

    virtual void saveState() {;}
    virtual void restoreState() {;}
    virtual void setTranslation(float, float) {;}
    virtual void translateBy(float, float) {;}
    virtual void setRotation(int) {;}
    virtual void rotateBy(int) {;}
    virtual int rotation() { return 0; }

    virtual void setWorldMatrix(QWMatrix, bool) {;}
};

#endif


