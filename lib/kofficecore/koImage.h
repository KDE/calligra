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
#ifndef __koImage_h__
#define __koImage_h__

#include <qimage.h>
#include <qstring.h>
#include <qdatetime.h>

class KoImagePrivate;
class QDomElement;
class QDomDocument;

/**
 * KoImageKey is the structure describing an image in a unique way.
 * It currently includes the original path to the image and the modification
 * date.
 */
struct KoImageKey
{
    /**
     * Default constructor. Creates a null key
     */
    KoImageKey()
        : m_filename(), m_lastModified()
        {}

    /**
     * Constructs a key, from a filename and a modification date
     * Storing the modification date as part of the key allows the user
     * to update the file and import it into the application again, without
     * the application reusing the old copy from the collection.
     */
    KoImageKey( const QString &fn, const QDateTime &mod )
        : m_filename( fn ), m_lastModified( mod )
        {}
    /**
     * Copy constructor
     */
    KoImageKey( const KoImageKey &key )
        : m_filename( key.m_filename ), m_lastModified( key.m_lastModified )
        {}

    /**
     * Assignment operator
     */
    KoImageKey &operator=( const KoImageKey &key ) {
        m_filename = key.m_filename;
        m_lastModified = key.m_lastModified;
        return *this;
    }

    /**
     * Comparison operator
     */
    bool operator==( const KoImageKey &key ) const {
        return ( key.m_filename == m_filename &&
                 key.m_lastModified == m_lastModified );
    }

    /**
     * Comparison operator - used for sorting in the collection's map
     */
    bool operator<( const KoImageKey &key ) const {
        return key.toString() < toString();
    }

    /**
     * Convert this key into a string representation of it
     */
    QString toString() const {
        return QString::fromLatin1( "%1_%2" ).arg( m_filename ).arg( m_lastModified.toString() );
    }

    /**
     * Save this key in XML.
     */
    void saveAttributes( QDomElement &elem );
    /**
     * Load this key from XML.
     * The default date and default time are used if the XML doesn't specify any date & time (compat)
     */
    void loadAttributes( const QDomElement &elem, const QDate &dDate, const QTime &dTime );

    /**
     * Returns the format in which to save this image
     * (this is determined from the filename).
     */
    QString format() const;


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

/**
 * KoImage is a container class for holding a QImage, a cached QPixmap version
 * of it, right together associated with a key (for storage in a
 * @ref KoImageCollection). While KoImage itself is implicitly shared, note
 * that the contained QImage is explicitly shared, as documented in the
 * Qt documentation.
 */
class KoImage
{
public:
    /**
     * Default constructor. Creates a null image.
     */
    KoImage();

    /**
     * Constructs a KoImage object from the given key and
     * QImage. Note that KoImage will create a copy of the
     * provided QImage.
     */
    KoImage( const KoImageKey &key, const QImage &image );

    /**
     * Copy constructor.
     */
    KoImage( const KoImage &other );

    /**
     * Destructor.
     */
    ~KoImage();

    /**
     * Assignment operator.
     */
    KoImage &operator=( const KoImage &other );

    /**
     * Retrieve the stored QImage object.
     */
    QImage image() const;

    /**
     * Retrieve a pixmap representation of the stored image.
     * (Note that the pixmap is cached internally, so the (slow) conversion
     *  is not done for each call)
     */
    QPixmap pixmap() const;

    /**
     * Retrieve the key structure describing the image in a unique way.
     */
    KoImageKey key() const;

    /**
     * Returns true if the image is null. A null image is created using the
     * default constructor.
     */
    bool isNull() const;

    /**
     * Convenience method, returns the size of the image.
     */
    QSize size() const;

    /**
     * Convenience method, returns the original size of the image, before scaling.
     */
    QSize originalSize() const;

    /**
     * Scales the image's width and height to the specified size and returns
     * a new KoImage object for it.
     * Note that KoImage is intelligent enough to always scale from the
     * very original image, to provide best scaling quality.
     */
    KoImage scale( const QSize &size ) const;

private:
    KoImagePrivate *d;
};

#endif
