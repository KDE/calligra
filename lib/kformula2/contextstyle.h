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

#ifndef CONTEXTSTYLE_H
#define CONTEXTSTYLE_H

//KDE Include
//#include <kconfig.h>

//Qt Include
#include <qcolor.h>
#include <qfont.h>
#include <qstring.h>

//Formula include


/**
 * Contains all the style information for the formela. The idea
 * is to change the values here (user configurable) and have
 * the elements paint themselves with this information.
 */
class ContextStyle
{
public:  
    enum Alignment {left, center, right};

    /**
     * Build a default context style
     */
    ContextStyle();

    /**
     * Build a context style reading settings from config
     */
    //ContextStyle(KConfig *config);

    void setResolution(double zX, double zY);
    
    QColor getDefaultColor()  const { return defaultColor; }
    QColor getNumberColor()   const { return numberColor; }
    QColor getOperatorColor() const { return operatorColor; }
    QColor getErrorColor()    const { return errorColor; }
    QColor getEmptyColor()    const { return emptyColor; }

    QFont getDefaultFont()    const { return defaultFont; }
    QFont getNameFont()       const { return nameFont; }
    QFont getNumberFont()     const { return numberFont; }
    QFont getOperatorFont()   const { return operatorFont; }
    QFont getSymbolFont()     const { return symbolFont; }

    int getDistanceX(int size) const;
    int getDistanceY(int size) const;
    int getOperatorSpace(int size) const;
    int getBaseSize() const;
    int getMinimumSize() const;
    int getSizeReduction() const;
    int getLineWidth() const;
    
    int getEmptyRectWidth() const;
    int getEmptyRectHeight() const;

    Alignment getMatrixAlignment() const { return center; }

    bool getCenterSymbol() const { return centerSymbol; }
    
private:

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
    int distance;

    /**
     * The space to be left before and after a normal operator.
     */
    int operatorSpace;
    
    /**
     * The cursors movement style. You need to notify each cursor
     * if you change this.
     */
    bool linearMovement;

    /**
     * The (font) size of the formula's main sequence.
     */
    int baseSize;

    /**
     * The smallest font size we use. Sometimes things have to be readable...
     */
    int minimumSize;

    /**
     * The amount the indexes are smaller that their parent.
     */
    int sizeReduction;

    /**
     * The thickness of our lines.
     */
    int lineWidth;

    /**
     * The little rect (square in most cases) that marks the
     * empty place where elements might be inserted.
     */
    int emptyRectWidth;
    int emptyRectHeight;

    /**
     * true means to center the symbol between its indexes.
     * false means alignment to the right.
     */
    bool centerSymbol;
    
    /**
     * All characters that are valid as exponent chars inside a number.
     * This used to be "Ee" but the symbol font has no "e" and some
     * fortran numbers know the "D", too.
     */
    // Unfortunately this is not the right place to store this
    // information. There is no ContextStyle while parsing.
    //QString exponentChars;
};

#endif // CONTEXTSTYLE_H
