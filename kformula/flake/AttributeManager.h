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
class BasicElement;

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

    /**
     * Obtain the @p attribute's value as color
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    QColor colorOf( const QString& attribute, BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as boolean
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    bool boolOf( const QString& attribute, const BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as double
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    double doubleOf( const QString& attribute, const BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as list of doubles
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    QList<double> doubleListOf( const QString& attribute,
                                const BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as string
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    QString stringOf( const QString& attribute, BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as align
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    Align alignOf( const QString& attribute, BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as list of aligns
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    QList<Align> alignListOf( const QString& attribute, BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as Qt::PenStyle
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The value that was looked up
     */
    Qt::PenStyle penStyleOf( const QString& attribute, BasicElement* element ) const;

    /**
     * Obtain the @p attribute's value as list of Qt::PenStyles
     * @param attribute A string with the attribute to look up
     * @param element The element the value is looked up for
     * @return The valuefont  that was looked up
     */
    QList<Qt::PenStyle> penStyleListOf( const QString& attribute,
                                        BasicElement* element ) const;

    /**
     * Obtain @p element's scaling factor based on the script level
     * @param element The element which scaling is determined
     * @return The scaling factor
     */
    double scriptLevelScaling( const BasicElement* element ) const;

    /// @return A value used for spacing tasks during layouting
    double layoutSpacing( const BasicElement* element ) const;

    /**
     * Determine the maximal height of an given element's child elements
     * @param element The element whos children are used
     * @return The maximal height
     */
    double maxHeightOfChildren( BasicElement* element ) const;

    /**
     * Determine the maximal height of an given element's child elements
     * @param element The element whos children are used
     * @return The maximal height
     */
    double maxWidthOfChildren( BasicElement* element ) const;

    /// @return The Align value that was passed as QString @p value
    Align parseAlign( const QString& value ) const;

    /// @return The font that is set for @p element 
    QFont font( const BasicElement* element ) const;

    /// Set the KoViewConverter to use
    void setViewConverter( KoViewConverter* converter );

private:
    /// @return The parsed the @p value into a Qt::PenStyle
    Qt::PenStyle parsePenStyle( const QString& value ) const;

    /// @return The parsed @p value which is given with a unit
    double parseUnit( const QString& value, const BasicElement* element ) const;

    /// Find a value for @p attribute that applies to @p element
    QString findValue( const QString& attribute, const BasicElement* element ) const;

    /// The KoViewConverter used to determine the point values of pixels
    KoViewConverter* m_viewConverter;
};

#endif // ATTRIBUTEMANAGER_H
