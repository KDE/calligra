/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#ifndef CONORDINARYSTYLE_H
#define CONORDINARYSTYLE_H

//KDE Include
//#include <kconfig.h>

//Qt Include
#include <qcolor.h>
#include <qfont.h>
#include <qstring.h>

//Formula include
#include "formuladefs.h"

KFORMULA_NAMESPACE_BEGIN

/**
 * Contains all the style information for the formela. The idea
 * is to change the values here (user configurable) and have
 * the elements paint themselves with this information.
 *
 * All distances are stored in point. Most methods return pixel
 * values.
 */
class ContextStyle
{
public:

    enum Alignment { left, center, right };

    /**
     * Textstyles like in TeX. In the remaining documentation, the
     * styles are abbreviated like this:
     *
     * displayStyle: D
     *
     * textStyle: T
     *
     * scriptStyle: S
     *
     * scriptScriptStyle: SS
     **/
    enum TextStyle {
        displayStyle = 0,
        textStyle = 1,
        scriptStyle = 2,
        scriptScriptStyle = 3
    };

    enum IndexStyle {normal, cramped};

    /**
     * Build a default context style
     */
    ContextStyle();

    /**
     * Build a context style reading settings from config
     */
    //ContextStyle(KConfig *config);


    double getXResolution() const { return m_zoomedResolutionX; }
    double getYResolution() const { return m_zoomedResolutionY; }

    void setResolution(double zX, double zY);

    bool getSyntaxHighlighting() { return syntaxHighlighting; }
    void setSyntaxHighlighting(bool highlight) { syntaxHighlighting = highlight; }

    QColor getDefaultColor()  const { return defaultColor; }
    QColor getNumberColor()   const;
    QColor getOperatorColor() const;
    QColor getErrorColor()    const;
    QColor getEmptyColor()    const;

    QFont getDefaultFont()    const { return defaultFont; }
    QFont getNameFont()       const { return nameFont; }
    QFont getNumberFont()     const { return numberFont; }
    QFont getOperatorFont()   const { return operatorFont; }
    QFont getSymbolFont()     const { return symbolFont; }


    double getReductionFactor( TextStyle tstyle ) const;

    double getBaseSize() const;
    /** Sets the base size as point value. Unzoomed. */
    void setBaseSize( int pointSize );

    TextStyle getBaseTextStyle() const { return m_baseTextStyle; }
    bool isScript( TextStyle tstyle ) const { return ( tstyle == scriptStyle ) ||
                                                     ( tstyle == scriptScriptStyle ); }

    /**
     * TeX like spacings. Zoomed.
     */
    double getThinSpace( TextStyle tstyle ) const;
    double getMediumSpace( TextStyle tstyle ) const;
    double getThickSpace( TextStyle tstyle ) const;

    /**
     * Calculates the font size corresponding to the given TextStyle.
     *
     * Takes into account the current zoom factor.
     **/
    double getAdjustedSize( TextStyle tstyle ) const;

    /**
     * All simple lines like the one that makes up a fraction.
     */
    double getLineWidth() const;

    double getEmptyRectWidth() const;
    double getEmptyRectHeight() const;

    Alignment getMatrixAlignment() const { return center; }

    bool getCenterSymbol() const { return centerSymbol; }

    /**
     * Font-conversions a la TeX.
     *
     * For fractions (and also matrices), we have the following conversions:
     * D->T, T->S, S,SS->SS
     */
    TextStyle convertTextStyleFraction( TextStyle tstyle ) const;

    /**
     * Font-conversions a la TeX.
     *
     * For indices, we have the following conversions:
     * D->S, T->S, S,SS->SS
     */
    TextStyle convertTextStyleIndex( TextStyle tstyle ) const;

    /**
     * Index-style-conversions a la TeX.
     *
     * The function convertIndexStyleUpper is responsible for everything
     * that ends 'up', like nominators of fractions, or upper indices.
     *
     * We have the following rule:
     * normal->normal, cramped->cramped
     */
    IndexStyle convertIndexStyleUpper( IndexStyle istyle ) const {
	return istyle; }


    /**
     * Index-style-conversions a la TeX.
     *
     * The function convertIndexStyleLower is responsible for everything
     * that ends 'down', like nominators of fractions, or upper indices.
     *
     * We have the following rule:
     * normal->cramped, cramped->cramped
     */
    IndexStyle convertIndexStyleLower( IndexStyle /*istyle*/ ) const {
	return cramped; }


    // Shamelessly stolen from kword. (See kwdoc.h)

    // Input: pt. Output: pixels. Resolution and zoom are applied.
    int zoomItX( int z ) const {
        return static_cast<int>(m_zoomedResolutionX * z);
    }
    unsigned int zoomItX( unsigned int z ) const {
        return static_cast<unsigned int>(m_zoomedResolutionX * z);
    }
    double zoomItX( double z ) const {
        return m_zoomedResolutionX * z;
    }
    int zoomItY( int z ) const {
        return static_cast<int>(m_zoomedResolutionY * z);
    }
    unsigned int zoomItY( unsigned int z ) const {
        return static_cast<unsigned int>(m_zoomedResolutionY * z);
    }
    double zoomItY( double z ) const {
        return m_zoomedResolutionY * z;
    }

    double unzoomItX( double z ) const {
        return z / m_zoomedResolutionX;
    }
    double unzoomItY( double z ) const {
        return z / m_zoomedResolutionY;
    }

private:

    void setup();

    struct TextStyleValues {

        void setup( QFont font, double baseSize, double reduction );

        double thinSpace() const   { return static_cast<double>( quad )/6.; }
        double mediumSpace() const { return static_cast<double>( quad )*2./9.; }
        double thickSpace() const  { return static_cast<double>( quad )*5./18.; }

        double reductionFactor;
        int quad;
    };

    TextStyleValues textStyleValues[ 4 ];

    /**
     * The resolution in pixel/point*zoom.
     */
    double m_zoomedResolutionX;
    double m_zoomedResolutionY;

    QFont defaultFont;
    QFont nameFont;
    QFont numberFont;
    QFont operatorFont;
    QFont symbolFont;

    QColor defaultColor;
    QColor numberColor;
    QColor operatorColor;
    QColor errorColor;
    QColor emptyColor;

    /**
     * The basic distance. Used everywhere a non specific
     * distance is needed. (Maybe we should think about this.)
     */
    //double distance;

    /**
     * The space to be left before and after a normal operator.
     */
    //double operatorSpace;

    /**
     * The cursors movement style. You need to notify each cursor
     * if you change this.
     */
    bool linearMovement;

    /**
     * The (font) size of the formula's main sequence.
     */
    double baseSize;

    /**
     * The base text style of the formula.
     **/
    TextStyle m_baseTextStyle;

    /**
     * The thickness of our lines.
     */
    double lineWidth;

    /**
     * The little rect (square in most cases) that marks the
     * empty place where elements might be inserted.
     */
    double emptyRectWidth;
    double emptyRectHeight;

    /**
     * true means to center the symbol between its indexes.
     * false means alignment to the right.
     */
    bool centerSymbol;

    /**
     * Whether we want coloured formulae.
     */
    bool syntaxHighlighting;
};

KFORMULA_NAMESPACE_END

#endif // CONORDINARYSTYLE_H
