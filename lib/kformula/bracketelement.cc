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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qptrlist.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpointarray.h>

#include <kdebug.h>
#include <klocale.h>

#include "bracketelement.h"
#include "elementvisitor.h"
#include "fontstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"

KFORMULA_NAMESPACE_BEGIN

SingleContentElement::SingleContentElement(BasicElement* parent )
    : BasicElement( parent )
{
    content = new SequenceElement( this );
}


SingleContentElement::SingleContentElement( const SingleContentElement& other )
    : BasicElement( other )
{
    content = new SequenceElement( other.content );
    content->setParent( this );
}


SingleContentElement::~SingleContentElement()
{
    delete content;
}


QChar SingleContentElement::getCharacter() const
{
    // This is meant to make the SingleContentElement text only.
    // This "fixes" the parenthesis problem (parenthesis too large).
    // I'm not sure if we really want this. There should be better ways.
    if ( content->isTextOnly() ) {
        return '\\';
    }
    return content->getCharacter();
}

BasicElement* SingleContentElement::goToPos( FormulaCursor* cursor, bool& handled,
                                             const LuPixelPoint& point, const LuPixelPoint& parentOrigin )
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        LuPixelPoint myPos(parentOrigin.x() + getX(),
                           parentOrigin.y() + getY());

        e = content->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }
        return this;
    }
    return 0;
}

void SingleContentElement::dispatchFontCommand( FontCommand* cmd )
{
    content->dispatchFontCommand( cmd );
}

void SingleContentElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        //bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            content->moveLeft(cursor, this);
        }
        else {
            getParent()->moveLeft(cursor, this);
        }
    }
}

void SingleContentElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        //bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            content->moveRight(cursor, this);
        }
        else {
            getParent()->moveRight(cursor, this);
        }
    }
}

void SingleContentElement::moveUp(FormulaCursor* cursor, BasicElement* /*from*/)
{
    getParent()->moveUp(cursor, this);
}

void SingleContentElement::moveDown(FormulaCursor* cursor, BasicElement* /*from*/)
{
    getParent()->moveDown(cursor, this);
}

void SingleContentElement::remove( FormulaCursor* cursor,
                                   QPtrList<BasicElement>& removedChildren,
                                   Direction direction )
{
    switch (cursor->getPos()) {
    case contentPos:
        BasicElement* parent = getParent();
        parent->selectChild(cursor, this);
        parent->remove(cursor, removedChildren, direction);
    }
}

void SingleContentElement::normalize( FormulaCursor* cursor, Direction direction )
{
    if (direction == beforeCursor) {
        content->moveLeft(cursor, this);
    }
    else {
        content->moveRight(cursor, this);
    }
}

SequenceElement* SingleContentElement::getMainChild()
{
    return content;
}

void SingleContentElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == content) {
        cursor->setTo(this, contentPos);
    }
}

void SingleContentElement::writeDom(QDomElement element)
{
    BasicElement::writeDom(element);

    QDomDocument doc = element.ownerDocument();

    QDomElement con = doc.createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    element.appendChild(con);
}

bool SingleContentElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    if ( !buildChild( content, node, "CONTENT" ) ) {
        kdWarning( DEBUGID ) << "Empty content in " << getTagName() << endl;
        return false;
    }
    node = node.nextSibling();

    return true;
}

int SingleContentElement::readContentFromMathMLDom( QDomNode& node )
{
    if ( BasicElement::readContentFromMathMLDom( node ) == -1 ) {
        return -1;
    }

    if ( !content->buildMathMLChild( node ) ) {
        kdWarning( DEBUGID) << "Empty content in SingleContentElement\n";
        return -1;
    }
    node = node.nextSibling();

    return 1;
}

void SingleContentElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    content->writeMathML( doc, parent, oasisFormat );
}



BracketElement::BracketElement(SymbolType l, SymbolType r, BasicElement* parent)
    : SingleContentElement(parent),
      left( 0 ), right( 0 ),
      leftType( l ), rightType( r ),
      m_operator( false ), m_customLeft( false ), m_customRight( false )
{
}


BracketElement::~BracketElement()
{
    delete left;
    delete right;
}


BracketElement::BracketElement( const BracketElement& other )
    : SingleContentElement( other ),
      left( 0 ), right( 0 ),
      leftType( other.leftType ), rightType( other.rightType ),
      m_operator( other.m_operator ),
      m_customLeft( other.m_customLeft ), m_customRight( other.m_customRight )
{
}


