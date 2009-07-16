/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#include "FormulaCursor.h"
#include "BasicElement.h"
#include "RowElement.h"
#include "FixedElement.h"
#include "EmptyElement.h"
#include "NumberElement.h"
#include "ElementFactory.h"
#include "OperatorElement.h"
#include "IdentifierElement.h"
#include "ElementFactory.h"
#include "FormulaCommand.h"
#include <QPainter>
#include <QPen>
#include <algorithm>

#include <kdebug.h>
#include <QUndoCommand>

FormulaCursor::FormulaCursor( BasicElement* element, FormulaData* data )
              : m_currentElement( element ), m_data( data )
{
    m_position = 0;
    m_mark = 0;
    m_selecting = false;
    m_direction = NoDirection;
    m_data=data;
}

void FormulaCursor::paint( QPainter& painter ) const
{
    kDebug() << "Drawing cursor with selecting: "<< isSelecting() << " from "
    << mark()<<" to " << position();
    if( !m_currentElement )
        return;
    painter.save();
    QPointF origin=m_currentElement->absoluteBoundingRect().topLeft();
    double baseline=m_currentElement->baseLine();
    QPen pen;
    pen.setWidthF( 0.5 );
    pen.setColor(Qt::black);
    painter.setPen( pen );
    painter.drawLine(m_currentElement->cursorLine( m_position ));
    pen.setWidth( 0.1);
    pen.setColor(Qt::blue);
    pen.setStyle(Qt::DashLine);
    painter.setPen( pen );
//    painter.drawLine( origin+QPointF(0.0,baseline),
//        origin+QPointF(m_currentElement->width(), baseline) );
    pen.setStyle(Qt::DotLine);
    //Only here for debug purpose for now
    switch(m_currentElement->elementType()) {
    case Number:
        pen.setColor(Qt::red);
        break;
    case Identifier:
        pen.setColor(Qt::darkRed);
        break;
    case Row:
        pen.setColor(Qt::yellow);
        break;
    case Fraction:
        pen.setColor(Qt::blue);
        break;
    case Table:
        pen.setColor(Qt::darkGreen);
        break;
    case TableRow:
        pen.setColor(Qt::green);
        break;
    default:
        pen.setColor(Qt::darkGray);
        break;
    }
    painter.setPen(pen);
    painter.drawRect( m_currentElement->absoluteBoundingRect() );
    //draw the selection rectangle
    if ( m_selecting ) {
        QBrush brush;
        QColor color(Qt::blue);
        color.setAlpha(128);
        brush.setColor(color);
        brush.setStyle(Qt::SolidPattern);
        painter.setBrush(brush);
        painter.setPen(Qt::NoPen);
        int p1=position()<mark()? position() : mark();
        int p2=position()<mark()? mark() : position() ;
        painter.drawPath(m_currentElement->selectionRegion(p1,p2));
        painter.restore();
    }
}


void FormulaCursor::selectElement(BasicElement* element)
{
    m_selecting=true;
    m_currentElement=element;
    m_mark=0;
    m_position=m_currentElement->length();
}


FormulaCommand* FormulaCursor::insertText( const QString& text )
{
    FormulaCommand *undo=0;
    m_inputBuffer = text;
    if (insideToken()) {
        TokenElement* token=static_cast<TokenElement*>(m_currentElement);
        if (hasSelection()) {
            undo = new FormulaCommandRemoveText(token,selection().first,selection().second-selection().first,undo);
        }
        undo = new FormulaCommandAddText(token,m_position,text,undo);
    } else {
        TokenElement* token = static_cast<TokenElement*>
            (ElementFactory::createElement(tokenType(text[0]),0));
        token->insertText(0,text);
        insertElement(token);
        moveTo(token,token->length());
    }
    return undo;
}

FormulaCommand* FormulaCursor::insertData( const QString& data )
{
    BasicElement* elementToInsert=ElementFactory::createElement("mfrac",0);
    return insertElement( elementToInsert );
    //FIXME: we leak memory of the insert fails
}

