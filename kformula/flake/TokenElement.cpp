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
#include "ElementFactory.h"
#include <KoXmlWriter.h>
#include <QPainter>
#include <QFontMetricsF>

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
     painter.drawPath( m_contentPath );        // draw content 

     foreach( BasicElement* tmp, m_content )   // draw possible mglyph elements
         if( tmp != this )
             tmp->paint( painter, am );
}

void TokenElement::layout( AttributeManager* am )
{
    m_contentPath = QPainterPath();
    int rawCounter = 0;
    foreach( BasicElement* tmp, m_content )
        if( tmp == this )
        {
            m_contentPath.addText( 0.0, 0.0, am->font( this ),
                                   stringToRender( m_rawStringList[ rawCounter ] ) );
            rawCounter++;
        }
        else
        {
            tmp->setOrigin( QPointF( m_contentPath.boundingRect().width(), 0.0 ) );
            m_contentPath.moveTo( tmp->origin().x() + tmp->width(), 0.0 );
        }

    setWidth( m_contentPath.boundingRect().width() );
    setHeight( m_contentPath.boundingRect().height() );
    setBaseLine( height() );  // TODO is this correct ??

    QMatrix tmpMatrix;
    tmpMatrix.translate( 0, m_contentPath.boundingRect().height() );
    m_contentPath = tmpMatrix.map( m_contentPath );
}

bool TokenElement::readMathMLContent( const KoXmlElement& element )
{
    BasicElement* tmpGlyph;
    KoXmlNode node = element.firstChild();
    while (! node.isNull() ) {
        if( node.isElement() )
        {
            KoXmlElement tmp = node.toElement();
            tmpGlyph = ElementFactory::createElement( tmp.tagName(), this );
            m_content << tmpGlyph;
            tmpGlyph->readMathML( tmp );
        }
        else
        {
            m_rawStringList << node.toText().data().trimmed();
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

QString TokenElement::stringToRender( const QString& rawString ) const
{
    return rawString;
}
