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

#include "basicelement.h"
#include "contextstyle.h"
#include "elementtype.h"
#include "formulaelement.h"
#include "sequenceelement.h"
#include "textelement.h"
#include "symboltable.h"

KFORMULA_NAMESPACE_BEGIN

TextElement::TextElement(QChar ch, bool beSymbol, BasicElement* parent)
        : BasicElement(parent), character(ch), symbol(beSymbol)
{
}


TokenType TextElement::getTokenType() const
{
    if ( isSymbol() ) {
        return getSymbolTable().charClass( character );
    }

    switch ( character.latin1() ) {
    case '+':
    case '-':
    case '*':
        //case '/':  because it counts as text -- no extra spaces
        return BINOP;
    case '=':
    case '<':
    case '>':
        return RELATION;
    case ',':
    case ';':
    case ':':
        return PUNCTUATION;
    case '\\':
        return SEPARATOR;
    case '\0':
        return ELEMENT;
    default:
        if ( character.isNumber() ) {
            return NUMBER;
        }
        else {
            return ORDINARY;
        }
    }
}


bool TextElement::isInvisible() const
{
    if (getElementType() != 0) {
        return getElementType()->isInvisible(*this);
    }
    return false;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void TextElement::calcSizes(const ContextStyle& context, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle /*istyle*/)
{
    luPt mySize = context.getAdjustedSize( tstyle );
    //kdDebug( DEBUGID ) << "TextElement::calcSizes size=" << mySize << endl;

    QFont font = getFont( context );
    font.setPointSizeFloat( context.layoutUnitPtToPt( mySize ) );

    QFontMetrics fm( font );
    QChar ch = getRealCharacter();
    if ( ch != QChar::null ) {
        QRect bound = fm.boundingRect( ch );
        setWidth( context.ptToLayoutUnitPt( fm.width( ch ) ) );
        setHeight( context.ptToLayoutUnitPt( bound.height() ) );
        setBaseline( context.ptToLayoutUnitPt( -bound.top() ) );
    }
    else {
        setWidth( qRound( context.getEmptyRectWidth() * 2./3. ) );
        setHeight( qRound( context.getEmptyRectHeight() * 2./3. ) );
        setBaseline( getHeight() );
    }

    //kdDebug( DEBUGID ) << "height: " << getHeight() << endl;
    //kdDebug( DEBUGID ) << "width: " << getWidth() << endl;
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void TextElement::draw( QPainter& painter, const LuPixelRect& r,
                        const ContextStyle& context,
                        ContextStyle::TextStyle tstyle,
                        ContextStyle::IndexStyle /*istyle*/,
                        const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
        return;

    setUpPainter( context, painter );

    luPt mySize = context.getAdjustedSize( tstyle );
    QFont font = getFont( context );
    font.setPointSizeFloat( context.layoutUnitToFontSize( mySize, false ) );
    painter.setFont( font );

    //kdDebug( DEBUGID ) << "TextElement::draw font=" << font.rawName() << endl;
    //kdDebug( DEBUGID ) << "TextElement::draw size=" << mySize << endl;
    //kdDebug( DEBUGID ) << "TextElement::draw size=" << context.layoutUnitToFontSize( mySize, false ) << endl;
    //kdDebug( DEBUGID ) << "TextElement::draw height: " << getHeight() << endl;
    //kdDebug( DEBUGID ) << "TextElement::draw width: " << getWidth() << endl;
    //kdDebug( DEBUGID ) << endl;

    QChar ch = getRealCharacter();
    if ( ch != QChar::null ) {
        painter.drawText( context.layoutUnitToPixelX( myPos.x() ),
                          context.layoutUnitToPixelY( myPos.y()+getBaseline() ),
                          ch );
    }
    else {
        painter.setPen( QPen( context.getErrorColor(),
                              context.layoutUnitToPixelX( context.getLineWidth() ) ) );
        painter.drawRect( context.layoutUnitToPixelX( myPos.x() ),
                          context.layoutUnitToPixelY( myPos.y() ),
                          context.layoutUnitToPixelX( getWidth() ),
                          context.layoutUnitToPixelY( getHeight() ) );
    }

    // Debug
    //painter.setBrush(Qt::NoBrush);
//     painter.setPen( Qt::red );
//     painter.drawRect( context.layoutUnitToPixelX( myPos.x() ),
//                       context.layoutUnitToPixelX( myPos.y() ),
//                       context.layoutUnitToPixelX( getWidth() ),
//                       context.layoutUnitToPixelX( getHeight() ) );
    //painter.setPen(Qt::green);
    //painter.drawLine(myPos.x(), myPos.y()+getMidline(),
    //                 myPos.x()+getWidth(), myPos.y()+getMidline());
}


QChar TextElement::getRealCharacter()
{
    if ( !isSymbol() ) {
        return character;
    }
    else {
        return getSymbolTable().character(character);
    }
}


QFont TextElement::getFont(const ContextStyle& context)
{
    if ( !isSymbol() ) {
        if (getElementType() != 0) {
            return getElementType()->getFont(context);
        }
        else {
            return context.getDefaultFont();
        }
    }
    return context.symbolTable().font(character);
}


void TextElement::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    if (getElementType() != 0) {
        getElementType()->setUpPainter(context, painter);
    }
    else {
        painter.setPen(Qt::red);
    }
}

const SymbolTable& TextElement::getSymbolTable() const
{
    return formula()->getSymbolTable();
}


/**
 * Appends our attributes to the dom element.
 */
void TextElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);
    element.setAttribute("CHAR", QString(character));
    if (symbol) element.setAttribute("SYMBOL", "3");
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool TextElement::readAttributesFromDom(QDomElement& element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }
    QString charStr = element.attribute("CHAR");
    if(!charStr.isNull()) {
        character = charStr.at(0);
    }
    QString symbolStr = element.attribute("SYMBOL");
    if(!symbolStr.isNull()) {
        int symbolInt = symbolStr.toInt();
        if ( symbolInt == 1 ) {
            character = getSymbolTable().unicodeFromSymbolFont(character);
        }
        if ( symbolInt == 2 ) {
            switch ( character.unicode() ) {
            case 0x03D5: character = 0x03C6; break;
            case 0x03C6: character = 0x03D5; break;
            case 0x03Ba: character = 0x03BA; break;
            case 0x00B4: character = 0x2032; break;
            case 0x2215: character = 0x2244; break;
            case 0x00B7: character = 0x2022; break;
            case 0x1D574: character = 0x2111; break;
            case 0x1D579: character = 0x211C; break;
            case 0x2219: character = 0x22C5; break;
            case 0x2662: character = 0x26C4; break;
            case 0x220B: character = 0x220D; break;
            case 0x224C: character = 0x2245; break;
            case 0x03DB: character = 0x03C2; break;
            }
        }
        symbol = symbolInt != 0;
    }
    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool TextElement::readContentFromDom(QDomNode& node)
{
    return BasicElement::readContentFromDom(node);
}

