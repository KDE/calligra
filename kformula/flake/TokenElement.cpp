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

void TokenElement::paint( QPainter& painter, const AttributeManager* am )
{
     QPointF tmpOrigin;
     int rawCounter = 0;
     foreach( BasicElement* tmp, m_content )
         if( tmp == this )
         {
             painter.drawText( tmpOrigin, stringToRender( m_rawStringList[ rawCounter ] ) );
             rawCounter++;
         }
         else
         {
             tmp->paint( painter, am );
             tmpOrigin = tmp->boundingRect().topRight(); // set origin for text
         }
}

void TokenElement::layout( const AttributeManager* am )
{
    int rawCounter = 0;
    double width = 0.0;
    double height = 0.0;
    QFontMetricsF fm( am->font() );
    QRectF tmpRect;
    foreach( BasicElement* tmp, m_content )
        if( tmp == this )
        {
            tmpRect = fm.boundingRect( stringToRender( m_rawStringList[ rawCounter ] ) );
            width += tmpRect.width();
            height = qMax( height, tmp->height() );
            rawCounter++;
        }
        else
        {
            tmp->setOrigin( QPointF( width, 0.0 ) );
            width += tmp->width();
            height = qMax( height, tmp->height() );
        }

    setWidth( width );
    setHeight( height );
    setBaseLine( height );  // TODO is this correct ??
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
