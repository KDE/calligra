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
#ifndef __koImageCollection_h__
#define __koImageCollection_h__

#include <qmap.h>
#include <qdom.h>

#include "koImage.h"
class KoStore;

/**
 * This is a little extension to a QMap holding a bunch of @ref KoImage
 * objects. It actually inherits from QMap, but it provides to additional
 * methods. See @ref findImage and @ref insertImage
 */
class KoImageCollection : public QMap<KoImageKey, KoImage>
{
public:
    /**
     * Convenience method to QMap::find . Returns the data directly.
     */
    KoImage findImage( const KoImageKey &key ) const;

    /**
     * Similar to QMap::insert, however it doesn't overwrite the
     * existing entry if it exists already.
     */
    KoImage insertImage( const KoImageKey &key, const QImage &image );

    /**
     * Load an image from a file (and insert into the collection).
     * The modification date of the file is checked, to create the key
     * for this KoImage. If this key maps to an existing image in the
     * collection, then this image is returned, otherwise the file is loaded.
     */
    KoImage loadImage( const QString &fileName );

    /**
     * Save the used images from the collection into the store
     * Usually called from completeSaving().
     *
     * @param store the store in which to save the images
     * @param keys the list of keys corresponding to the images to save
     * @param prefix the prefix to use when saving the images in the store
     * (usually: isStoredExtern() ? QString::null : url().url() + "/",
     * where 'this' is the document)
     */
    void saveToStore( KoStore * store, QValueList<KoImageKey> keys, const QString & prefix );

    /**
     * Generate the <PIXMAPS> tag, that saves the key and the related
     * relative path in the store (e.g. pictures/picture1.ext) for each pixmap.
     */
    QDomElement saveXML( QDomDocument &doc, QValueList<KoImageKey> keys, const QString & prefix );

    typedef QMap<KoImageKey, QString> StoreMap;
    /**
     * Read the <PIXMAPS> tag, and save the result (key<->store-filename associations)
     * into the QMap. You may want to 'new' a QMap in loadXML, and to use and then delete
     * it in completeLoading (to save memory).
     */
    StoreMap readXML( QDomElement &pixmapsElem, const QDateTime & defaultDateTime );

    /**
     * Read all images from the store, into this collection
     * The map comes from readXML above, and is used to find which images
     * to load, and which key to associate them.
     * @param prefix same as above (only used for old format support)
     */
    void readFromStore( KoStore * store, const StoreMap &storeMap,const QString &prefix );

};

#endif
