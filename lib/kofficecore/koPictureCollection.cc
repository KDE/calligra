/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (c) 2001, 2004 David Faure <faure@kde.org>
   Copyright (C) 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <qdom.h>

#include <kdebug.h>
#include <kurl.h>

#include <koStoreDevice.h>

#include "koPicture.h"
#include "koPictureCollection.h"

//#define DEBUG_PICTURES

KoPicture KoPictureCollection::findPicture(const KoPictureKey& key) const
{
#ifdef DEBUG_PICTURES
    kdDebug(30003) << "KoPictureCollection::findPicture " << key.toString() << endl;
#endif
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
#ifdef DEBUG_PICTURES
    kdDebug(30003) << "KoPictureCollection::insertPicture " << key.toString() << endl;
#endif
    KoPicture c = findPicture(key);
    if (c.isNull())
    {
#ifdef DEBUG_PICTURES
        kdDebug(30003) << "KoPictureCollection::insertPicture not found -> inserting" << endl;
#endif
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

KoPicture KoPictureCollection::downloadPicture(const KURL& url, QWidget *window)
{
#ifdef DEBUG_PICTURES
    kdDebug(30003) << "KoPictureCollection::downloadPicture " << url.prettyURL() << endl;
#endif

    // If it is a local file, we can check the last modification date, so we should better use loadPicture
    if (url.isLocalFile())
        return loadPicture(url.path());


    // We have really a remote file, so we cannot check the last modification date
    // Therefore we have to always download the file

    KoPicture pic;
#ifdef DEBUG_PICTURES
    kdDebug(30003) << "Trying to download picture from file " << url.prettyURL() << endl;
#endif

    if (pic.setKeyAndDownloadPicture(url, window))
        insertPicture(pic.getKey(), pic);
    else
        kdWarning(30003) << "Could not download KoPicture from " << url.prettyURL() << endl;

    return pic;
}

KoPicture KoPictureCollection::loadPicture(const QString& fileName)
{
#ifdef DEBUG_PICTURES
    kdDebug(30003) << "KoPictureCollection::loadPicture " << fileName << endl;
#endif
    // Check the last modified date of the file, as it is now.
    KoPictureKey key;
    key.setKeyFromFile(fileName);

    KoPicture c = findPicture(key);
    if (c.isNull())
    {
#ifdef DEBUG_PICTURES
        kdDebug(30003) << "Trying to load picture from file " << fileName << endl;
#endif
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
    // ### TODO: remove "cliparts" when KPresenter is ready for it
    if (pictureType==CollectionClipart)
        storeURL="cliparts/clipart";
    else
        storeURL="pictures/picture";
    storeURL+=QString::number(++counter);
    storeURL+='.';
    storeURL+=picture.getExtension();
    return storeURL;
}

QString KoPictureCollection::getFileNameAsKOffice1Dot1(const Type pictureType, KoPicture& picture, int& counter)
{
    QString storeURL;
    if (pictureType==CollectionClipart)
        storeURL="cliparts/clipart";
    else
        storeURL="pictures/picture";
    storeURL+=QString::number(++counter);
    storeURL+='.';
    storeURL+=picture.getExtensionAsKOffice1Dot1();
    return storeURL;
}


bool KoPictureCollection::saveToStoreInternal(const Type pictureType, KoStore *store, QValueList<KoPictureKey>& keys, const bool koffice11)
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
            QString storeURL;
            if (koffice11)
                storeURL=getFileNameAsKOffice1Dot1(pictureType, c, counter);
            else
                storeURL=getFileName(pictureType, c, counter);

            if (store->open(storeURL))
            {
                KoStoreDevice dev(store);
                if (koffice11)
                {
                    if ( !c.saveAsKOffice1Dot1(&dev) )
                        return false;
                }
                else
                {
                    if ( ! c.save(&dev) )
                        return false; // e.g. bad image?
                }
                if ( !store->close() )
                    return false; // e.g. disk full
            }
        }
    }
    return true;
}

bool KoPictureCollection::saveToStore(const Type pictureType, KoStore *store, QValueList<KoPictureKey> keys)
{
    return saveToStoreInternal(pictureType,store, keys, false);
}

bool KoPictureCollection::saveToStoreAsKOffice1Dot1(const Type pictureType, KoStore *store, QValueList<KoPictureKey> keys)
{
    return saveToStoreInternal(pictureType,store, keys, true);
}

QDomElement KoPictureCollection::saveXML(const Type pictureType, QDomDocument &doc, QValueList<KoPictureKey> keys)
{
    QString strElementName("PICTURES");
    if (pictureType==CollectionImage)
        strElementName="PIXMAPS";
    else if (pictureType==CollectionClipart)
        strElementName="CLIPARTS";
    QDomElement cliparts = doc.createElement( strElementName );
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

void KoPictureCollection::saveXMLAsKOffice1Dot1(QDomDocument &doc, QDomElement& parent, QValueList<KoPictureKey> keys)
{
    QDomElement pixmaps = doc.createElement( "PIXMAPS" );
    QDomElement cliparts = doc.createElement( "CLIPARTS" );
    parent.appendChild(pixmaps);
    parent.appendChild(cliparts);
    int counter=0;
    QValueList<KoPictureKey>::Iterator it = keys.begin();
    for ( ; it != keys.end(); ++it )
    {
        KoPicture picture = findPicture( *it );
        if ( picture.isNull() )
            kdWarning(30003) << "Picture " << (*it).toString() << " not found in collection !" << endl;
        else
        {
            QString pictureName("error");
            QDomElement keyElem = doc.createElement( "KEY" );

            if (picture.isClipartAsKOffice1Dot1())
            {
                pictureName=getFileNameAsKOffice1Dot1(CollectionClipart, picture, counter);
                cliparts.appendChild(keyElem);
            }
            else
            {
                pictureName=getFileNameAsKOffice1Dot1(CollectionImage, picture, counter);
                pixmaps.appendChild(keyElem);
            }

            (*it).saveAttributes(keyElem);
            keyElem.setAttribute("name", pictureName);
        }
    }
    return;
}

void KoPictureCollection::readXML( QDomElement& pixmapsElem, QMap <KoPictureKey, QString>& map )
{
    for( QDomNode n = pixmapsElem.firstChild();
         !n.isNull();
         n = n.nextSibling() )
    {
        QDomElement keyElement = n.toElement();
        if (keyElement.isNull()) continue;
        if (keyElement.tagName()=="KEY")
        {
            KoPictureKey key;
            key.loadAttributes(keyElement);
            map.insert(key, keyElement.attribute("name"));
        }
    }
}


KoPictureCollection::StoreMap KoPictureCollection::readXML( QDomElement& pixmapsElem )
{
    StoreMap map;
    readXML(pixmapsElem, map);
    return map;
}

void KoPictureCollection::readFromStore( KoStore * store, const StoreMap & storeMap )
{
#ifdef DEBUG_PICTURES
    kdDebug(30003) << "KoPictureCollection::readFromStore " << store << endl;
#endif
    StoreMap::ConstIterator it = storeMap.begin();
    for ( ; it != storeMap.end(); ++it )
    {
        KoPicture c = findPicture(it.key());
        if (!c.isNull())
        {
            // Do not load a file that we already have!
            //kdDebug(30003) << store << endl;
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
