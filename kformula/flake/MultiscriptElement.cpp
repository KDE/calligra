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

#include "MultiscriptElement.h"
#include "AttributeManager.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>

MultiscriptElement::MultiscriptElement( BasicElement* parent ) : BasicElement( parent )
{
    m_baseElement = new BasicElement( this );
    //For now, we will only worry about msub, msup and msubsup
    //All of these will need to become QList<BasicElement*> for mmultiscript support
//    m_preSubscript = new BasicElement( this );
//    m_preSuperscript = new BasicElement( this );
    m_postSubscript = new BasicElement( this );
    m_postSuperscript = new BasicElement( this );
}

MultiscriptElement::~MultiscriptElement()
{
    delete m_baseElement;
    //delete m_preSubscript;
    //	delete m_preSuperscript;
    delete m_postSubscript;
    delete m_postSuperscript;
}

void MultiscriptElement::paint( QPainter& painter, AttributeManager* am )
{ 
    Q_UNUSED(painter)
    Q_UNUSED(am)
    /*do nothing as UnderOverElement has no visual representance*/
}

void MultiscriptElement::layout( const AttributeManager* am )
{
    // Get the minimum amount of shifting
    double subscriptshift   = am->doubleOf( "subscriptshift", this ); 
    double superscriptshift = am->doubleOf( "superscriptshift", this );
    
    // The yOffset is the amount the base element is moved down to make
    // room for the superscript
    double yOffset = 0;
    if(m_postSuperscript) {
        yOffset = m_postSuperscript->height() - m_baseElement->height()/2;
        yOffset = qMax( yOffset, superscriptshift );
    }
    double largestWidth;
    if(m_postSubscript)
        largestWidth = m_postSubscript->width();
    if(m_postSuperscript) {
        largestWidth = qMax( largestWidth, m_postSuperscript->width());
        m_postSuperscript->setOrigin( QPointF( m_baseElement->width(), 0) );
    }
    setWidth( m_baseElement->width() + largestWidth );
    
    m_baseElement->setOrigin( QPointF( 0, yOffset ) );

    if(m_postSubscript) {
        double yPos = yOffset +
	       	qMax( m_baseElement->height()/2, 
		      m_baseElement->height() - m_postSubscript->baseLine() 
		          + subscriptshift );
        m_postSubscript->setOrigin( QPointF( m_baseElement->width(), yPos ) );
	setHeight( yPos + m_postSubscript->height() );
    } else
        setHeight( yOffset + m_baseElement->height() );
}

BasicElement* MultiscriptElement::acceptCursor( CursorDirection direction )
{
    return 0;
}

const QList<BasicElement*> MultiscriptElement::childElements()
{
    QList<BasicElement*> tmp;
    tmp << m_baseElement;
    if(m_postSubscript)
	    tmp << m_postSubscript;
    if(m_postSuperscript)
	    tmp << m_postSuperscript;
    return tmp;
}

QString MultiscriptElement::attributesDefaultValue( const QString& attribute ) const
{
    return QString();
}

ElementType MultiscriptElement::elementType() const
{
    if( m_postSubscript && m_postSuperscript )
        return SubSupScript;
    else if( m_postSubscript )
        return SubScript;
    else if( m_postSuperscript )
        return SupScript;
    else
        return MultiScript;
    //multiscript not yet supported
    //return MultiScripts;
}

bool MultiscriptElement::readMathMLContent( const KoXmlElement& parent )
{
    QString name = parent.tagName().toLower();
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;

    if(name == "mmultiscripts") return false;  //Not yet coded
    if(!name.contains( "sub" )) {
        delete m_postSubscript;
	m_postSubscript = NULL;
    }
    if(!name.contains( "sup" )) {
        delete m_postSuperscript;
	m_postSuperscript = NULL;
    }
    forEachElement( tmp, parent ) { 
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( !tmpElement->readMathML( tmp ) )
            return false;

        if( m_baseElement->elementType() == Basic ) {
            delete m_baseElement; 
            m_baseElement = tmpElement;
        }
        else if( m_postSubscript && m_postSubscript->elementType() == Basic ) {
            delete m_postSubscript;
            m_postSubscript = tmpElement;
	    Q_ASSERT(m_postSubscript);
        }
        else if( m_postSuperscript && m_postSuperscript->elementType() == Basic ) {
            delete m_postSuperscript;
            m_postSuperscript = tmpElement;
	    Q_ASSERT(m_postSuperscript);
        }
        else
            return false;
    }
    Q_ASSERT(m_baseElement);  //We should have at least a BasicElement for the base
    Q_ASSERT(m_postSubscript || m_postSuperscript);
    return true;
}

void MultiscriptElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_baseElement->writeMathML( writer );        // Just save the children in
                                                 // the right order
    if( m_postSubscript )
        m_postSubscript->writeMathML( writer );
    
    if( m_postSuperscript )
        m_postSuperscript->writeMathML( writer );
}

