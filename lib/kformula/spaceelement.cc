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
#include <koprinter.h>

#include "contextstyle.h"
#include "spaceelement.h"


KFORMULA_NAMESPACE_BEGIN

SpaceElement::SpaceElement( SpaceWidths space, BasicElement* parent )
    : BasicElement( parent ), spaceWidth( space )
{
}

void SpaceElement::calcSizes( const ContextStyle& context,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle /*istyle*/ )
{
    double mySize = context.getAdjustedSize( tstyle );

    QFont font = context.getDefaultFont();
    font.setPointSizeFloat( mySize );

    QFontMetrics fm( font );
    QChar ch = 'x';
    QRect bound = fm.boundingRect( ch );

    setWidth( context.getSpace( tstyle, spaceWidth ) );
    setHeight( bound.height() );
    setBaseline( -bound.top() );
    setMidline( getBaseline() - fm.strikeOutPos() );
}

void SpaceElement::draw( QPainter& painter, const QRect& r,
                         const ContextStyle& context,
                         ContextStyle::TextStyle /*tstyle*/,
                         ContextStyle::IndexStyle /*istyle*/,
                         const KoPoint& parentOrigin )
{
    KoPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    //double mySize = context.getAdjustedSize( tstyle );
    if (!QRect(myPos.x(), myPos.y(), getWidth(), getHeight()).intersects(r))
        return;

    if ( painter.device()->devType() != QInternal::Printer ) {
        painter.setPen( context.getEmptyColor() );
        painter.drawLine( myPos.x(), myPos.y()+getHeight(),
                          myPos.x()+getWidth()-1, myPos.y()+getHeight() );
        painter.drawLine( myPos.x(), myPos.y()+getHeight(),
                          myPos.x(), myPos.y()+getHeight()-getHeight()/5 );
        painter.drawLine( myPos.x()+getWidth()-1, myPos.y()+getHeight(),
                          myPos.x()+getWidth()-1, myPos.y()+getHeight()-getHeight()/5 );
    }
}

QString SpaceElement::toLatex()
{
    switch ( spaceWidth ) {
    case THIN:   return "\\, ";
    case MEDIUM: return "\\> ";
    case THICK:  return "\\; ";
    case QUAD:   return "\\quad ";
    }
    return "";
}

void SpaceElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);
    switch ( spaceWidth ) {
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
