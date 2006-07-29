/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#include "KWPictureFrameSet.h"
#include "KWDocument.h"
#include "KWordPictureFrameSetIface.h"
#include <KoStoreDevice.h>
#include <KoOasisContext.h>
#include <KoPictureCollection.h>
#include <KoDom.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <klocale.h>
#include <kdebug.h>

//#define DEBUG_DRAW

KWPictureFrameSet::KWPictureFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc ), m_keepAspectRatio( true ), m_finalSize( false )
{
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Picture %1" ) );
    else
        m_name = name;
}

KWPictureFrameSet::KWPictureFrameSet( KWDocument* doc, const QDomElement& frame, const QDomElement& imageTag, KoOasisContext& context )
    : KWFrameSet( doc ), m_keepAspectRatio( true ), m_finalSize( false )
{
    m_name = frame.attributeNS( KoXmlNS::draw, "name", QString::null );
    if ( doc->frameSetByName( m_name ) ) // already exists!
        m_name = doc->generateFramesetName( m_name + " %1" );
    loadOasis( frame, imageTag, context );
}

KWPictureFrameSet::~KWPictureFrameSet() {
}

KWordFrameSetIface* KWPictureFrameSet::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KWordPictureFrameSetIface( this );

    return m_dcop;
}

void KWPictureFrameSet::loadPicture( const QString & fileName )
{
    KoPictureCollection *collection = m_doc->pictureCollection();

    m_picture = collection->loadPicture( fileName );
}

void KWPictureFrameSet::insertPicture( const KoPicture& picture )
{
    KoPictureCollection *collection = m_doc->pictureCollection();

    m_picture = collection->insertPicture( picture.getKey(), picture );
}

void KWPictureFrameSet::reloadPicture( const KoPictureKey& key )
{
    KoPictureCollection *collection = m_doc->pictureCollection();
    // If the picture is not already in the collection, then it gives a blank picture
    m_picture = collection->insertPicture( key, KoPicture() );
}

QDomElement KWPictureFrameSet::save( QDomElement & parentElem, bool saveFrames )
{
    if ( m_frames.isEmpty() ) // Deleted frameset -> don't save
        return QDomElement();
    QDomElement framesetElem = parentElem.ownerDocument().createElement( "FRAMESET" );
    parentElem.appendChild( framesetElem );

    KWFrameSet::saveCommon( framesetElem, saveFrames );

    QDomElement imageElem = parentElem.ownerDocument().createElement( "PICTURE" );
    framesetElem.appendChild( imageElem );
    imageElem.setAttribute( "keepAspectRatio", m_keepAspectRatio ? "true" : "false" );

    QDomElement elem = parentElem.ownerDocument().createElement( "KEY" );
    imageElem.appendChild( elem );
    m_picture.getKey().saveAttributes( elem );
    return framesetElem;
}

void KWPictureFrameSet::load( QDomElement &attributes, bool loadFrames )
{
    KWFrameSet::load( attributes, loadFrames );

    QString defaultRatio="true";
    // <PICTURE>
    QDomNode node=attributes.namedItem( "PICTURE" );
    if ( node.isNull() )
    {
        node=attributes.namedItem( "IMAGE" );
        if ( node.isNull() )
        {
            node=attributes.namedItem( "CLIPART" );
            defaultRatio="false";
        }
    }

    QDomElement image = node.toElement();
    if ( !image.isNull() ) {
        m_keepAspectRatio = image.attribute( "keepAspectRatio", defaultRatio ) == "true";
        // <KEY>
        QDomElement keyElement = image.namedItem( "KEY" ).toElement();
        if ( !keyElement.isNull() )
        {
            KoPictureKey key;
            key.loadAttributes( keyElement );
            m_picture.clear();
            m_picture.setKey( key );
            m_doc->addPictureRequest( this );
        }
        else
        {
            // <FILENAME> (old format, up to KWord-1.1-beta2)
            QDomElement filenameElement = image.namedItem( "FILENAME" ).toElement();
            if ( !filenameElement.isNull() )
            {
                QString filename = filenameElement.attribute( "value" );
                m_picture.clear();
                m_picture.setKey( KoPictureKey( filename ) );
                m_doc->addPictureRequest( this );
            }
            else
            {
                kdError(32001) << "Missing KEY tag in IMAGE" << endl;
            }
        }
    } else {
        kdError(32001) << "Missing PICTURE/IMAGE/CLIPART tag in FRAMESET" << endl;
    }
}

