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

#include "KoEnhancedPathParameter.h"
#include "KoEnhancedPathFormula.h"
#include "KoEnhancedPathShape.h"
#include <math.h>

#include <kdebug.h>

KoEnhancedPathParameter::KoEnhancedPathParameter()
{
}

KoEnhancedPathParameter::~KoEnhancedPathParameter()
{
}

double KoEnhancedPathParameter::evaluate( KoEnhancedPathShape *path )
{
    Q_UNUSED( path )
    return 0.0;
}

void KoEnhancedPathParameter::modify( double value, KoEnhancedPathShape *path )
{
    Q_UNUSED( value );
    Q_UNUSED( path );
}

KoEnhancedPathConstantParameter::KoEnhancedPathConstantParameter( double value )
: m_value( value )
{
}

double KoEnhancedPathConstantParameter::evaluate( KoEnhancedPathShape *path )
{
    Q_UNUSED( path )
    return m_value;
}

KoEnhancedPathNamedParameter::KoEnhancedPathNamedParameter( Identifier identifier )
: m_identifier( identifier )
{
}

KoEnhancedPathNamedParameter::KoEnhancedPathNamedParameter( const QString &identifier )
{
    if( identifier.isEmpty() )
        m_identifier = IdentifierUnknown;
    else if( identifier == "pi" )
        m_identifier = IdentifierPi;
    else if( identifier == "left" )
        m_identifier = IdentifierLeft;
    else if( identifier == "top" )
        m_identifier = IdentifierTop;
    else if( identifier == "right" )
        m_identifier = IdentifierRight;
    else if( identifier == "bottom" )
        m_identifier = IdentifierBottom;
    else if( identifier == "xstretch" )
        m_identifier = IdentifierXstretch;
    else if( identifier == "ystretch" )
        m_identifier = IdentifierYstretch;
    else if( identifier == "hasstroke" )
        m_identifier = IdentifierHasStroke;
    else if( identifier == "hasfill" )
        m_identifier = IdentifierHasFill;
    else if( identifier == "width" )
        m_identifier = IdentifierWidth;
    else if( identifier == "height" )
        m_identifier = IdentifierHeight;
    else if( identifier == "logwidth" )
        m_identifier = IdentifierLogwidth;
    else if( identifier == "logheight" )
        m_identifier = IdentifierLogheight;
}

double KoEnhancedPathNamedParameter::evaluate( KoEnhancedPathShape *path )
{
    switch( m_identifier )
    {
        case IdentifierPi:
            return M_PI;
        break;
        case IdentifierLeft:
            return path->boundingRect().left();
        break;
        case IdentifierTop:
            return path->boundingRect().top();
        break;
        case IdentifierRight:
            return path->boundingRect().right();
        break;
        case IdentifierBottom:
            return path->boundingRect().bottom();
        break;
        case IdentifierXstretch:
        break;
        case IdentifierYstretch:
        break;
        case IdentifierHasStroke:
            return path->border() ? 1.0 : 0.0;
        break;
        case IdentifierHasFill:
            return path->background().style() == Qt::NoBrush ? 0.0 : 1.0;
        break;
        case IdentifierWidth:
            kDebug() << "identifier width = " << path->KoShape::size().width() << endl;
            return path->KoShape::size().width();
        break;
        case IdentifierHeight:
            kDebug() << "identifier height = " << path->KoShape::size().width() << endl;
            return path->KoShape::size().height();
        break;
        case IdentifierLogwidth:
        break;
        case IdentifierLogheight:
        break;
        default:
            return 0.0;
    }
    return 0.0;
}

KoEnhancedPathReferenceParameter::KoEnhancedPathReferenceParameter( const QString &reference )
: m_reference( reference )
{
}

double KoEnhancedPathReferenceParameter::evaluate( KoEnhancedPathShape *path )
{
    return path->evaluateReference( m_reference );
}

void KoEnhancedPathReferenceParameter::modify( double value, KoEnhancedPathShape *path )
{
    path->modifyReference( m_reference, value );
}
