/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
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
#ifndef __koPictureKey_h__
#define __koPictureKey_h__

#include <qstring.h>
#include <qdatetime.h>

class QDomElement;

namespace KoPictureType
{
    const int formatVersionQPicture=3; // 3 = Qt 2.1.x and later

    enum Type
    {
        TypeUnknown = 0,
        TypeImage,          // Image, QImage-based
        TypeEps,            // EPS
        TypeJpeg,           // JPEG
        TypeClipart,        // Clipart, QPicture-based
        TypeSvg             // SVG
    };
};

// TODO: correct documentation

/**
 * KoPictureKey is the structure describing an image in a unique way.
 * It currently includes the original path to the image and the modification
 * date.
 */
class KoPictureKey
{
public:
    /**
     * Default constructor. Creates a null key
     */
    KoPictureKey()  { m_lastModified.setTime_t(0); }

    /**
     * Constructs a key, from a filename and a modification date
     * Storing the modification date as part of the key allows the user
     * to update the file and import it into the application again, without
     * the application reusing the old copy from the collection.
     */
    KoPictureKey( const QString &fn, const QDateTime &mod )
        : m_filename( fn ), m_lastModified( mod )
        {}
    /**
     * Copy constructor
     */
    KoPictureKey( const KoPictureKey &key )
        : m_filename( key.m_filename ), m_lastModified( key.m_lastModified )
        {}

    /**
     * Assignment operator
     */
    KoPictureKey &operator=( const KoPictureKey &key ) {
        m_filename = key.m_filename;
        m_lastModified = key.m_lastModified;
        return *this;
    }

    /**
     * Comparison operator
     */
    bool operator==( const KoPictureKey &key ) const {
        return ( key.m_filename == m_filename &&
                 key.m_lastModified == m_lastModified );
    }

    /**
     * Comparison operator - used for sorting in the collection's map
     */
    bool operator<( const KoPictureKey &key ) const {
        return key.toString() < toString();
    }

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
     * The default date and default time are used if the XML doesn't specify any date & time (compat)
     */
    void loadAttributes( const QDomElement &elem, const QDate &dDate, const QTime &dTime );

    /**
     * First part of the key: the filename
     */
    QString filename() const { return m_filename; }

    /**
     * Second part of the key: the modification date
     */
    QDateTime lastModified() const { return m_lastModified; }

protected:
    QString m_filename;
    QDateTime m_lastModified;
};

#endif /* __koPictureKey_h__ */
