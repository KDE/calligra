/* This file is part of thShapee KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include "UnderOverElement.h"
#include "AttributeManager.h"
#include <KoXmlReader.h>
#include <kdebug.h>

UnderOverElement::UnderOverElement( BasicElement* parent ) : BasicElement( parent )
{
    m_baseElement = new BasicElement( this );
    m_underElement = new BasicElement( this );
    m_overElement = new BasicElement( this );
}

UnderOverElement::~UnderOverElement()
{
    delete m_baseElement;
    if(m_underElement)
        delete m_underElement;
    if(m_overElement)
        delete m_overElement;
}

const QList<BasicElement*> UnderOverElement::childElements()
{
    QList<BasicElement*> tmp;
    tmp << m_baseElement;
    if(m_underElement) tmp << m_underElement;
    if(m_overElement)  tmp << m_overElement;
    return tmp;
}

void UnderOverElement::paint( QPainter& painter, AttributeManager* am)
{
    /*do nothing as UnderOverElement has no visual representance*/
    Q_UNUSED(painter)
    Q_UNUSED(am)
}

void UnderOverElement::layout( const AttributeManager* am )
{
    double thinSpace   = am->mathSpaceValue( "thinmathspace" );
    double accent      = am->boolOf( "accent", this );  //Whether to add a space above
    double accentunder = am->boolOf( "accentunder", this );//Whether to add a space below
    double yOffset = 0.0;
    double largestWidth = m_baseElement->width();
    if(m_underElement) 
        largestWidth = qMax(m_underElement->width(), largestWidth);
    if(m_overElement) {
        yOffset = m_overElement->height();
        if(accent) yOffset += thinSpace;
        largestWidth = qMax(m_overElement->width(), largestWidth);
        m_overElement->setOrigin( 
			QPointF((largestWidth - m_overElement->width())/2.0, 0.0));
    }
    m_baseElement->setOrigin(
		    QPointF((largestWidth - m_baseElement->width())/2.0, yOffset));
    setWidth(largestWidth);
    setBaseLine( yOffset + m_baseElement->baseLine() );
    yOffset += m_baseElement->height();

    if(m_underElement) {
        if(accentunder) yOffset += thinSpace;
        m_underElement->setOrigin( 
			QPointF( (largestWidth - m_underElement->width())/2.0, yOffset));
	yOffset += m_underElement->height();
    }
    setHeight(yOffset);
   
}

BasicElement* UnderOverElement::acceptCursor( CursorDirection direction )
{
    return 0;
}

QString UnderOverElement::attributesDefaultValue( const QString& attribute ) const
{
/*    if( m_overElement->elementType() == Operator )
    else if( m_underElement->elementType() == Operator )*/
    return "false";  // the default for accent and
}

bool UnderOverElement::readMathMLContent( const KoXmlElement& parent )
{
    QString name = parent.tagName().toLower();
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    if(!name.contains( "under" )) {
        delete m_underElement;
	m_underElement = NULL;
    }
    if(!name.contains( "over" )) {
        delete m_overElement;
	m_overElement = NULL;
    }
    forEachElement( tmp, parent ) { 
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( !tmpElement->readMathML( tmp ) )
            return false;

        if( m_baseElement->elementType() == Basic ) {
            delete m_baseElement; 
            m_baseElement = tmpElement;
        }
        else if( m_underElement && m_underElement->elementType() == Basic ) {
            delete m_underElement;
            m_underElement = tmpElement;
	    Q_ASSERT(m_underElement);
        }
        else if( m_overElement && m_overElement->elementType() == Basic ) {
            delete m_overElement;
            m_overElement = tmpElement;
	    Q_ASSERT(m_overElement);
        }
        else
            return false;
    }
    Q_ASSERT(m_baseElement);  //We should have at least a BasicElement for the base
    Q_ASSERT(m_underElement || m_overElement);
    return true;
} 

void UnderOverElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_baseElement->writeMathML( writer );        // Just save the children in
                                                 // the right order
    if( m_underElement )
        m_underElement->writeMathML( writer );
    
    if( m_overElement )
        m_overElement->writeMathML( writer );
}

ElementType UnderOverElement::elementType() const
{
    if( m_underElement && m_overElement )
        return UnderOver;
    else if( m_underElement )
        return Under;
    else if( m_overElement )
        return Over;
    else
        return UnderOver;
}
