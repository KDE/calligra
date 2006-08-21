/* This file is part of the KDE project
   Copyright (C) 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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
 * Boston, MA 02110-1301, USA.
*/

#include <qpainter.h>

#include "elementtype.h"
#include "sequenceelement.h"
#include "textelement.h"
#include "fontstyle.h"
#include "operatorelement.h"

KFORMULA_NAMESPACE_BEGIN

OperatorElement::OperatorElement( BasicElement* parent ) : TokenElement( parent ),
                                                           m_customForm( false ),
                                                           m_customFence( false ),
                                                           m_customSeparator( false ),
                                                           m_customLSpace( false ),
                                                           m_customRSpace( false ),
                                                           m_customStretchy( false ),
                                                           m_customSymmetric( false ),
                                                           m_customMaxSize( false ),
                                                           m_customMinSize( false ),
                                                           m_customLargeOp( false ),
                                                           m_customMovableLimits( false ),
                                                           m_customAccent( false )
{
}

bool OperatorElement::readAttributesFromMathMLDom( const QDomElement &element )
{
    if ( ! BasicElement::readAttributesFromMathMLDom( element ) ) {
        return false;
    }

    QString formStr = element.attribute( "form" ).stripWhiteSpace().lower();
    if ( ! formStr.isNull() ) {
        m_customForm = true;
        if ( formStr == "prefix" ) {
            m_form = PrefixForm;
        }
        else if ( formStr == "infix" ) {
            m_form = InfixForm;
        }
        else if ( formStr == "postfix" ) {
            m_form = PostfixForm;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `form': " << formStr << endl;
            m_customForm = false;
        }
    }
    QString fenceStr = element.attribute( "fence" ).stripWhiteSpace().lower();
    if ( ! fenceStr.isNull() ) {
        m_customFence = true;
        if ( fenceStr == "true" ) {
            m_fence = true;
        }
        else if ( fenceStr == "false" ) {
            m_fence = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `fence': " << fenceStr << endl;
            m_customFence = false;
        }
    }
    QString separatorStr = element.attribute( "separator" ).stripWhiteSpace().lower();
    if ( ! separatorStr.isNull() ) {
        m_customSeparator = true;
        if ( separatorStr == "true" ) {
            m_separator = true;
        }
        else if ( separatorStr == "false" ) {
            m_separator = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `separator': " << separatorStr << endl;
            m_customSeparator = false;
        }
    }
    QString lspaceStr = element.attribute( "lspace" ).stripWhiteSpace().lower();
    if ( ! lspaceStr.isNull() ) {
        m_customLSpace = true;
        m_lspace = getSize( lspaceStr, &m_lspaceType );
        if ( m_lspaceType == NoSize ) {
            m_lspaceType = getSpace( lspaceStr );
        }
    }
    QString rspaceStr = element.attribute( "rspace" ).stripWhiteSpace().lower();
    if ( ! rspaceStr.isNull() ) {
        m_customRSpace = true;
        m_rspace = getSize( rspaceStr, &m_rspaceType );
        if ( m_rspaceType == NoSize ) {
            m_rspaceType = getSpace( rspaceStr );
        }
    }
    QString stretchyStr = element.attribute( "stretchy" ).stripWhiteSpace().lower();
    if ( ! stretchyStr.isNull() ) {
        m_customStretchy = true;
        if ( stretchyStr == "true" ) {
            m_stretchy = true;
        }
        else if ( stretchyStr == "false" ) {
            m_stretchy = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `stretchy': " << stretchyStr << endl;
            m_customStretchy = false;
        }
    }
    QString symmetricStr = element.attribute( "symmetric" ).stripWhiteSpace().lower();
    if ( ! symmetricStr.isNull() ) {
        m_customSymmetric = true;
        if ( symmetricStr == "true" ) {
            m_symmetric = true;
        }
        else if ( symmetricStr == "false" ) {
            m_symmetric = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `symmetric': " << symmetricStr << endl;
            m_customSymmetric = false;
        }
    }
    QString maxsizeStr = element.attribute( "maxsize" ).stripWhiteSpace().lower();
    if ( ! maxsizeStr.isNull() ) {
        m_customMaxSize = true;
        if ( maxsizeStr == "infinity" ) {
            m_maxSizeType = InfinitySize;
        }
        else {
            m_maxSize = getSize( maxsizeStr, &m_maxSizeType );
            if ( m_maxSizeType == NoSize ) {
                m_maxSizeType = getSpace( maxsizeStr );
            }
        }
    }
    QString minsizeStr = element.attribute( "minsize" ).stripWhiteSpace().lower();
    if ( ! minsizeStr.isNull() ) {
        m_customMinSize = true;
        m_minSize = getSize( minsizeStr, &m_minSizeType );
        if ( m_minSizeType == NoSize ) {
            m_minSizeType = getSpace( minsizeStr );
        }
    }
    QString largeopStr = element.attribute( "largeop" ).stripWhiteSpace().lower();
    if ( ! largeopStr.isNull() ) {
        m_customLargeOp = true;
        if ( largeopStr == "true" ) {
            m_largeOp = true;
        }
        else if ( largeopStr == "false" ) {
            m_largeOp = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `largeop': " << largeopStr << endl;
            m_customLargeOp = false;
        }
    }
    QString movablelimitsStr = element.attribute( "movablelimits" ).stripWhiteSpace().lower();
    if ( ! movablelimitsStr.isNull() ) {
        m_customMovableLimits = true;
        if ( movablelimitsStr == "true" ) {
            m_movableLimits = true;
        }
        else if ( movablelimitsStr == "false" ) {
            m_movableLimits = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `movablelimits': " << movablelimitsStr << endl;
            m_customMovableLimits = false;
        }
    }
    QString accentStr = element.attribute( "accent" ).stripWhiteSpace().lower();
    if ( ! accentStr.isNull() ) {
        m_customAccent = true;
        if ( accentStr == "true" ) {
            m_accent = true;
        }
        else if ( accentStr == "false" ) {
            m_accent = false;
        }
        else {
            kdWarning( DEBUGID ) << "Invalid value for attribute `accent': " << accentStr << endl;
            m_customAccent = false;
        }
    }
    return true;
}

KFORMULA_NAMESPACE_END