bool BracketElement::accept( ElementVisitor* visitor )
{
    return visitor->visit( this );
}


void BracketElement::entered( SequenceElement* /*child*/ )
{
    formula()->tell( i18n( "Delimited list" ) );
}


BasicElement* BracketElement::goToPos( FormulaCursor* cursor, bool& handled,
                                       const LuPixelPoint& point, const LuPixelPoint& parentOrigin )
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        LuPixelPoint myPos(parentOrigin.x() + getX(),
                           parentOrigin.y() + getY());
        e = getContent()->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }

        // We are in one of those gaps.
        luPixel dx = point.x() - myPos.x();
        luPixel dy = point.y() - myPos.y();

        if ((dx > getContent()->getX()+getContent()->getWidth()) ||
            (dy > getContent()->getY()+getContent()->getHeight())) {
            getContent()->moveEnd(cursor);
            handled = true;
            return getContent();
        }
        return this;
    }
    return 0;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void BracketElement::calcSizes( const ContextStyle& context,
                                ContextStyle::TextStyle tstyle,
                                ContextStyle::IndexStyle istyle,
                                StyleAttributes& style )
{
    SequenceElement* content = getContent();
    content->calcSizes( context, tstyle, istyle, style );

    //if ( left == 0 ) {
    delete left;
    delete right;
    left = context.fontStyle().createArtwork( leftType );
    right = context.fontStyle().createArtwork( rightType );
    //}

    double factor = style.sizeFactor();
    if (content->isTextOnly()) {
        left->calcSizes(context, tstyle, factor);
        right->calcSizes(context, tstyle, factor);

        setBaseline(QMAX(content->getBaseline(),
                         QMAX(left->getBaseline(), right->getBaseline())));

        content->setY(getBaseline() - content->getBaseline());
        left   ->setY(getBaseline() - left   ->getBaseline());
        right  ->setY(getBaseline() - right  ->getBaseline());

        //setMidline(content->getY() + content->getMidline());
        setHeight(QMAX(content->getY() + content->getHeight(),
                       QMAX(left ->getY() + left ->getHeight(),
                            right->getY() + right->getHeight())));
    }
    else {
        //kdDebug( DEBUGID ) << "BracketElement::calcSizes " << content->axis( context, tstyle ) << " " << content->getHeight() << endl;
        luPixel contentHeight = 2 * QMAX( content->axis( context, tstyle, factor ),
                                          content->getHeight() - content->axis( context, tstyle, factor ) );
        left->calcSizes( context, tstyle, factor, contentHeight );
        right->calcSizes( context, tstyle, factor, contentHeight );

        // height
        setHeight(QMAX(contentHeight,
                       QMAX(left->getHeight(), right->getHeight())));
        //setMidline(getHeight() / 2);

        content->setY(getHeight() / 2 - content->axis( context, tstyle, factor ));
        setBaseline(content->getBaseline() + content->getY());

        if ( left->isNormalChar() ) {
            left->setY(getBaseline() - left->getBaseline());
        }
        else {
            left->setY((getHeight() - left->getHeight())/2);
        }
        if ( right->isNormalChar() ) {
            right->setY(getBaseline() - right->getBaseline());
        }
        else {
            right->setY((getHeight() - right->getHeight())/2);
        }

//         kdDebug() << "BracketElement::calcSizes" << endl
//                   << "getHeight(): " << getHeight() << endl
//                   << "left->getHeight():  " << left->getHeight() << endl
//                   << "right->getHeight(): " << right->getHeight() << endl
//                   << "left->getY():  " << left->getY() << endl
//                   << "right->getY(): " << right->getY() << endl
//                   << endl;
    }

    // width
    setWidth(left->getWidth() + content->getWidth() + right->getWidth());
    content->setX(left->getWidth());
    right  ->setX(left->getWidth()+content->getWidth());
}


