/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net> 

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
   Boston, MA 02110-1301, USA.
*/

#ifndef ATTRIBUTEMANAGER_H
#define ATTRIBUTEMANAGER_H

#include <QFont>
#include "kformula_export.h"
class KoViewConverter;
class QPaintDevice;

class BasicElement;
	
enum MathVariant {
    Normal,
    Bold,
    Italic,
    BoldItalic,
    DoubleStruck,
    BoldFraktur,
    Script,
    BoldScript,
    Fraktur,
    SansSerif,
    BoldSansSerif,
    SansSerifItalic,
    SansSerifBoldItalic,
    Monospace,
    InvalidMathVariant
};

enum NamedSpaces { 
    NegativeVeryVeryThinMathSpace,
    NegativeVeryThinMathSpace,
    NegativeThinMathSpace,
    NegativeMediumMathSpace,
    NegativeThickMathSpace,
    NegativeVeryThickMathSpace,
    NegativeVeryVeryThickMathSpace,
    VeryVeryThinMathSpace,
    VeryThinMathSpace,
    ThinMathSpace,
    MediumMathSpace,
    ThickMathSpace,
    VeryThickMathSpace,
    VeryVeryThickMathSpace,
    InvalidNameSpace
};

/** Enum encoding all possibilities to align */
enum Align {
    Left /**< Align to the left*/,
    Center /**< Align to the center*/,
    Right /**< Align to the right*/,
    Top /**< Align to the top*/,
    Bottom /**< Align to the bottom*/,
    BaseLine /**< Align to the baseline*/,
    Axis /**< Align to the axis*/,
    InvalidAlign
};

/** Enum encoding all states of  mo's form attribute */
enum Form {
    Prefix /**< mo is a prefix*/,
    Infix /**< mo is a infix - used for all cases where it's not prefix or postfix*/,
    Postfix /**< mo is a postfix*/,
    InvalidForm
};

/** Enum encoding all states of mspace's linebreak attribute */
enum LineBreak {
    Auto /**< Renderer should use default linebreaking algorithm*/,
    NewLine /**< Start a new line and do not indent*/,
    IndentingNewLine /**< Start a new line and do indent*/,
    NoBreak /**< Do not allow a linebreak here*/,
    GoodBreak /**< If a linebreak is needed on the line, here is a good spot*/,
    BadBreak /**< If a linebreak is needed on the line, try to avoid breaking here*/,
    InvalidLineBreak
};

/**
 * @short manages all the attributes, used by the elements to obtain attribute values
 *
 * The AttributeManager is the central point dealing with the MathML attributes and
 * their values. It is in fact something like a StyleManager. As the normal elements
 * only have a general hash list of the elements they hold, there is the need for a
 * class that manages conversion and heritage of the attributes during the painting
 * phase. These are the two main tasks of AttributeManager.
 * The AttributeManager is always called when an element needs a value to work with.
 * The AttributeManager looks for that value first in the asking element's own
 * attribute list. If nothing is found the AttributeManager looks for a parent element
 * that might inherit the right value. If AttributeManager has no success again it
 * returns the default value for that attribute.
 *
 * @author Martin Pfeiffer <hubipete@gmx.net>
 */
class KOFORMULA_EXPORT AttributeManager {
public:
    /// The constructor
    AttributeManager();

    /// The destructor
    ~AttributeManager();

    /// @return The MathVariant value for @p element
    MathVariant mathVariant( BasicElement* element );

    /// @return The foreground color for @p element
    QColor mathColor( BasicElement* element );

    /// @return The background color for @p element
    QColor mathBackground( BasicElement* element );

    /// @return The value of the mathSize attribute for @p element
    double mathSize( BasicElement* element );

    /// @return The current font style - just a dummy atm
    QFont font( const BasicElement* element ) const;

    /// @return Obtain the current scriptlevel
    int scriptLevel( BasicElement* element );
 
    /// @return Obtain the current displaystyle
    bool displayStyle( BasicElement* element ) const;

    /**
     * Obtain a value for attribute
     * @param attribute A string with the attribute to look up
     * @return QVariant with the value
     */
    QString stringOf( const QString& attribute, BasicElement* element ) const;

    Align alignOf( const QString& attribute, BasicElement* element ) const;

    QList<Align> alignListOf( const QString& attribute, BasicElement* element ) const;

    bool boolOf( const QString& attribute, const BasicElement* element ) const;

    double doubleOf( const QString& attribute, BasicElement* element ) const;

    int intOf( const QString& attribute, BasicElement* element ) const;
/*
    Qt::PenStyle lineOf( const QString& attribute, const BasicElement* element );

    QList< Qt::PenStyle > lineListOf( const QString attribute,
                                      const BasicElement* element );
*/
    /// @return Obtain the value 
    double mathSpaceValue( const QString& value ) const;

    /// Set the KoViewConverter to use
    void setViewConverter( KoViewConverter* converter );

protected:
    /// Find a value for @p attribute that applies to @p element
    QString findValue( const QString& attribute, const BasicElement* element ) const;

    /// @return The Form value that was passed as QString @p value
    Form parseForm( const QString& value ) const;

    /// @return The Align value that was passed as QString @p value
    Align parseAlign( const QString& value ) const;

    /// Parse the given @p element according to its affect on the scriptlevel
    void parseScriptLevel( BasicElement* element );

    /**
     * Calculates the pt values of a passes em or ex value
     * @param value The em or ex value to be used for calculation
     * @param isEm Indicates whether to calculate an ex or em value
     * @return The calculated pt value
     */
    double calculateEmExUnits( double value, bool isEm ) const;

private:
    /// The current BasicElement that asks for a value
    mutable BasicElement const* m_currentElement;

    /// The last calculated scriptLevel
    int m_cachedScriptLevel;

    /// The KoViewConverter used to determine the point values of pixels
    KoViewConverter* m_viewConverter;

    /// The QPaintDevice we are currently painting on - needed for em/ ex units
    QPaintDevice* m_paintDevice;
};

#endif // ATTRIBUTEMANAGER_H