FormulaCommand* FormulaCursor::insertElement( BasicElement* element )
{
    FormulaCommand *undo=0;
    if (insideInferredRow()) {
        if (hasSelection()) {
            remove(true);
        }
        m_currentElement->insertChild( m_position, element );
        element->setParentElement(m_currentElement);        
    } else if (insideFixedElement()) {
        if (hasSelection()) {
            //TODO
            return 0;
        } else {
            BasicElement* tmpRow = new RowElement(m_currentElement);
            BasicElement* oldchild;
            if (m_currentElement->elementAfter(m_position)!=0) {
                oldchild=m_currentElement->elementAfter(m_position);
                tmpRow->insertChild(0,element);
                tmpRow->insertChild(1,oldchild);
            } else {
                oldchild=m_currentElement->elementBefore(m_position);
                tmpRow->insertChild(0,oldchild);
                tmpRow->insertChild(1,element);
            }
            oldchild->setParentElement(tmpRow);
            element->setParentElement(tmpRow);
            m_currentElement->replaceChild(oldchild,tmpRow);
        }
    } else if (insideEmptyElement()) {
        m_currentElement->parentElement()->replaceChild(m_currentElement,element);
        element->setParentElement(m_currentElement->parentElement());
        delete m_currentElement;
    } else {
        return 0;
    }
    m_currentElement = element;
    m_position=0;
    moveToEmpty();
    return undo;
}

FormulaCommand* FormulaCursor::remove( bool elementBeforePosition )
{
    FormulaCommand *undo=0;
    if (insideInferredRow()) {
        RowElement* tmprow=static_cast<RowElement*>(m_currentElement);
        //TODO: store the element for undo/redo
        if (isSelecting()) {
            foreach (BasicElement* tmp, tmprow->elementsBetween(selection().first,selection().second)) {
                tmprow->removeChild(tmp);
            }
            if (m_mark<m_position) {
                m_position=m_mark;
            }
            setSelecting(false);
        } else {
            if (elementBeforePosition && !isHome()) {
                m_position--;
                tmprow->removeChild(m_currentElement->childElements()[m_position]);
            } else if (!elementBeforePosition && !isEnd()) {
                tmprow->removeChild(m_currentElement->childElements()[m_position]);
            }
        }
        if (tmprow->length()==0) {
            BasicElement* empty=new EmptyElement(tmprow);
            tmprow->insertChild(0,empty);
        }
    } else if (insideToken()) {
        TokenElement* tmptoken=static_cast<TokenElement*>(m_currentElement);
        if (hasSelection()) {
            undo=new FormulaCommandRemoveText(tmptoken,selection().first,selection().second-selection().first,undo);
        } else {
            if (elementBeforePosition && !isHome()) {
                undo=new FormulaCommandRemoveText(tmptoken,m_position-1,1,undo);
            } else if (!elementBeforePosition && !isEnd()) {
                undo=new FormulaCommandRemoveText(tmptoken,m_position,1,undo);
            }
        }
    } else if (insideFixedElement()) {
        FixedElement* tmpfixed=static_cast<FixedElement*>(m_currentElement);
        if (hasSelection()) {
            foreach (BasicElement* tmp, tmpfixed->elementsBetween(selection().first,selection().second)) {
                BasicElement* newelement=new EmptyElement(m_currentElement);
                tmpfixed->replaceChild(tmp,newelement);
            }
        } else {
            
            BasicElement* newelement=new EmptyElement(m_currentElement);
            if (elementBeforePosition && (m_currentElement->elementBefore(m_position)!=0)) {
                    tmpfixed->replaceChild(m_currentElement->elementBefore(m_position),newelement);
            } else if (!elementBeforePosition && (m_currentElement->elementAfter(m_position)!=0)) {
                    tmpfixed->replaceChild(m_currentElement->elementAfter(m_position),newelement);
            }
        }
    }
    moveToEmpty();
    return undo;
}

void FormulaCursor::move( CursorDirection direction )
{
    FormulaCursor oldcursor(*this);
    m_direction = direction;
    if (performMovement(direction,&oldcursor)==false) {
        (*this)=oldcursor;
    } else {
        moveToEmpty();
    }
    m_direction=NoDirection;
}

bool FormulaCursor::moveCloseTo(BasicElement* element, FormulaCursor* cursor) 
{    
    if (element->setCursorTo(this,cursor->getCursorPosition()-element->absoluteBoundingRect().topLeft())) {
        moveToEmpty();
        return true;
    } else {
        return false;
    }
}


