/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <goutte@kde.org>

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
#ifndef __koPictureKey_h__
#define __koPictureKey_h__

#include <qstring.h>
#include <qdatetime.h>

class QDomElement;

namespace KoPictureType
{
    // 3 = Qt 2.1.x and later
    // 4 = Qt 3.0
    // 5 = Qt 3.1 and later
    // -1 = current QT
    const int formatVersionQPicture=-1;

    enum Type
    {
        TypeUnknown = 0,
        TypeImage,          /// Image, QImage-based
        TypeEps,            /// Encapsulated Postscript
        TypeClipart,        /// Clipart, QPicture-based
        TypeWmf             /// WMF (Windows Meta File)
    };
}

// TODO: correct documentation

/**
 * KoPictureKey is the structure describing an image in a unique way.
 * It currently includes the original path to the image and the modification
 * date.
 *
 * @short Structure describing an image on disk
 */
class KoPictureKey
{
public:
    /**
     * Default constructor. Creates a null key
     */
    KoPictureKey();

    /**
     * Constructs a key, from a filename and a modification date
     * Storing the modification date as part of the key allows the user
     * to update the file and import it into the application again, without
     * the application reusing the old copy from the collection.
     */
    KoPictureKey( const QString &fn, const QDateTime &mod );

    /**
     * Constructs a key, from a filename (without modification date)
     */
    KoPictureKey( const QString &fn );

    /**
     * Copy constructor
     */
    KoPictureKey( const KoPictureKey &key );

    /**
     * Assignment operator
     */
    KoPictureKey &operator=( const KoPictureKey &key );

    /**
     * Comparison operator
     */
    bool operator==( const KoPictureKey &key ) const;

    /**
     * Comparison operator - used for sorting in the collection's map
     */
    bool operator<( const KoPictureKey &key ) const;

    /**
     * Convert this key into a string representation of it
     */
    QString toString() const;

    /**
     * Save this key in XML.
     */
    void saveAttributes( QDomElement &elem ) const;

    /**
     * Load this key from XML.
     */
    void loadAttributes( const QDomElement &elem );

    /**
     * First part of the key: the filename
     */
    QString filename() const { return m_filename; }

    /**
     * Second part of the key: the modification date
     */
    QDateTime lastModified() const { return m_lastModified; }

    /**
     * Sets the key according to @p filename modification time
     */
    void setKeyFromFile (const QString& filename);

protected:
    QString m_filename;
    QDateTime m_lastModified;
};

#endif /* __koPictureKey_h__ */
