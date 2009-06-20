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
#include "Dictionary.h"
#include "GlyphElement.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>
#include <kdebug.h>

TokenElement::TokenElement( BasicElement* parent ) : BasicElement( parent )
{
    m_stretchHorizontally = false;
    m_stretchVertically = false;
}

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
    QColor color = am->colorOf( "mathcolor", this );
    if (!color.isValid())
        color = am->colorOf( "color", this );

    painter.translate( m_xoffset, baseLine() );
    if(m_stretchHorizontally || m_stretchVertically)
        painter.scale(width() / m_originalSize.width(), height() / m_originalSize.height());

    painter.setPen( color );
    painter.setBrush( QBrush( color ) );
    painter.drawPath( m_contentPath );
}

void TokenElement::layout( const AttributeManager* am )
{
    // Query the font to use
    m_font = am->font( this );
    QFontMetricsF fm(m_font);

    // save the token in an empty path
    m_contentPath = QPainterPath();

    /* Current bounding box.  Note that the left can be negative, for italics etc */
    QRectF boundingrect;
    if(m_glyphs.isEmpty()) {//optimize for the common case
        boundingrect = renderToPath(m_rawString, m_contentPath);
     } else {
        // replace all the object replacement characters with glyphs
        // We have to keep track of the bounding box at all times
        QString chunk;
        int counter = 0;
        for( int i = 0; i < m_rawString.length(); i++ ) {
            if( m_rawString[ i ] != QChar::ObjectReplacementCharacter )
                chunk.append( m_rawString[ i ] );
            else {
                m_contentPath.moveTo(boundingrect.right(), 0);
                QRectF newbox = renderToPath( chunk, m_contentPath );
                boundingrect.setRight( boundingrect.right() + newbox.right());
                boundingrect.setTop( qMax(boundingrect.top(), newbox.top()));
                boundingrect.setBottom( qMax(boundingrect.bottom(), newbox.bottom()));

                m_contentPath.moveTo(boundingrect.right(), 0);
                newbox = m_glyphs[ counter ]->renderToPath( QString(), m_contentPath );
                boundingrect.setRight( boundingrect.right() + newbox.right());
                boundingrect.setTop( qMax(boundingrect.top(), newbox.top()));
                boundingrect.setBottom( qMax(boundingrect.bottom(), newbox.bottom()));

                counter++;
                chunk.clear();
            }
            if( !chunk.isEmpty() ) {
                m_contentPath.moveTo(boundingrect.right(), 0);
                QRectF newbox = renderToPath( chunk, m_contentPath );
                boundingrect.setRight( boundingrect.right() + newbox.right());
                boundingrect.setTop( qMax(boundingrect.top(), newbox.top()));
                boundingrect.setBottom( qMax(boundingrect.bottom(), newbox.bottom()));
            }
        }
    } 
    //The left side may be negative, because of italised letters etc. we need to adjust for this when painting
    //The qMax is just incase.  The bounding box left should never be >0
    m_xoffset = qMax(-boundingrect.left(), (qreal)0.0);
    // As the text is added to (0,0) the baseline equals the top edge of the
    // elements bounding rect, while translating it down the text's baseline moves too
    setBaseLine( -boundingrect.y() ); // set baseline accordingly
    setWidth( boundingrect.right() + m_xoffset );
    setHeight( boundingrect.height() );
    m_originalSize = QSizeF(width(), height());
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

void TokenElement::removeChild( FormulaCursor* cursor, BasicElement* child )
{
    m_rawString.remove( cursor->position(), 1 );
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
            m_glyphs.append(tmpGlyph);
        }
        else if( node.isElement() )
            return false;
        /*
        else if (node.isEntityReference()) {
            Dictionary dict;
            m_rawString.append( dict.mapEntity( node.nodeName() ) );
        }
        */
        else {
            m_rawString.append( node.toText().data().trimmed() );
        }

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
            if (i + 1 < tmp.count()) {
                writer->addTextNode( tmp[ i ] );
            }
        }
        else {
            writer->addTextNode( tmp[ i ] );
            if (i + 1 < tmp.count()) {
                m_glyphs[ i ]->writeMathML( writer );
            }
        }
    }
}

