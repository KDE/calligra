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
#include "FormulaCursor.h"
#include "GlyphElement.h"
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
    foreach( GlyphElement* tmp, m_glyphs )
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
    // Query the font to use
    m_font = am->font( this );

    // save the token in an empty path
    m_contentPath = QPainterPath();

    // replace all the object replacement characters with glyphs
    QString chunk;
    int counter = 0;
    for( int i = 0; i < m_rawString.length(); i++ ) {
        if( m_rawString[ i ] != QChar::ObjectReplacementCharacter )
            chunk.append( m_rawString[ i ] );
        else {
            renderToPath( chunk, m_contentPath );
            m_glyphs[ counter ]->renderToPath( QString(), m_contentPath );
            counter++;
        }
    }

    // As the text is added to ( 0 / 0 ) the baseline equals the top edge of the
    // elements bounding rect, while translating it down the text's baseline moves too
    setBaseLine( -m_contentPath.boundingRect().y() ); // set baseline accordingly
    setWidth( m_contentPath.boundingRect().right() );
    setHeight( m_contentPath.boundingRect().height() );
}

void TokenElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
/*    if( child && child->elementType() == Glyph ) {
        m_rawString.insert( QChar( QChar::ObjectReplacementCharacter ) );
        m_glyphs.insert();
    }
    else*/ if( !child )
        m_rawString.insert( cursor->position(), cursor->inputBuffer() );
}

BasicElement* TokenElement::acceptCursor( const FormulaCursor* cursor )
{
    Q_UNUSED( cursor )
    return this;
}

double TokenElement::cursorOffset( const FormulaCursor* cursor ) const
{
    if( m_rawString.contains( QChar::ObjectReplacementCharacter ) ) {
        // TODO do something special
    }

    QFontMetricsF fm( m_font );
    return fm.width( m_rawString.left( cursor->position() ) );
}

QFont TokenElement::font() const
{
    return m_font;
}

bool TokenElement::readMathMLContent( const KoXmlElement& element )
{
    // iterate over all child elements ( possible embedded glyphs ) and put the text
    // content in the m_rawString and mark glyph positions with
    // QChar::ObjectReplacementCharacter
    GlyphElement* tmpGlyph;
    KoXmlNode node = element.firstChild();
    while( !node.isNull() ) {
        if( node.isElement() && node.toElement().tagName() == "mglyph" ) {
            tmpGlyph = new GlyphElement( this );
            m_rawString.append( QChar( QChar::ObjectReplacementCharacter ) );
            tmpGlyph->readMathML( node.toElement() );
        }
        else if( node.isElement() )
            return false;
        else
            m_rawString.append( node.toText().data().trimmed() );

        node = node.nextSibling();
    }
    return true;
}

void TokenElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    // split the m_rawString into text content chunks that are divided by glyphs 
    // which are represented as ObjectReplacementCharacter and write each chunk
    QStringList tmp = m_rawString.split( QChar( QChar::ObjectReplacementCharacter ) );
    for ( int i = 0; i < tmp.count(); i++ ) {
        if( m_rawString.startsWith( QChar( QChar::ObjectReplacementCharacter ) ) ) {
            m_glyphs[ i ]->writeMathML( writer );
            writer->addTextNode( tmp[ i ] );
        }
        else {
            writer->addTextNode( tmp[ i ] );       
            m_glyphs[ i ]->writeMathML( writer );
        }
    }
}