QPointF FormulaCursor::getCursorPosition() 
{
    return ( m_currentElement->cursorLine(m_position).p1()
           + m_currentElement->cursorLine(m_position).p2())/2.;
}


void FormulaCursor::moveTo( BasicElement* element, int position )
{
        m_currentElement = element;
        m_position = position;
}

void FormulaCursor::setCursorTo( const QPointF& point )
{
    if (m_selecting) {
        while (!m_currentElement->absoluteBoundingRect().contains(point)) {
            if ( m_currentElement->parentElement() ) {
                m_position=0;
                if (point.x()<m_currentElement->cursorLine(m_mark).p1().x()) {
                    //the point is left of the old selection start, so we move the selection 
                    //start after the old current element
                    m_mark=m_currentElement->parentElement()->positionOfChild(m_currentElement)+1;
                } else {
                    m_mark=m_currentElement->parentElement()->positionOfChild(m_currentElement);
                }
                m_currentElement=m_currentElement->parentElement();
            } else {
                return;
            }
        }
        while (!m_currentElement->setCursorTo(this,point-m_currentElement->absoluteBoundingRect().topLeft())) {
            if ( m_currentElement->parentElement() ) {
                m_mark=m_currentElement->parentElement()->positionOfChild(m_currentElement);
                m_position=0;
                if (point.x()<m_currentElement->cursorLine(m_mark).p1().x()) {
                    //the point is left of the old selection start, so we move the selection 
                    //start after the old current element
                    m_mark++;
                }
                m_currentElement=m_currentElement->parentElement();
            } else {
                    return;
            }
        }
    } else {
        BasicElement* formulaElement = m_currentElement;
        while( formulaElement->parentElement() != 0 ) {
            formulaElement = formulaElement->parentElement();
        }
        formulaElement->setCursorTo(this,point);
    }
    moveToEmpty();
}

int FormulaCursor::mark() const 
{
    return m_mark;
}

void FormulaCursor::moveHome()
{
    m_position = 0;
}

void FormulaCursor::moveEnd()
{
    m_position=m_currentElement->length();
}

bool FormulaCursor::isHome() const
{
    return m_position == 0;
}

bool FormulaCursor::isEnd() const
{
    return m_position == m_currentElement->length();
}

bool FormulaCursor::insideToken() const
{
    if( m_currentElement->elementType() == Number ||
        m_currentElement->elementType() == Operator ||
        m_currentElement->elementType() == Identifier ) {
        return true;
    }
    return false;
}

bool FormulaCursor::insideInferredRow() const
{
    if( m_currentElement->elementType() == Row ||
        m_currentElement->elementType() == SquareRoot ||
        m_currentElement->elementType() == Formula ||
        m_currentElement->elementType() == Fenced ) {
        return true;
    }
    return false;
}

bool FormulaCursor::insideFixedElement() const
{
    if (m_currentElement->elementType() == Fraction ||
        m_currentElement->elementType() == Root ||
        m_currentElement->elementType() == SubScript ||
        m_currentElement->elementType() == SupScript ||
        m_currentElement->elementType() == SubScript ||
        m_currentElement->elementType() == SubSupScript ) {
        return true;
    }
    return false;
}
 

bool FormulaCursor::insideEmptyElement() const
{
    return (m_currentElement->elementType() == Empty);
}


BasicElement* FormulaCursor::currentElement() const
{
    return m_currentElement;
}

int FormulaCursor::position() const
{
    return m_position;
}

void FormulaCursor::setCurrentElement(BasicElement* element) {
    m_currentElement=element;
}


void FormulaCursor::setData ( FormulaData* data )
{
    m_data=data;
}


FormulaData* FormulaCursor::formulaData() const
{
    return m_data;
}


void FormulaCursor::setPosition(int position) {
    m_position=position;
}

CursorDirection FormulaCursor::direction() const
{
    return m_direction;
}

QString FormulaCursor::inputBuffer() const
{
    return m_inputBuffer;
}

bool FormulaCursor::isSelecting() const
{
    return m_selecting;
}