QString TextElement::toLatex()
{
    if ( isSymbol() ) {
        QString texName = getSymbolTable().name( character );
        if ( !texName.isNull() )
            return "\\" + texName;
        return  " ? ";
    }
    else {
        return character;
    }
}

QString TextElement::formulaString()
{
    if ( isSymbol() ) {
        QString texName = getSymbolTable().name( character );
        if ( !texName.isNull() )
            return " " + texName + " ";
        return " ? ";
    }
    else {
        return character;
    }
}


EmptyElement::EmptyElement( BasicElement* parent )
    : BasicElement( parent )
{
}

void EmptyElement::calcSizes( const ContextStyle& context,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle /*istyle*/ )
{
    luPt mySize = context.getAdjustedSize( tstyle );
    //kdDebug( DEBUGID ) << "TextElement::calcSizes size=" << mySize << endl;

    QFont font = context.getDefaultFont();
    font.setPointSizeFloat( context.layoutUnitPtToPt( mySize ) );

    QFontMetrics fm( font );
    QChar ch = 'A';
    QRect bound = fm.boundingRect( ch );
    setWidth( 0 );
    setHeight( context.ptToLayoutUnitPt( bound.height() ) );
    setBaseline( context.ptToLayoutUnitPt( -bound.top() ) );
}

void EmptyElement::draw( QPainter& painter, const LuPixelRect& /*r*/,
                         const ContextStyle& context,
                         ContextStyle::TextStyle /*tstyle*/,
                         ContextStyle::IndexStyle /*istyle*/,
                         const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    /*
    if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
        return;
    */

    if ( context.edit() ) {
        painter.setPen( context.getEmptyColor() );
        painter.drawLine( context.layoutUnitToPixelX( myPos.x() ),
                          context.layoutUnitToPixelY( myPos.y() ),
                          context.layoutUnitToPixelX( myPos.x() ),
                          context.layoutUnitToPixelY( myPos.y()+getHeight() ) );
    }
}

QString EmptyElement::toLatex()
{
    return "{}";
}

KFORMULA_NAMESPACE_END
