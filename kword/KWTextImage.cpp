/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>

#include <KoPictureCollection.h>

#include "KWDocument.h"
#include "defs.h"
#include "KWTextFrameSet.h"

#include "KWTextImage.h"

KWTextImage::KWTextImage( KWTextDocument *textdoc, const QString & filename )
    : KoTextCustomItem( textdoc ), place( PlaceInline )
{
    KWDocument * doc = textdoc->textFrameSet()->kWordDocument();
    if ( !filename.isEmpty() )
    {
        m_image = doc->pictureCollection()->loadPicture( filename );
        Q_ASSERT( !m_image.isNull() );
        resize(); // Zoom if necessary
    }
}

void KWTextImage::setImage( const KoPictureCollection & collection )
{
    kdDebug(32001) << "Loading text image " << m_image.getKey().toString() << " (in KWTextImage::setImage)" << endl;
    m_image=collection.findPicture( m_image.getKey() );
    Q_ASSERT( !m_image.isNull() );
    kdDebug(32001) << "size: " << m_image.getOriginalSize().width() << "x" << m_image.getOriginalSize().height() << endl;
    resize();
}

void KWTextImage::resize()
{
    if ( m_deleted )
        return;
    if ( !m_image.isNull() ) {
        //KWDocument * doc = static_cast<KWTextDocument *>(parent)->textFrameSet()->kWordDocument();
        width = m_image.getOriginalSize().width();
        // width is a 100%-zoom pixel size. We want a LU pixel size -> we simply need 'to LU', i.e. ptToLayoutPt
        width = KoTextZoomHandler::ptToLayoutUnitPt( width );
        height = m_image.getOriginalSize().height();
        height = KoTextZoomHandler::ptToLayoutUnitPt( height );
        kdDebug() << "KWTextImage::resize: " << width << ", " << height << endl;
        // no! m_image.setSize( QSize( width, height ) );
    }
}

void KWTextImage::drawCustomItem( QPainter* p, int x, int y, int wpix, int hpix, int /*ascentpix*/, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected, int /*offset*/,  bool drawingShadow)
{
    if ( drawingShadow )
        return;

    // (x,y) is the position of the inline item (in pixels)
    // (wpix,hpix) is the size of the inline item (in pixels)
    // (cx,cy,cw,ch) is the rectangle to be painted, in pixels too
    if ( m_image.isNull() ) {
        kdDebug() << "KWTextImage::draw null image!" << endl;
        p->fillRect( x, y, 50, 50, cg.dark() );
        return;
    }

    QSize imgSize( wpix, hpix );

    QRect rect( QPoint(x, y), imgSize );
    if ( !rect.intersects( QRect( cx, cy, cw, ch ) ) )
        return;

    QPixmap pixmap=m_image.generatePixmap( imgSize, true );
    //if ( placement() == PlaceInline )
        p->drawPixmap( x, y, pixmap );
    //else
    //    p->drawPixmap( cx, cy, pixmap, cx - x, cy - y, cw, ch );

    if ( selected && placement() == PlaceInline && p->device()->devType() != QInternal::Printer ) {
        p->fillRect( rect , QBrush( cg.highlight(), QBrush::Dense4Pattern) );
    }
}

void KWTextImage::save( QDomElement & parentElem )
{
    // This code is similar to KWPictureFrameSet::save
    KWDocument * doc = static_cast<KWTextDocument *>(parent)->textFrameSet()->kWordDocument();

    QDomElement imageElem = parentElem.ownerDocument().createElement( "PICTURE" );
    parentElem.appendChild( imageElem );
    //imageElem.setAttribute( "keepAspectRatio", "true" );
    QDomElement elem = parentElem.ownerDocument().createElement( "KEY" );
    imageElem.appendChild( elem );
    image().getKey().saveAttributes( elem );
    // Now we must take care that a <KEY> element will be written as a child of <PICTURES>
    doc->addTextImageRequest( this );
}

void KWTextImage::load( QDomElement & parentElem )
{
    // This code is similar to KWPictureFrameSet::load
    KWDocument * doc = static_cast<KWTextDocument *>(parent)->textFrameSet()->kWordDocument();

    // <IMAGE> (KOffice 1.0) or <PICTURE> (KWord 1.2)
    QDomNode node=parentElem.namedItem( "PICTURE" );
    if ( node.isNull() )
    {
        node=parentElem.namedItem( "IMAGE" );
    }
    QDomElement image = node.toElement();
    if ( image.isNull() )
        image = parentElem; // The data is directly child of <FORMAT>
    // <KEY>
    QDomElement keyElement = image.namedItem( "KEY" ).toElement();
    if ( !keyElement.isNull() )
    {
        KoPictureKey key;
        key.loadAttributes( keyElement );
        m_image.setKey(key);
        doc->addTextImageRequest( this );
    }
    else
    {
        // <FILENAME> (old format, up to KWord-1.1-beta2)
        QDomElement filenameElement = image.namedItem( "FILENAME" ).toElement();
        if ( !filenameElement.isNull() )
        {
            QString filename = filenameElement.attribute( "value" );
            m_image.setKey( KoPictureKey( filename ) );
            doc->addTextImageRequest( this );
        }
        else
        {
            kdError(32001) << "Missing KEY or FILENAME tag in IMAGE (KWTextImage::load)" << endl;
        }
    }
}

KoPictureKey KWTextImage::getKey( void ) const
{
    return m_image.getKey();
}

void KWTextImage::saveOasis( KoXmlWriter&, KoSavingContext& ) const
{
    // Not implemented
}