/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void BracketElement::draw( QPainter& painter, const LuPixelRect& r,
                           const ContextStyle& context,
                           ContextStyle::TextStyle tstyle,
                           ContextStyle::IndexStyle istyle,
                           StyleAttributes& style,
                           const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    //if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
    //    return;

    SequenceElement* content = getContent();
    content->draw(painter, r, context, tstyle, istyle, style, myPos);

    if (content->isTextOnly()) {
        left->draw(painter, r, context, tstyle, style, myPos);
        right->draw(painter, r, context, tstyle, style, myPos);
    }
    else {
        double factor = style.sizeFactor();
        luPixel contentHeight = 2 * QMAX(content->axis( context, tstyle, factor ),
                                         content->getHeight() - content->axis( context, tstyle, factor ));
        left->draw(painter, r, context, tstyle, style, contentHeight, myPos);
        right->draw(painter, r, context, tstyle, style, contentHeight, myPos);
    }

    // Debug
#if 0
    painter.setBrush( Qt::NoBrush );
    painter.setPen( Qt::red );
    painter.drawRect( context.layoutUnitToPixelX( myPos.x()+left->getX() ),
                      context.layoutUnitToPixelY( myPos.y()+left->getY() ),
                      context.layoutUnitToPixelX( left->getWidth() ),
                      context.layoutUnitToPixelY( left->getHeight() ) );
    painter.drawRect( context.layoutUnitToPixelX( myPos.x()+right->getX() ),
                      context.layoutUnitToPixelY( myPos.y()+right->getY() ),
                      context.layoutUnitToPixelX( right->getWidth() ),
                      context.layoutUnitToPixelY( right->getHeight() ) );
#endif
}


/**
 * Appends our attributes to the dom element.
 */
void BracketElement::writeDom(QDomElement element)
{
    SingleContentElement::writeDom(element);
    element.setAttribute("LEFT", leftType);
    element.setAttribute("RIGHT", rightType);
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool BracketElement::readAttributesFromDom(QDomElement element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }
    QString leftStr = element.attribute("LEFT");
    if(!leftStr.isNull()) {
        leftType = static_cast<SymbolType>(leftStr.toInt());
    }
    QString rightStr = element.attribute("RIGHT");
    if(!rightStr.isNull()) {
        rightType = static_cast<SymbolType>(rightStr.toInt());
    }
    return true;
}

/**
 * Reads our attributes from the MathML element.
 * Returns false if it failed.
 */
bool BracketElement::readAttributesFromMathMLDom(const QDomElement& element)
{
    if ( !BasicElement::readAttributesFromMathMLDom( element ) ) {
        return false;
    }

    if ( element.tagName().lower() == "mo" ) {
        m_operator = true;
        // TODO: parse attributes in section 3.2.5.2
    }
    else { // mfenced, see attributes in section 3.3.8.2
        leftType = LeftRoundBracket;
        rightType = RightRoundBracket;
        QString openStr = element.attribute( "open" ).stripWhiteSpace();
        if ( !openStr.isNull() ) {
            m_customLeft = true;
            if ( openStr == "[" )
                leftType = LeftSquareBracket;
            else if ( openStr == "]" )
                leftType = RightSquareBracket;
            else if ( openStr == "{" )
                leftType = LeftCurlyBracket;
            else if ( openStr == "}" )
                leftType = RightCurlyBracket;
            else if ( openStr == "<" )
                leftType = LeftCornerBracket;
            else if ( openStr == ">" )
                leftType = RightCornerBracket;
            else if ( openStr == "(" )
                leftType = LeftRoundBracket;
            else if ( openStr == ")" )
                leftType = RightRoundBracket;
            else if ( openStr == "/" )
                leftType = SlashBracket;
            else if ( openStr == "\\" )
                leftType = BackSlashBracket;
            else // TODO: Check for entity references
                leftType = LeftRoundBracket;
        }
        QString closeStr = element.attribute( "close" ).stripWhiteSpace();
        if ( !closeStr.isNull() ) {
            m_customRight = true;
            if ( closeStr == "[" )
                rightType = LeftSquareBracket;
            else if ( closeStr == "]" )
                rightType = RightSquareBracket;
            else if ( closeStr == "{" )
                rightType = LeftCurlyBracket;
            else if ( closeStr == "}" )
                rightType = RightCurlyBracket;
            else if ( closeStr == "<" )
                rightType = LeftCornerBracket;
            else if ( closeStr == ">" )
                rightType = RightCornerBracket;
            else if ( closeStr == "(" )
                rightType = LeftRoundBracket;
            else if ( closeStr == ")" )
                rightType = RightRoundBracket;
            else if ( closeStr == "/" )
                rightType = SlashBracket;
            else if ( closeStr == "\\" )
                rightType = BackSlashBracket;
            else // TODO: Check for entity references
                rightType = LeftRoundBracket;
        }
        QString separatorStr = element.attribute( "separators" ).stripWhiteSpace();
        if ( !separatorStr.isNull() ) {
            m_separators = QStringList::split( ' ', separatorStr );
        }
    }
    return true;
}

