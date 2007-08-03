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

#include "RowElement.h"
#include "kformuladefs.h"
#include "ElementFactory.h"
#include "FormulaCursor.h"
#include <kdebug.h>
#include <KoXmlWriter.h>
#include <QPainter>

RowElement::RowElement( BasicElement* parent ) : BasicElement( parent )
{}

RowElement::~RowElement()
{
    qDeleteAll( m_childElements );
}

void RowElement::layout( const AttributeManager* am )
{
    kDebug(39001) <<"layout Row";

    Q_UNUSED( am )          // there are no attributes that can be processed here

    if( m_childElements.isEmpty() )  // do not do anything if there are no children
        return;

    QPointF origin;
    double width = 0.0;
    double topToBaseline = 0.0;
    double baselineToBottom = 0.0;
    foreach( BasicElement* child, m_childElements  )  // iterate through the children
    {
        kDebug(39001) <<"new origin:" << QPointF( width, 0.0 );
        child->setOrigin( QPointF( width, 0.0 ) );    // set their origin
        topToBaseline = qMax( topToBaseline, child->baseLine() );
        baselineToBottom = qMax( baselineToBottom, child->height()-child->baseLine() );
        width += child->width();       // add their width
    }

    setWidth( width );
    setHeight( topToBaseline + baselineToBottom );
    setBaseLine( topToBaseline );
}

void RowElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
}

void RowElement::removeChild( BasicElement* element )
{
}

void RowElement::moveLeft( FormulaCursor* cursor, BasicElement* from )
{
    if( from == parentElement() )  // parent element enters the seqeunce from the left
        cursor->moveCursorTo( this, m_childElements.count() );
    else if( from == this )        // moveLeft was invoked in this element
        m_childElements[ cursor->position()-1 ]->moveLeft( cursor, this );
    else                           // the cursor comes from a child element
        cursor->moveCursorTo( this, m_childElements.indexOf( from ) );
}

void RowElement::moveRight( FormulaCursor* cursor, BasicElement* from )
{
    if( from == parentElement() )  // parent element enters the seqeunce from the right
        cursor->moveCursorTo( this, 0 );
    else if( from == this )        // moveRight was invoked in this element
        m_childElements[ cursor->position() ]->moveRight( cursor, this );
    else                           // the cursor comes from a child element
        cursor->moveCursorTo( this, m_childElements.indexOf( from )+1 );
}

QRectF RowElement::childBoundingRect()
{
    return QRectF();
}

ElementType RowElement::elementType() const
{
    return Row;
}


bool RowElement::readMathMLContent( const KoXmlElement& parent )
{
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmp, parent )
    {
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        m_childElements << tmpElement;
        tmpElement->readMathML( tmp );
    }

    kWarning( DEBUGID ) << "Loaded " << m_childElements.count() << " inside Row element";
    return true;
}

void RowElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    foreach( BasicElement* tmp, m_childElements )
        tmp->writeMathML( writer );
}

const QList<BasicElement*> RowElement::childElements()
{
    return m_childElements;
}

