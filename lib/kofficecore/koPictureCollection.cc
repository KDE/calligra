/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2001 David Faure <faure@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <nicog@snafu.de>

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

#include <qfileinfo.h>
#include <qdom.h>

#include <kdebug.h>
#include <koStoreDevice.h>

#include "koPicture.h"
#include "koPictureCollection.h"

KoPicture KoPictureCollection::findPicture(const KoPictureKey& key) const
{
    kdDebug(30003) << "KoPictureCollection::findPicture " << key.toString() << endl;
    ConstIterator it = find( key );
    if ( it == end() )
    {
        KoPicture picture;
        picture.setKey(key);
        return picture;
    }

    return *it;
}

KoPicture KoPictureCollection::insertPicture(const KoPictureKey& key, const KoPicture& picture)
{
    kdDebug(30003) << "KoPictureCollection::insertPicture " << key.toString() << endl;
    KoPicture c = findPicture(key);
    if (c.isNull())
    {
        kdDebug(30003) << "KoPictureCollection::insertPicture not found -> inserting" << endl;
        c=picture;
        c.setKey(key); // Be sure that the key is correctly set in the KoPicture!
        insert(key, c);
    }
    return c;
}

KoPicture KoPictureCollection::insertPicture(const KoPicture& picture)
{
    return insertPicture(picture.getKey(), picture);
}

KoPicture KoPictureCollection::loadPicture(const QString& fileName)
{
    kdDebug(30003) << "KoPictureCollection::loadPicture " << fileName << endl;
    // Check the last modified date of the file, as it is now.
    QFileInfo inf(fileName);
    KoPictureKey key(fileName, inf.lastModified());

    KoPicture c = findPicture(key);
    if (c.isNull())
    {
        kdDebug(30003) << "Trying to load picture from file " << fileName << endl;
        if (c.loadFromFile(fileName))
            insertPicture(key, c);
        else
            kdWarning(30003) << "Could not load KoPicture from " << fileName << endl;
    }
    return c;
}

QString KoPictureCollection::getFileName(const Type pictureType, KoPicture& picture, int& counter)
{
    QString storeURL;
    if (pictureType==CollectionClipart)
        storeURL="cliparts/clipart";
    else
        storeURL="pictures/picture";
    storeURL+=QString::number(++counter);
    storeURL+='.';
    storeURL+=picture.getExtension();
    return storeURL;
}


void KoPictureCollection::saveToStore(const Type pictureType, KoStore *store, QValueList<KoPictureKey> keys)
{
    int counter=0;
    QValueList<KoPictureKey>::Iterator it = keys.begin();
    for ( ; it != keys.end(); ++it )
    {
        KoPicture c = findPicture( *it );
        if (c.isNull())
            kdWarning(30003) << "Picture " << (*it).toString() << " not found in collection !" << endl;
        else
        {
            QString storeURL=getFileName(pictureType, c, counter);

            if (store->open(storeURL))
            {
                KoStoreDevice dev(store);
                c.save(&dev);
                store->close();
            }
        }
    }
}

QDomElement KoPictureCollection::saveXML(const Type pictureType, QDomDocument &doc, QValueList<KoPictureKey> keys)
{
    QDomElement cliparts = doc.createElement(
        (pictureType==CollectionClipart)?"CLIPARTS":"PIXMAPS");
    int counter=0;
    QValueList<KoPictureKey>::Iterator it = keys.begin();
    for ( ; it != keys.end(); ++it )
    {
        KoPicture picture = findPicture( *it );
        if ( picture.isNull() )
            kdWarning(30003) << "Picture " << (*it).toString() << " not found in collection !" << endl;
        else
        {
            QString pictureName=getFileName(pictureType, picture, counter);

            QDomElement keyElem = doc.createElement( "KEY" );
            cliparts.appendChild(keyElem);
            (*it).saveAttributes(keyElem);
            keyElem.setAttribute("name", pictureName);
        }
    }
    return cliparts;
}

KoPictureCollection::StoreMap KoPictureCollection::readXML( QDomElement& pixmapsElem )
{
    StoreMap map;

    for(
        QDomElement keyElement = pixmapsElem.firstChild().toElement();
        !keyElement.isNull();
        keyElement = keyElement.nextSibling().toElement()
        )
    {
        if (keyElement.tagName()=="KEY")
        {
            KoPictureKey key;
            key.loadAttributes(keyElement);
            map.insert(key, keyElement.attribute("name"));
        }
    }
    return map;
}

void KoPictureCollection::readFromStore( KoStore * store, const StoreMap & storeMap )
{
    kdDebug(30003) << "KoPictureCollection::readFromStore " << store << endl;
    StoreMap::ConstIterator it = storeMap.begin();
    for ( ; it != storeMap.end(); ++it )
    {
        KoPicture c = findPicture(it.key());
        if (!c.isNull())
        {
            // Do not load a file that we already have!
            kdDebug(30003) << store << endl;
            continue;
        }
        QString u(it.data());
        if (u.isEmpty())
        {
            // old naming I suppose ?
            u=it.key().toString();
        }

        KoPicture picture;

        if (!store->open( u ))
        {
            u.prepend( "file:" );
            if (!store->open( u ))
            {
                kdWarning(30003) << "Could load neither from store nor from file: " << it.data() << endl;
                return;
            }
        }

        const int pos=u.findRev('.');
        if (pos==-1)
        {
            kdError(30003) << "File with no extension! Not supported!" << endl;
            return;
        }
        const QString extension(u.mid(pos+1));

        KoStoreDevice dev(store);
        picture.load(&dev, extension);
        store->close();

        if (!picture.isNull())
            insertPicture(it.key(), picture);
    }
}

KoPicture KoPictureCollection::findOrLoad(const QString& fileName, const QDateTime& dateTime)
{
    // As now all KoPictureKey objects have a valid QDateTime, we must do it without a date/time check.
    ConstIterator it = find( KoPictureKey ( fileName, dateTime ) );
    if ( it == end() )
    {
        return loadPicture( fileName );
    }
    return *it;
}