/**
 * Reads our content from the MathML node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
int BracketElement::readContentFromMathMLDom(QDomNode& node)
{
    bool empty = false;
    if ( m_operator ) {
        node = node.parentNode();
        QDomNode open = node;
        QDomNode parent = node.parentNode();
        if ( ! operatorType( node, true ) )
            return -1;
        int nodeNum = searchOperator( node );
        if ( nodeNum == -1 ) // Closing bracket not found
            return -1;
        if ( nodeNum == 1 ) { // Empty content
            empty = true;
        }
        if ( nodeNum > 2 ) { // More than two elements inside, infer a mrow
            kdWarning() << "NodeNum: " << nodeNum << endl;
            QDomDocument doc = node.ownerDocument();
            QDomElement de = doc.createElement( "mrow" );
            int i = 0;
            do {
                QDomNode n = node.nextSibling();
                de.appendChild( node.toElement() );
                node = n;
            } while ( ++i < nodeNum );
            parent.insertAfter( de, open );
            node = de;
            kdWarning() << doc.toString() << endl;
        }
    }
    else {
        // if it's a mfence tag, we need to convert to equivalent expanded form.
        // See section 3.3.8
        if ( ! node.nextSibling().isNull() ) {
            QDomDocument doc = node.ownerDocument();
            QDomNode parent = node.parentNode();
            QDomElement de = doc.createElement( "mrow" );
            QStringList::Iterator it = m_separators.begin();
            while ( !node.isNull() ) {
                QDomNode no = node.nextSibling();
                de.appendChild( node.toElement() );
                if ( ! no.isNull() ) {
                    QDomElement sep = doc.createElement( "mo" );
                    de.appendChild( sep );
                    if ( it != m_separators.end() ) {
                        sep.appendChild( doc.createTextNode( *it ) );
                    }
                    else {
                        sep.appendChild( doc.createTextNode( "," ) );
                    }
                    it++;
                }
                node = no;
            }
            parent.appendChild( de );
            node = parent.firstChild();
        }
    }
    if ( ! empty ) {
        inherited::readContentFromMathMLDom( node );
    }
    if ( m_operator ) {
        if ( ! operatorType( node, false ) ) {
            return -1;
        }
        if ( empty )
            return 2;
        return 3;
    }
    return 1;
}

QString BracketElement::toLatex()
{
    QString ls,rs,cs;
    cs=getContent()->toLatex();
    ls="\\left"+latexString(leftType) + " ";
    rs=" \\right"+latexString(rightType);

    return ls+cs+rs;
}

QString BracketElement::latexString(char type)
{
    switch (type) {
	case ']':
	    return "]";
	case '[':
	    return "[";
	case '{':
	    return "\\{";
	case '}':
	    return "\\}";
	case '(':
	    return "(";
	case ')':
	    return ")";
	case '|':
	    return "|";
        case '<':
            return "\\langle";
        case '>':
            return "\\rangle";
        case '/':
            return "/";
        case '\\':
            return "\\backslash";
    }
    return ".";
}

QString BracketElement::formulaString()
{
    return "(" + getContent()->formulaString() + ")";
}

bool BracketElement::operatorType( QDomNode& node, bool open )
{
    SymbolType* type = open ? &leftType : &rightType;
    if ( node.isElement() ) {
        QDomElement e = node.toElement();
        QString s =  e.text();
        if ( s.isNull() )
            return false;
        *type = static_cast<SymbolType>( QString::number( s.at( 0 ).latin1() ).toInt() );
        node = node.nextSibling();
    }
    else if ( node.isEntityReference() ) {
        QString name = node.nodeName();
        // TODO: To fully support these, SymbolType has to be extended,
        //       and better Unicode support is a must
        // CloseCurlyDoubleQuote 0x201D
        // CloseCurlyQoute       0x2019
        // LeftCeiling           0x2308
        // LeftDoubleBracket     0x301A
        // LeftFloor             0x230A
        // OpenCurlyDoubleQuote  0x201C
        // OpenCurlyQuote        0x2018
        // RightCeiling          0x2309
        // RightDoubleBracket    0x301B
        // RightFloor            0x230B
        if ( name == "LeftAngleBracket" ) {
            *type = LeftCornerBracket;
        }
        else if ( name == "RightAngleBracket" ) {
            *type = RightCornerBracket; 
        }
        else {
            if ( open ) {
                *type = LeftRoundBracket;
            }
            else
                *type = RightRoundBracket;
        }
        node = node.nextSibling();
    }
    else {
        return false;
    }
    return true;
}

int BracketElement::searchOperator( const QDomNode& node )
{
    QDomNode n = node;
    for ( int i = 0; ! n.isNull(); n = n.nextSibling(), i++ ) {
        if ( n.isElement() ) {
            QDomElement e = n.toElement();
            if ( e.tagName().lower() == "mo" ) {
                // Try to guess looking at attributes
                QString form = e.attribute( "form" );
                QString f;
                if ( ! form.isNull() ) {
                    f = form.stripWhiteSpace().lower();
                }
                QString fence = e.attribute( "fence" );
                if ( ! fence.isNull() ) {
                    if ( fence.stripWhiteSpace().lower() == "false" ) {
                        continue;
                    }
                    if ( ! f.isNull() ) {
                        if ( f == "postfix" ) {
                            return i;
                        }
                        else {
                            continue;
                        }
                    }
                }
                
                // Guess looking at contents
                QDomNode child = e.firstChild();
                QString name;
                if ( child.isText() )
                    name = child.toText().data().stripWhiteSpace();
                else if ( child.isEntityReference() )
                    name = child.nodeName();
                else 
                    continue;
                if ( name == ")"
                     || name == "]"
                     || name == "}"
                     || name == "CloseCurlyDoubleQuote"
                     || name == "CloseCurlyQuote"
                     || name == "RightAngleBracket"
                     || name == "RightCeiling"
                     || name == "RightDoubleBracket"
                     || name == "RightFloor" ) {
                    if ( f.isNull() || f == "postfix" )
                        return i;
                }
                if ( name == "("
                     || name == "["
                     || name == "{"
                     || name == "LeftAngleBracket"
                     || name == "LeftCeiling"
                     || name == "LeftDoubleBracket"
                     || name == "LeftFloor"
                     || name == "OpenCurlyQuote" ) {
                    if ( ! f.isNull() && f == "postfix" )
                        return i;
                }
            }
        }
    }
    return -1;
}


void BracketElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:mfenced" : "mfenced" );
    if ( left->getType() != LeftRoundBracket ||
         right->getType() != RightRoundBracket )
    {
        de.setAttribute( "open",  QString( QChar( leftType ) ) );
        de.setAttribute( "close", QString( QChar( rightType ) ) );
    }
    SingleContentElement::writeMathML( doc, de, oasisFormat );
    parent.appendChild( de );
}


OverlineElement::OverlineElement( BasicElement* parent )
    : SingleContentElement( parent )
{
}

OverlineElement::~OverlineElement()
{
}

OverlineElement::OverlineElement( const OverlineElement& other )
    : SingleContentElement( other )
{
}


bool OverlineElement::accept( ElementVisitor* visitor )
{
    return visitor->visit( this );
}


void OverlineElement::entered( SequenceElement* /*child*/ )
{
    formula()->tell( i18n( "Overline" ) );
}


