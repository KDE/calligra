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

#ifndef __CONTEXTSTYLE_H
#define __CONTEXTSTYLE_H

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

    int getDistance() const { return distance; }
    int getOperatorSpace(int /*size*/) const { return operatorSpace; }
    int getBaseSize() const { return baseSize; }
    int getMinimumSize() const { return minimumSize; }
    int getSizeReduction() const { return sizeReduction; }

    Alignment getMatrixAlignment() const { return center; }
    
private:
    
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
     * All characters that are valid as exponent chars inside a number.
     * This used to be "Ee" but the symbol font has no "e" and some
     * fortran numbers know the "D", too.
     */
    // Unfortunately this is not the right place to store this
    // information. There is no ContextStyle while parsing.
    //QString exponentChars;
};

#endif // __CONTEXTSTYLE_H
