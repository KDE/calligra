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
#ifndef KIVIO_FILL_STYLE_H
#define KIVIO_FILL_STYLE_H

#include <qbrush.h>
#include <qcolor.h>
#include <qdom.h>

#include <kimageeffect.h>

class KivioFillStyle
{
public:
    /*
     * kcsNone - No fill
     * kcsSolid - Solid fill
     * kcsGradient - Gradient fill
     * kcsPixmap - Pixmap fill
     */
    typedef enum {
        kcsNone = 0,
        kcsSolid,
        kcsGradient,
        kcsPixmap
    } KivioColorStyle;

protected:
    KivioColorStyle m_colorStyle;       // The color style to use when filling
    QColor m_color;                     // The color to use when solid filling
    QColor m_color2;                     // The second color to use when using a gradient
    QBrush::BrushStyle m_brushStyle;    // The brush pattern to use when solid filling (maybe gradient too?)
    KImageEffect::GradientType m_gradientType;  //Which type of gradient to use

public:
    KivioFillStyle();
    KivioFillStyle( const KivioFillStyle & );


    void copyInto( KivioFillStyle *pTarget ) const;

    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );


    inline KivioColorStyle colorStyle() const { return m_colorStyle; }
    inline void setKivioColorStyle( KivioColorStyle k ) { m_colorStyle = k; }


    inline QColor color() const { return m_color; }
    inline void setColor( const QColor& c ) { m_color = c; }
    inline QColor color2() const { return m_color2; }
    inline void setColor2( const QColor& c ) { m_color2 = c; }

    inline QBrush::BrushStyle brushStyle() const { return m_brushStyle; }
    inline void setBrushStyle( QBrush::BrushStyle b ) { m_brushStyle = b; }

    inline KImageEffect::GradientType gradientType() const { return m_gradientType; }
    inline void setGradientType(KImageEffect::GradientType t) { m_gradientType = t; }

    QBrush brush();
};

#endif


