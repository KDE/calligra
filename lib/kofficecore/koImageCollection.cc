/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>

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

#include "koImageCollection.h"
#include <koStore.h>
#include <koStoreDevice.h>
#include <kdebug.h>
#include <qfileinfo.h>

KoImage KoImageCollection::findImage( const KoImageKey &key ) const
{
    ConstIterator it = find( key );
    if ( it == end() )
        return KoImage();

    return *it;
}

KoImage KoImageCollection::insertImage( const KoImageKey &key, const QImage &image )
{
    KoImage res = findImage( key );

    if ( res.isNull() )
    {
        res = KoImage( key, image );

        res = insert( key, res ).data(); // why not just insert(key,res) ?
    }

    return res;
}

KoImage KoImageCollection::loadImage( const QString &fileName )
{
    // Check the last modified date of the file, as it is now.
    QFileInfo inf( fileName );
    KoImageKey key( fileName, inf.lastModified() );

    KoImage i = findImage( key );
    if ( i.isNull() )
    {
        //kdDebug(30003) << " building image " << endl;
        QImage img( fileName );
        if ( !img.isNull() )
            i = insertImage( key, img );
        else
            kdWarning(30003) << "Couldn't build QImage from " << fileName << endl;
    }
    return i;
}

void KoImageCollection::saveToStore( KoStore *store, QValueList<KoImageKey> keys, const QString & prefix ) const
{
    int i = 0;
    QValueList<KoImageKey>::Iterator it = keys.begin();
    for ( ; it != keys.end(); ++it )
    {
        KoImage image = findImage( *it );
        if ( image.isNull() )
            kdWarning(30003) << "Image " << (*it).toString() << " not found in collection !" << endl;
        else
        {
            QString format = (*it).format();
            QString storeURL = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
            storeURL.prepend( prefix );

            if ( store->open( storeURL ) ) {
                KoStoreDevice dev( store );
                QImageIO io;
                io.setIODevice( &dev );
                io.setImage( image.image() );
                io.setFormat( format.latin1() );
                io.write();
                store->close();
            }
        }
    }
}

QDomElement KoImageCollection::saveXML( QDomDocument &doc, QValueList<KoImageKey> keys, const QString & prefix ) const
{
    QDomElement pixmaps = doc.createElement( "PIXMAPS" );
    int i = 0;
    QValueList<KoImageKey>::Iterator it = keys.begin();
    for ( ; it != keys.end(); ++it )
    {
        KoImage image = findImage( *it );
        if ( image.isNull() )
            kdWarning(30003) << "Image " << (*it).toString() << " not found in collection !" << endl;
        else
        {
            QString format = (*it).format();
            QString pictureName = QString( "pictures/picture%1.%2" ).arg( ++i ).arg( format.lower() );
            pictureName.prepend( prefix );

            QDomElement keyElem = doc.createElement( "KEY" );
            pixmaps.appendChild( keyElem );
            (*it).saveAttributes( keyElem );
            keyElem.setAttribute( "name", pictureName );
        }
    }
    return pixmaps;
}

KoImageCollection::StoreMap
KoImageCollection::readXML( QDomElement &pixmapsElem, const QDateTime & defaultDateTime )
{
    StoreMap map;
    QDomElement keyElement = pixmapsElem.firstChild().toElement();
    while( !keyElement.isNull() )
    {
        if (keyElement.tagName()=="KEY")
        {
            KoImageKey key;
            key.loadAttributes( keyElement, defaultDateTime.date(), defaultDateTime.time() );
            QString n = keyElement.attribute( "name" );
            map.insert( key, n );
        }
        keyElement = keyElement.nextSibling().toElement();
    }
    return map;
}

void KoImageCollection::readFromStore( KoStore * store, const StoreMap & storeMap, const QString &prefix )
{
    StoreMap::ConstIterator it = storeMap.begin();
    for ( ; it != storeMap.end(); ++it )
    {
        QString u;
        if ( ! it.data().isEmpty() )
            u = it.data();
        else {
            // old naming I suppose ?
            u = prefix + it.key().toString();
        }

        QImage img;

        if ( store->open( u ) ) {
            KoStoreDevice dev( store );
            QImageIO io( &dev, 0 );
            if( ! io.read() )
                // okay - has to be a funky - old xpm in a very old kpr file...
                // Don't ask me why this is needed...
                img = QImage( store->read( store->size() ) );
            else
                img = io.image();

            store->close();
        } else {
            u.prepend( "file:" );
            if ( store->open( u ) ) {
                KoStoreDevice dev( store );
                QImageIO io( &dev, 0 );
                io.read( );
                img = io.image();

                store->close();
            }
        }

        if ( !img.isNull() )
            insertImage( it.key(), img );
    }
}
