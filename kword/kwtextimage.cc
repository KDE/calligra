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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwtextimage.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include "defs.h"
#include <kdebug.h>

KWTextImage::KWTextImage( KWTextDocument *textdoc, const QString & filename )
    : KoTextCustomItem( textdoc ), place( PlaceInline )
{
    KWDocument * doc = textdoc->textFrameSet()->kWordDocument();
    if ( !filename.isEmpty() )
    {
        m_image = doc->imageCollection()->loadPicture( filename );
        Q_ASSERT( !m_image.isNull() );
        resize(); // Zoom if necessary
    }
}

void KWTextImage::setImage( const KoPicture &image )
{
    kdDebug() << "KWTextImage::setImage" << endl;
    Q_ASSERT( !image.isNull() );
    m_image = image;
    kdDebug() << "size: " << m_image.getOriginalSize().width() << "x" << m_image.getOriginalSize().height() << endl;
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

void KWTextImage::drawCustomItem( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected , const int /*offset*/)
{
    // (x,y) is the position of the inline item (in pixels)
    // (cx,cy,cw,ch) is the rectangle to be painted, in pixels too
    if ( m_image.isNull() ) {
        kdDebug() << "KWTextImage::draw null image!" << endl;
        p->fillRect( x, y, 50, 50, cg.dark() );
        return;
    }

    KoZoomHandler *zh = textDocument()->paintingZoomHandler();
    QSize imgSize( zh->layoutUnitToPixelX( width ), zh->layoutUnitToPixelY( height ) );

    QRect rect( QPoint(x, y), imgSize );
    if ( !rect.intersects( QRect( cx, cy, cw, ch ) ) )
        return;

    QPixmap pixmap=m_image.generatePixmap( imgSize );
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
    QDomElement imageElem = parentElem.ownerDocument().createElement( "IMAGE" );
    parentElem.appendChild( imageElem );
    QDomElement elem = parentElem.ownerDocument().createElement( "FILENAME" );
    imageElem.appendChild( elem );
    elem.setAttribute( "value", image().getKey().filename() );
}

void KWTextImage::load( QDomElement & parentElem )
{
    // <IMAGE>
    QDomElement image = parentElem.namedItem( "IMAGE" ).toElement();
    if ( image.isNull() )
	image = parentElem;
    // The FILENAME tag can be under IMAGE, or directly under parentElement in old koffice-1.0 docs.
    // <FILENAME>
    QDomElement filenameElement = image.namedItem( "FILENAME" ).toElement();
    if ( !filenameElement.isNull() )
    {
        QString filename = filenameElement.attribute( "value" );
        KWDocument * doc = static_cast<KWTextDocument *>(parent)->textFrameSet()->kWordDocument();
	// Important: we use a null QDateTime so that there's no date/time in the key.
        doc->addImageRequest( KoPictureKey( filename, QDateTime() ), this );
    }
    else
        kdError(32001) << "Missing FILENAME tag in IMAGE" << endl;
}