void OverlineElement::calcSizes( const ContextStyle& context,
                                 ContextStyle::TextStyle tstyle,
                                 ContextStyle::IndexStyle istyle,
                                 StyleAttributes& style )
{
    SequenceElement* content = getContent();
    content->calcSizes(context, tstyle,
                       context.convertIndexStyleLower(istyle), style );

    //luPixel distX = context.ptToPixelX( context.getThinSpace( tstyle, style.sizeFactor() ) );
    luPixel distY = context.ptToPixelY( context.getThinSpace( tstyle, style.sizeFactor() ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    setWidth( content->getWidth() );
    setHeight( content->getHeight() + distY );

    content->setX( 0 );
    content->setY( distY );
    setBaseline(content->getBaseline() + content->getY());
}

void OverlineElement::draw( QPainter& painter, const LuPixelRect& r,
                            const ContextStyle& context,
                            ContextStyle::TextStyle tstyle,
                            ContextStyle::IndexStyle istyle,
                            StyleAttributes& style,
                            const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    //if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
    //    return;

    SequenceElement* content = getContent();
    content->draw( painter, r, context, tstyle,
                   context.convertIndexStyleLower( istyle ), style, myPos );

    luPixel x = myPos.x();
    luPixel y = myPos.y();
    //int distX = context.getDistanceX(tstyle);
    double factor = style.sizeFactor();
    luPixel distY = context.ptToPixelY( context.getThinSpace( tstyle, factor ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    painter.setPen( QPen( context.getDefaultColor(),
                          context.layoutUnitToPixelY( context.getLineWidth( factor ) ) ) );

    painter.drawLine( context.layoutUnitToPixelX( x ),
                      context.layoutUnitToPixelY( y+distY/3 ),
                      context.layoutUnitToPixelX( x+content->getWidth() ),
                      context.layoutUnitToPixelY( y+distY/3 ) );
}


QString OverlineElement::toLatex()
{
    return "\\overline{" + getContent()->toLatex() + "}";
}

QString OverlineElement::formulaString()
{
    return getContent()->formulaString();
}

void OverlineElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:mover" : "mover" );
    SingleContentElement::writeMathML( doc, de, oasisFormat );
    QDomElement op = doc.createElement( oasisFormat ? "math:mo" : "mo" );
    // is this the right entity? Mozilla renders it correctly.
    op.appendChild( doc.createEntityReference( "OverBar" ) );
    de.appendChild( op );
    parent.appendChild( de );
}


UnderlineElement::UnderlineElement( BasicElement* parent )
    : SingleContentElement( parent )
{
}

UnderlineElement::~UnderlineElement()
{
}


UnderlineElement::UnderlineElement( const UnderlineElement& other )
    : SingleContentElement( other )
{
}


bool UnderlineElement::accept( ElementVisitor* visitor )
{
    return visitor->visit( this );
}


void UnderlineElement::entered( SequenceElement* /*child*/ )
{
    formula()->tell( i18n( "Underline" ) );
}


void UnderlineElement::calcSizes( const ContextStyle& context,
                                  ContextStyle::TextStyle tstyle,
                                  ContextStyle::IndexStyle istyle,
                                  StyleAttributes& style )
{
    SequenceElement* content = getContent();
    double factor = style.sizeFactor();
    content->calcSizes(context, tstyle,
                       context.convertIndexStyleLower(istyle), style );

    //luPixel distX = context.ptToPixelX( context.getThinSpace( tstyle ) );
    luPixel distY = context.ptToPixelY( context.getThinSpace( tstyle, factor ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    setWidth( content->getWidth() );
    setHeight( content->getHeight() + distY );

    content->setX( 0 );
    content->setY( 0 );
    setBaseline(content->getBaseline() + content->getY());
}

void UnderlineElement::draw( QPainter& painter, const LuPixelRect& r,
                             const ContextStyle& context,
                             ContextStyle::TextStyle tstyle,
                             ContextStyle::IndexStyle istyle,
                             StyleAttributes& style,
                             const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    //if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
    //    return;

    SequenceElement* content = getContent();
    content->draw( painter, r, context, tstyle,
                   context.convertIndexStyleLower( istyle ), style, myPos );

    luPixel x = myPos.x();
    luPixel y = myPos.y();
    //int distX = context.getDistanceX(tstyle);
    //luPixel distY = context.ptToPixelY( context.getThinSpace( tstyle ) );
    //luPixel unit = (content->getHeight() + distY)/ 3;

    double factor = style.sizeFactor();
    painter.setPen( QPen( context.getDefaultColor(),
                          context.layoutUnitToPixelY( context.getLineWidth( factor ) ) ) );

    painter.drawLine( context.layoutUnitToPixelX( x ),
                      context.layoutUnitToPixelY( y+getHeight()-context.getLineWidth( factor ) ),
                      context.layoutUnitToPixelX( x+content->getWidth() ),
                      context.layoutUnitToPixelY( y+getHeight()-context.getLineWidth( factor ) ) );
}


QString UnderlineElement::toLatex()
{
    return "\\underline{" + getContent()->toLatex() + "}";
}

QString UnderlineElement::formulaString()
{
    return getContent()->formulaString();
}

void UnderlineElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:munder" : "munder" );
    SingleContentElement::writeMathML( doc, de, oasisFormat );
    QDomElement op = doc.createElement( oasisFormat ? "math:mo" : "mo" );
    // is this the right entity? Mozilla renders it correctly.
    op.appendChild( doc.createEntityReference( "UnderBar" ) );
    de.appendChild( op );
    parent.appendChild( de );
}

KFORMULA_NAMESPACE_END
