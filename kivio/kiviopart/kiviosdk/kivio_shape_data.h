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
#ifndef KIVIO_SHAPE_DATA_H
#define KIVIO_SHAPE_DATA_H

#include <qfont.h>
#include <qcolor.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qstring.h>

#include "kivio_point.h"

class KivioFillStyle;
class KivioLineStyle;
class KivioTextStyle;

class KivioTextShapeData
{
public:
    KivioTextShapeData();

    QString m_text;                                 // The text inside this shape
    QColor m_textColor;                             // The text color
    QFont m_textFont;                               // The text font
    bool m_isHtml;                                  // Is the text HTML formatted?
    int m_hTextAlign, m_vTextAlign;                 // Horizontal and vertical text alignment flags
};

/**
 * A class containing all the data for a KivioShape.
 *
 * @author Dave Marotti
 */
class KivioShapeData
{
public:
    /**
     * The possible shape types.
     */
    typedef enum {
        kstNone = 0,
        kstArc,
        kstPie,
        kstLineArray,
        kstPolyline,
        kstPolygon,
        kstBezier,
        kstRectangle,
        kstRoundRectangle,
        kstEllipse,
        kstOpenPath,
        kstClosedPath,
        kstTextBox
    } KivioShapeType;

protected:
    friend class KivioShape;

    QPtrList <KivioPoint> *m_pOriginalPointList;       // Original point list loaded from an SML file

    KivioShapeType m_shapeType;                     // The shape type
    KivioPoint m_position, m_dimensions;            // The position and dimensions
    KivioFillStyle *m_pFillStyle;                   // The fill style

//    QColor m_fgColor;                               // The foreground color
//    float m_lineWidth;                              // The line width
    KivioLineStyle *m_pLineStyle;

    QString m_name;                                 // The name of this shape

    KivioTextStyle *m_pTextData;                    // A pointer to text data (if applicable, e.g. TextBox)

public:
    KivioShapeData();
    KivioShapeData( const KivioShapeData & );
    virtual ~KivioShapeData();

    virtual void copyInto( KivioShapeData *pTarget ) const;

    virtual bool loadXML( const QDomElement & );
    virtual QDomElement saveXML( QDomDocument & );


    KivioShapeType shapeType() const { return m_shapeType; }
    void setShapeType( KivioShapeType st );

    static KivioShapeType shapeTypeFromString( const QString & );

    QPtrList<KivioPoint> *pointList() { return m_pOriginalPointList; }

    KivioFillStyle *fillStyle() const { return m_pFillStyle; }
    KivioLineStyle *lineStyle() const { return m_pLineStyle; }
    void setLineStyle(KivioLineStyle ls);


    const QString &name() const { return m_name; }
    void setName( const QString &newName ) { m_name=newName; }


    // Dimensions / Position
    KivioPoint *position() { return &m_position; }
    KivioPoint *dimensions() { return &m_dimensions; }

    double x() { return m_position.x(); }
    double y() { return m_position.y(); }
    double w() { return m_dimensions.x(); }
    double h() { return m_dimensions.y(); }



    // Text functions
    QString text();
    void setText( const QString &newText );

    bool isHtml() const;
    void setIsHtml( bool b );

    int hTextAlign() const;
    void setHTextAlign( int i );

    int vTextAlign() const;
    void setVTextAlign( int i );

    QFont textFont();
    void setTextFont( const QFont &f );

    QColor textColor();
    void setTextColor( QColor c );

    void setTextStyle( KivioTextStyle * );
};

#endif


