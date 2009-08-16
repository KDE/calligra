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

#include "SubSupElement.h"
#include "FormulaCursor.h"
#include "AttributeManager.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>
#include <kdebug.h>

SubSupElement::SubSupElement( BasicElement* parent, ElementType elementType ) : FixedElement( parent )
{
    m_baseElement = new RowElement( this );
    m_subScript = new RowElement( this );
    m_superScript = new RowElement( this );
    m_elementType = elementType;
}

SubSupElement::~SubSupElement()
{
    delete m_baseElement;
    delete m_subScript;
    delete m_superScript;
}

void SubSupElement::paint( QPainter& painter, AttributeManager* am )
{ 
    Q_UNUSED(painter)
    Q_UNUSED(am)
    /*do nothing as this element has no visual representation*/
}

void SubSupElement::layout( const AttributeManager* am )
{
    // Get the minimum amount of shifting
    double subscriptshift   = am->doubleOf( "subscriptshift", this ); 
    double superscriptshift = am->doubleOf( "superscriptshift", this );
    double halfthinSpace   = 0;

    if(m_elementType == SubSupScript) {
        //Add half a thin space between both sup and superscript, so there is a minimum
        //of a whole thin space between them.
        halfthinSpace = am->layoutSpacing( this )/2.0;
    }
    
    // The yOffset is the amount the base element is moved down to make
    // room for the superscript
    double yOffset = 0;
    if(m_superScript) {
        yOffset = m_superScript->height() - m_baseElement->height()/2 + halfthinSpace;
        yOffset = qMax( yOffset, superscriptshift );
    }
    double largestWidth = 0;
    if(m_subScript) {
        largestWidth = m_subScript->width();
    }
    if(m_superScript) {
        largestWidth = qMax( largestWidth, m_superScript->width());
        m_superScript->setOrigin( QPointF( m_baseElement->width(), 0) );
    }

    setWidth( m_baseElement->width() + largestWidth );
    setBaseLine( yOffset + m_baseElement->baseLine() );
    m_baseElement->setOrigin( QPointF( 0, yOffset ) );


    if(m_subScript) {
        double yPos = yOffset +
	       	qMax( m_baseElement->height()/2 + halfthinSpace, 
		      m_baseElement->height() - m_subScript->baseLine() 
		          + subscriptshift );
        m_subScript->setOrigin( QPointF( m_baseElement->width(), yPos ) );
	setHeight( yPos + m_subScript->height() );
    } else {
        setHeight( yOffset + m_baseElement->height() );
    }
}

// bool SubSupElement::acceptCursor( const FormulaCursor* cursor )
// {
//     return true;
// }

const QList<BasicElement*> SubSupElement::childElements() const
{
    QList<BasicElement*> tmp;
    tmp << m_baseElement;
    tmp << m_superScript;
    tmp << m_subScript;
    return tmp;
}

// void SubSupElement::insertChild( FormulaCursor* cursor, BasicElement* child )
// {
//     if( cursor->currentElement() == m_baseElement )
//         m_baseElement = child;
//     else if( cursor->currentElement() == m_subScript )
//         m_subScript = child;
//     else if( cursor->currentElement() == m_superScript )
//         m_superScript = child;
// //    else
// //    TODO add some error
// }

QString SubSupElement::attributesDefaultValue( const QString& attribute ) const
{
    return QString();
}

ElementType SubSupElement::elementType() const
{
//      return m_elementType;
    // Should we decide the type also on whether the user has entered text for the sup and sub parts?
 /*    if( m_subScript->elementType() != Basic &&
        m_superScript->elementType() != Basic )
        return SubSupScript;
    else if( m_subScript->elementType() != Basic )
        return SubScript;
    else if( m_superScript->elementType() != Basic )
        return SupScript;
        return Unknown;*/
    return Unknown;
}

bool SubSupElement::readMathMLContent( const KoXmlElement& parent )
{
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    bool baseElement=true;
    bool subScript=true;
    bool superScript=true;
    
    forEachElement( tmp, parent ) { 
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( !tmpElement->readMathML( tmp ) ) {
            return false;
        }

        if( baseElement ) {
            delete m_baseElement; 
            m_baseElement = tmpElement;
            baseElement=false;
        } else if( subScript && m_elementType != SupScript) {
            delete m_subScript;
            m_subScript = tmpElement;
            subScript = false;
        } else if( superScript ) {
            delete m_superScript;
            m_superScript = tmpElement;
            superScript = false;
        }
        else
            return false;
    }
    return true;
}

void SubSupElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    // just save the children in the right order
    m_baseElement->writeMathML( writer );

    if( m_subScript->elementType() != Empty )
        m_subScript->writeMathML( writer );

    if( m_superScript->elementType() != Empty )
        m_superScript->writeMathML( writer );
}


int SubSupElement::length() const
{
    return 5;
}


bool SubSupElement::setCursorTo(FormulaCursor& cursor, QPointF point)
{
    if (cursor.isSelecting()) {
        return false;
    }
    if (point.x() <= m_baseElement->boundingRect().right()) {
        return m_baseElement->setCursorTo(cursor, point-m_baseElement->origin());
    } else {
        if (point.y()<=(m_subScript->boundingRect().top()+m_superScript->boundingRect().bottom())/2) {
            return m_superScript->setCursorTo(cursor, point-m_superScript->origin());
        } else {
            return m_subScript->setCursorTo(cursor, point-m_subScript->origin());
        }
    }
}


bool SubSupElement::moveCursor ( FormulaCursor& newcursor, FormulaCursor& oldcursor )
{
    // 0^1_2
    int childpos=newcursor.position()/2;
    kDebug()<<"USBSUSABSDUASD "<<childpos;
    switch( newcursor.direction()) {
    case MoveUp:
    case MoveDown:
        switch (childpos) {
        case 1:
        case 2:
            return moveVertSituation(newcursor,oldcursor,1,2);
            break;
        case 0:
            if (newcursor.direction()==MoveDown) {
                return moveHorSituation(newcursor,oldcursor,1,0);
            } else {
                return moveHorSituation(newcursor,oldcursor,0,2);
            }
            break;
        }
        break;
    case MoveLeft:
    case MoveRight:
        switch (childpos) {
        case 0:
            return moveHorSituation(newcursor,oldcursor,0,1);
            break;
        case 1:
            return moveHorSituation(newcursor,oldcursor,0,1);
            break;
        case 2:
            return moveHorSituation(newcursor,oldcursor,0,2);
        }
        break;
    }
    return false;
}