void FormulaCursor::setSelecting( bool selecting )
{
    if (selecting) {
        if (!m_selecting) {
            //we start a new selection
            m_selecting = selecting;
            m_mark=m_position;
        }
    } else {
        m_selecting = selecting;
        m_mark=0;
    }
}

void FormulaCursor::setSelectionStart(int position) {
    m_mark=position;
}


QPair< int,int > FormulaCursor::selection() const
{
    if (m_mark<m_position) {
        return QPair<int,int>(m_mark,m_position);
    } else {
        return QPair<int,int>(m_position,m_mark);
    }
}


bool FormulaCursor::hasSelection() const
{
    return (m_selecting && m_mark!=m_position);
}


bool FormulaCursor::isAccepted() const
{
    if (mark()<0 || mark()>m_currentElement->length() ||
        position()<0 || position()>m_currentElement->length()) {
        return false;
    }
    if ((m_direction==MoveLeft || m_direction==MoveRight) && !m_selecting) {
        return (m_currentElement->acceptCursor(this) && !nextToEmpty());
    } else {
        return (m_currentElement->acceptCursor(this));
    }
}

BasicElement* FormulaCursor::nextToEmpty() const
{
    BasicElement* before=m_currentElement->elementBefore(m_position);
    BasicElement* after=m_currentElement->elementAfter(m_position);
    
    if (before!=0 ) {
        if (before->elementType()==Empty) {
            return before;
        }
    } else if (after != 0) {
        if (after->elementType()==Empty) {
            return after;
        }
    }
    return 0;
}


bool FormulaCursor::moveToEmpty() 
{
    BasicElement* tmp=nextToEmpty();
    if (tmp!=0) {
        moveTo(tmp,0);
        if (isSelecting()) {
            m_mark=0;
        }
    }
}


QString FormulaCursor::tokenType ( const QChar& character ) const
{
    
    if (character.isNumber()) {
        return "mn";
    }
    else if ( character == '*' || character == '+' || character == '-' ||
            character == '=' || character == '>' || character == '<' ||
            character == '!' || character == '.' || character == '/' ||
            character == '?' || character == ',' || character == ':' ||
            character == '(' || character == ')' ||
            character == '[' || character == ']' ||
            character == '{' || character == '}' ) {
        return "mo";
    }
    else if (character.isLetter()) {
        return "mi";
    }
    return "mi";
}



bool FormulaCursor::performMovement ( CursorDirection direction, FormulaCursor *oldcursor )
{
    
    //handle selecting and not selecting case seperately, which makes more clear
    if (isSelecting()) {
        while ( m_currentElement ) {
            if ( m_currentElement->moveCursor( this, oldcursor ) ) {
                if (isAccepted()) {
                    return true;
                }
            } else {
                if ( m_currentElement->parentElement() ) {
                    bool ltr=m_mark<=m_position;
                    //update the starting point of the selection
                    m_mark=m_currentElement->parentElement()->positionOfChild(m_currentElement);
                    //move the cursor to the parent and place it before the old element
                    m_position=m_currentElement->parentElement()->positionOfChild(m_currentElement);
                    m_currentElement=m_currentElement->parentElement();
                    if (ltr) {
                        m_position++; //place the cursor behind
                    } else {
                        m_mark++; //place the selection beginning behind 
                    }
                    if (isAccepted()) {
                        return true;
                    }
                } else {
                    //we arrived at the toplevel element
                    return false;
                }
            }
        }
    } else {
        while ( m_currentElement ) {
            if ( m_currentElement->moveCursor( this, oldcursor ) ) {
                if (isAccepted()) {
                    return true;
                }
            } else {
                if ( m_currentElement->parentElement() ) {
                    //move the cursor to the parent and place it before the old element
                    m_position=m_currentElement->parentElement()->positionOfChild(m_currentElement);
                    m_currentElement=m_currentElement->parentElement();
                    if (m_direction==MoveRight || m_direction==MoveDown) {
                        m_position++; //place the cursor behin
                    }
                    if (m_direction==MoveRight || m_direction==MoveLeft) {
                        if (isAccepted()) {
                            return true;
                        }
                    }   
                } else {
                    //We arrived at the top level element
                    return false;
                }
            }
        }
    }
}


