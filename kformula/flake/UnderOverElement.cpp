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
#include "ElementFactory.h"
#include "RowElement.h"
#include "kformuladefs.h"
#include <kdebug.h>
#include <KoXmlWriter.h>

#include <assert.h>

UnderOverElement::UnderOverElement( BasicElement* parent ) : BasicElement( parent )
{
    m_baseElement = new RowElement( this );
    m_underElement = 0;
    m_overElement = 0;
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
    if ( m_baseElement ) {
        tmp << m_baseElement;
    }
    if ( m_underElement ) {
        tmp << m_underElement;
    }
    if ( m_overElement ) {
        tmp << m_overElement;
    }
    return tmp;
}

void UnderOverElement::paint( QPainter& painter, const AttributeManager* am )
{
}

void UnderOverElement::layout( const AttributeManager* am )
{
}

void UnderOverElement::moveLeft( FormulaCursor* cursor, BasicElement* from )
{
}

void UnderOverElement::moveRight( FormulaCursor* cursor, BasicElement* from )
{
}

void UnderOverElement::moveUp( FormulaCursor* cursor, BasicElement* from )
{
}

void UnderOverElement::moveDown( FormulaCursor* cursor, BasicElement* from )
{
}

bool UnderOverElement::readMathMLContent( const KoXmlElement& parent )
{
    BasicElement* tmpElement = 0;
    KoXmlElement child = parent.firstChildElement();
    QString name = parent.tagName().toLower();
    
    if ( child.isNull() ) {
        kWarning( DEBUGID ) << "Empty content in " << name << " element\n";
        return false;
    }
    delete m_baseElement;
    m_baseElement = new RowElement( this );
    if ( child.tagName().toLower() == "mrow" ) { // Inferred mrow ?
        if ( ! m_baseElement->readMathMLContent( child ) ) return false;
    }
    else {
        if ( ! m_baseElement->readMathMLChild( child ) ) return false;
    }
    if ( name.contains( "under" ) ) {
        child = child.nextSiblingElement();
        if ( child.isNull() ) {
            kWarning( DEBUGID ) << "Empty underscript in " << name << " element\n";
            return false;
        }
        delete m_underElement;
        m_underElement = new RowElement( this );
        if ( child.tagName().toLower() == "mrow" ) { // Inferred mrow ?
            if ( ! m_underElement->readMathMLContent( child ) ) return false;
        }
        else {
            if ( ! m_underElement->readMathMLChild( child ) ) return false;
        }
    }
    if ( name.contains( "over" ) ) {
        child = child.nextSiblingElement();
        if ( child.isNull() ) {
            kWarning( DEBUGID ) << "Empty overscript in " << name << " element\n";
            return false;
        }
        delete m_overElement;
        m_overElement = new RowElement( this );
        if ( child.tagName().toLower() == "mrow" ) { // Inferred mrow ?
            if ( ! m_overElement->readMathMLContent( child ) ) return false;
        }
        else {
            if ( ! m_overElement->readMathMLChild( child ) ) return false;
        }
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
    else {
        assert( m_overElement->elementType() != Basic );
        return Over;
    }
}