void KWPictureFrameSet::saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool /*saveFrames*/ ) const
{
    if( m_frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    KWFrame* frame = m_frames.getFirst();
    frame->startOasisFrame( writer, context.mainStyles(), name() ); // draw:frame
    writer.startElement( "draw:image" );
    writer.addAttribute( "xlink:type", "simple" );
    writer.addAttribute( "xlink:show", "embed" );
    writer.addAttribute( "xlink:actuate", "onLoad" );
    if ( context.savingMode() == KoSavingContext::Store )
        writer.addAttribute( "xlink:href", m_doc->pictureCollection()->getOasisFileName(m_picture) );
    else {
        writer.startElement( "office:binary-data" );
        m_picture.saveAsBase64( writer );
        writer.endElement();
    }
    writer.endElement();

    writer.endElement(); // draw:frame

}

void KWPictureFrameSet::loadOasis( const QDomElement& frame, const QDomElement& tag, KoOasisContext& context )
{
    kdDebug() << k_funcinfo << endl;
    KoPictureKey key;
    QDomNode binaryData = KoDom::namedItemNS( tag, KoXmlNS::office, "binary-data" );
    if ( !binaryData.isNull() )
    {
        QCString data = binaryData.toElement().text().latin1();
        m_picture.loadFromBase64( data );
        key = KoPictureKey("nofile", QDateTime::currentDateTime(Qt::UTC));
        m_picture.setKey(key);
    }
    else
    {
        const QString href( tag.attributeNS( KoXmlNS::xlink, "href", QString::null) );
        if ( !href.isEmpty() /*&& href[0] == '#'*/ )
        {
            QString strExtension;
            const int result=href.findRev(".");
            if (result>=0)
            {
                strExtension=href.mid(result+1); // As we are using KoPicture, the extension should be without the dot.
            }
            QString filename(href/*.mid(1)*/);
            key = KoPictureKey(filename, QDateTime::currentDateTime(Qt::UTC));
            m_picture.setKey(key);

            KoStore* store = context.store();
            Q_ASSERT( store );
            if ( store->open( filename ) )
            {
                KoStoreDevice dev(store);
                if ( !m_picture.load( &dev, strExtension ) )
                    kdWarning(32001) << "Cannot load picture: " << filename << " " << href << endl;
                store->close();
            }
        }
    }

    m_doc->pictureCollection()->insertPicture( key, m_picture );
    context.styleStack().save();
    context.fillStyleStack( frame, KoXmlNS::draw, "style-name", "graphic" ); // get the style for the graphics element
    loadOasisFrame( frame, context );
    context.styleStack().restore();
}

void KWPictureFrameSet::drawFrameContents( KWFrame *frame, QPainter *painter, const QRect &crect,
                                           const QColorGroup &, bool, bool, KWFrameSetEdit *, KWViewMode * )
{
#ifdef DEBUG_DRAW
    kdDebug(32001) << "KWPictureFrameSet::drawFrameContents crect=" << crect << " size=" << kWordDocument()->zoomItX( frame->innerWidth() ) << "x" << kWordDocument()->zoomItY( frame->innerHeight() ) << endl;
#endif
    m_picture.draw( *painter, 0, 0, kWordDocument()->zoomItX( frame->innerWidth() ), kWordDocument()->zoomItY( frame->innerHeight() ),
                  crect.x(), crect.y(), crect.width(), crect.height(), !m_finalSize);
}

FrameSetType KWPictureFrameSet::type() const
{
    return FT_PICTURE;
}

bool KWPictureFrameSet::keepAspectRatio() const
{
    return m_keepAspectRatio;
}

void KWPictureFrameSet::setKeepAspectRatio( bool b )
{
    m_keepAspectRatio = b;
}

#ifndef NDEBUG
void KWPictureFrameSet::printDebug( KWFrame *frame )
{
    KWFrameSet::printDebug( frame );
    if ( !isDeleted() )
    {
        kdDebug(32001) << "Image: key=" << m_picture.getKey().toString() << endl;
    }
}
#endif
