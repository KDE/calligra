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
    delete m_underElement;
    delete m_overElement;
}

const QList<BasicElement*> UnderOverElement::childElements()
{
    QList<BasicElement*> tmp;
    return tmp << m_baseElement << m_underElement << m_overElement;
}

void UnderOverElement::paint( QPainter& , AttributeManager* )
{ /*do nothing as UnderOverElement has no visual representance*/ }

void UnderOverElement::layout( const AttributeManager* am )
{
    Q_UNUSED( am ) 
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
    KoXmlElement child = parent.firstChild().toElement();
    QString name = parent.tagName().toLower();
    
    if ( child.isNull() ) {
        kWarning( 39001 ) << "Empty content in " << name << " element\n";
        return false;
    }
    delete m_baseElement;
    m_baseElement = ElementFactory::createElement( child.tagName(), this );

    if( name.contains( "under" ) ) {
        child = child.nextSibling().toElement();
        if ( child.isNull() ) {
            kWarning( 39001 ) << "Empty underscript in " << name << " element\n";
            return false;
        }
        delete m_underElement;
        m_underElement = ElementFactory::createElement( child.tagName(), this );
    }
    if ( name.contains( "over" ) ) {
        child = child.nextSibling().toElement();
        if ( child.isNull() ) {
            kWarning( 39001 ) << "Empty overscript in " << name << " element\n";
            return false;
        }
        delete m_overElement;
        m_overElement = ElementFactory::createElement( child.tagName(), this );
    }
    return true;
} 

void UnderOverElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_baseElement->writeMathML( writer );        // Just save the children in
                                                 // the right order
    if( m_underElement->elementType() != Basic )
        m_underElement->writeMathML( writer );
    
    if( m_overElement->elementType() != Basic )
        m_overElement->writeMathML( writer );
}

ElementType UnderOverElement::elementType() const
{
    if( m_underElement->elementType() != Basic && m_overElement->elementType() != Basic )
        return UnderOver;
    else if( m_underElement->elementType() != Basic )
        return Under;
    else if( m_overElement->elementType() != Basic )
        return Over;
    else
        return UnderOver;
}
