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

#include <qfontmetrics.h>
#include <qpainter.h>

#include <kdebug.h>
#include <kprinter.h>

#include "contextstyle.h"
#include "spaceelement.h"


KFORMULA_NAMESPACE_BEGIN

SpaceElement::SpaceElement( SpaceWidth space, BasicElement* parent )
    : BasicElement( parent ), spaceWidth( space )
{
}

void SpaceElement::calcSizes( const ContextStyle& style,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle /*istyle*/ )
{
    luPt mySize = style.getAdjustedSize( tstyle );

    QFont font = style.getDefaultFont();
    font.setPointSize( mySize );

    QFontMetrics fm( font );
    QChar ch = 'x';
    LuPixelRect bound = fm.boundingRect( ch );

    setWidth( style.getSpace( tstyle, spaceWidth ) );
    setHeight( bound.height() );
    setBaseline( -bound.top() );
    //setMidline( getBaseline() - fm.strikeOutPos() );
}

void SpaceElement::draw( QPainter& painter, const LuPixelRect& r,
                         const ContextStyle& style,
                         ContextStyle::TextStyle /*tstyle*/,
                         ContextStyle::IndexStyle /*istyle*/,
                         const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    // there is such a thing as negative space!
    //if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
    //    return;

    if ( style.edit() ) {
        painter.setPen( style.getEmptyColor() );
        painter.drawLine( style.layoutUnitToPixelX( myPos.x() ),
                          style.layoutUnitToPixelY( myPos.y()+getHeight() ),
                          style.layoutUnitToPixelX( myPos.x()+getWidth()-1 ),
                          style.layoutUnitToPixelY( myPos.y()+getHeight() ) );
        painter.drawLine( style.layoutUnitToPixelX( myPos.x() ),
                          style.layoutUnitToPixelY( myPos.y()+getHeight() ),
                          style.layoutUnitToPixelX( myPos.x() ),
                          style.layoutUnitToPixelY( myPos.y()+getHeight()-getHeight()/5 ) );
        painter.drawLine( style.layoutUnitToPixelX( myPos.x()+getWidth()-1 ),
                          style.layoutUnitToPixelY( myPos.y()+getHeight() ),
                          style.layoutUnitToPixelX( myPos.x()+getWidth()-1 ),
                          style.layoutUnitToPixelY( myPos.y()+getHeight()-getHeight()/5 ) );
    }
}

QString SpaceElement::toLatex()
{
    switch ( spaceWidth ) {
    case NEGTHIN: return "\\!";
    case THIN:    return "\\,";
    case MEDIUM:  return "\\>";
    case THICK:   return "\\;";
    case QUAD:    return "\\quad ";
    }
    return "";
}

void SpaceElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);
    switch ( spaceWidth ) {
    case NEGTHIN:
        element.setAttribute( "WIDTH", "negthin" );
        break;
    case THIN:
        element.setAttribute( "WIDTH", "thin" );
        break;
    case MEDIUM:
        element.setAttribute( "WIDTH", "medium" );
        break;
    case THICK:
        element.setAttribute( "WIDTH", "thick" );
        break;
    case QUAD:
        element.setAttribute( "WIDTH", "quad" );
        break;
    }
}

bool SpaceElement::readAttributesFromDom( QDomElement& element )
{
    if ( !BasicElement::readAttributesFromDom( element ) ) {
        return false;
    }
    QString widthStr = element.attribute( "WIDTH" );
    if( !widthStr.isNull() ) {
        if ( widthStr.lower() == "quad" ) {
            spaceWidth = QUAD;
        }
        else if ( widthStr.lower() == "thick" ) {
            spaceWidth = THICK;
        }
        else if ( widthStr.lower() == "medium" ) {
            spaceWidth = MEDIUM;
        }
        else if ( widthStr.lower() == "negthin" ) {
            spaceWidth = NEGTHIN;
        }
        else {
            spaceWidth = THIN;
        }
    }
    else {
        return false;
    }
    return true;
}

bool SpaceElement::readContentFromDom(QDomNode& node)
{
    return BasicElement::readContentFromDom( node );
}

KFORMULA_NAMESPACE_END
