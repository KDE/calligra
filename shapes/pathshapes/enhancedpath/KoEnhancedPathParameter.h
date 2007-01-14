/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOENHANCEDPATHPARAMETER_H
#define KOENHANCEDPATHPARAMETER_H

#include <QString>

class KoEnhancedPathShape;
class KoEnhancedPathFormula;

/// the different possible identifiers, taken from the odf spec
enum Identifier {
    IdentifierUnknown,   ///< unknown identifier
    IdentifierPi,        ///< value of pi.
    IdentifierLeft,      ///< left of svg:viewBox or draw:coordinate-origin-x
    IdentifierTop,       ///< top of svg:viewBox or draw:coordinate-origin-y
    IdentifierRight,     ///< right of svg:viewBox or draw:coordinate-origin-x + draw:coordinate-width
    IdentifierBottom,    ///< bottom of svg:viewBox or draw:coordinate-origin-y + draw:coordinate-height
    IdentifierXstretch,  ///< The value of draw:path-stretchpoint-x is used.
    IdentifierYstretch,  ///< The value of draw:path-stretchpoint-y is used.
    IdentifierHasStroke, ///< If the shape has a line style, a value of 1 is used.
    IdentifierHasFill,   ///< If the shape has a fill style, a value of 1 is used.
    IdentifierWidth,     ///< The width of the svg:viewBox is used.
    IdentifierHeight,    ///< The height of the svg:viewBox is used.
    IdentifierLogwidth,  ///< The width of the svg:viewBox in 1/100th mm is used.
    IdentifierLogheight  ///< The height of the svg:viewBox in 1/100th mm is used.
};

/// abstract parameter class
class KoEnhancedPathParameter
{
public:
    KoEnhancedPathParameter();
    virtual ~KoEnhancedPathParameter();
    /// evaluates the parameter using the given path
    virtual double evaluate( KoEnhancedPathShape *path ) = 0;
    /// modifies the parameter if possible, using the new value
    virtual void modify( double value, KoEnhancedPathShape *path );
};

/// a constant parameter
class KoEnhancedPathConstantParameter : public KoEnhancedPathParameter
{
public:
    KoEnhancedPathConstantParameter( double value );
    double evaluate( KoEnhancedPathShape *path );
private:
    double m_value;
};

/// a named parameter
class KoEnhancedPathNamedParameter : public KoEnhancedPathParameter
{
public:
    KoEnhancedPathNamedParameter( Identifier identifier );
    KoEnhancedPathNamedParameter( const QString &identifier );
    double evaluate( KoEnhancedPathShape *path );
private:
    Identifier m_identifier;
};

/// a referencing parameter
class KoEnhancedPathReferenceParameter : public KoEnhancedPathParameter
{
public:
    KoEnhancedPathReferenceParameter( const QString &reference );
    double evaluate( KoEnhancedPathShape *path );
    virtual void modify( double value, KoEnhancedPathShape *path );
private:
    QString m_reference;
};

#endif // KOENHANCEDPATHPARAMETER_H
