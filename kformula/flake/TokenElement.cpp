/* This file is part of the KDE project
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

#include "TokenElement.h"
#include "AttributeManager.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>
#include <kdebug.h>

TokenElement::TokenElement( BasicElement* parent ) : BasicElement( parent )
{}

const QList<BasicElement*> TokenElement::childElements()
{
    // only return the mglyph elements
    QList<BasicElement*> tmpList;
    foreach( BasicElement* tmp, m_content )
        if( tmp != this )
            tmpList << tmp;

    return tmpList;
}

void TokenElement::paint( QPainter& painter, AttributeManager* am )
{
    // set the painter to background color and paint it
    painter.setPen( am->colorOf( "mathbackground", this ) );
    painter.setBrush( QBrush( painter.pen().color() ) );
    painter.drawRect( QRectF( 0.0, 0.0, width(), height() ) );

    // set the painter to foreground color and paint the text in the content path
    painter.setPen( am->colorOf( "mathcolor", this ) );
    painter.setBrush( QBrush( painter.pen().color() ) );
    painter.translate( 0, baseLine() );
    painter.drawPath( m_contentPath );
}

void TokenElement::layout( const AttributeManager* am )
{
    kDebug( DEBUGID ) << "TokenElement::layout()";
    kDebug( DEBUGID ) << m_content.count();

    m_contentPath = QPainterPath();             // save the token in an empty path
    int rawCounter = 0;
    foreach( BasicElement* tmp, m_content )
        if( tmp == this )                       // a normal token
        {
            kDebug( DEBUGID ) << "renderToPath";
            renderToPath( m_rawStringList[ rawCounter ], m_contentPath );
            rawCounter++;
        }
        else                                    // a mglyph element
        {
            kDebug() << "glyph element ???";
            tmp->setOrigin( QPointF( m_contentPath.boundingRect().right(), 0.0 ) );
            m_contentPath.moveTo( tmp->origin().x()+ tmp->width(), 0.0 );
        }    
    
    // As the text is added to ( 0 / 0 ) the baseline equals the top edge of the
    // elements bounding rect, while translating it down the text's baseline moves too
    setBaseLine( -m_contentPath.boundingRect().y() ); // set baseline accordingly
    setWidth( m_contentPath.boundingRect().right() );
    setHeight( m_contentPath.boundingRect().height() );
}

BasicElement* TokenElement::acceptCursor( const FormulaCursor* cursor )
{
    return 0;
}

bool TokenElement::readMathMLContent( const KoXmlElement& element )
{
    BasicElement* tmpGlyph;
//    kDebug() << "child element count: " << element.childNodesCount();
    KoXmlNode node = element.firstChild();
    while( !node.isNull() ) {
        if( node.isElement() ) {
            KoXmlElement tmp = node.toElement();
            tmpGlyph = ElementFactory::createElement( tmp.tagName(), this );
            m_content << tmpGlyph;
            tmpGlyph->readMathML( tmp );
        }
        else {
            m_rawStringList << node.toText().data().trimmed();
//            kDebug() << "Node text: " << node.toText().data().trimmed();
            m_content << this;
        }
        node = node.nextSibling();
    }
    return true;
}

void TokenElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    int rawCounter = 0;
    foreach( BasicElement* tmp, m_content )
        if( tmp == this )
        {
            writer->addTextNode( m_rawStringList[ rawCounter ] );
            rawCounter++;
        }
        else
            tmp->writeMathML( writer );
}

