#ifndef __koImage_h__
#define __koImage_h__

#include <qimage.h>
#include <qshared.h>
#include <qstring.h>
#include <qdatetime.h>

class KoImagePrivate;

/**
 * KoImage is a container class for holding a QImage, a cached QPixmap version
 * of it, right together associated with a Key (for storage in a
 * @ref KoImageCollection . While KoImage itself is implicitly shared, note
 * that the contained QImage is explicitly shared, as documented in the
 * Qt documentation.
 */
class KoImage
{
public:
    /**
     * A simple Key structure for holding a 'unique' filename and timestamp
     * for a image
     */
    struct Key
    {
        Key() {}

        Key( const QString &_filename, const QDateTime &_timestamp = QDateTime() )
            : fileName( _filename ), timestamp( _timestamp ) {}

        bool operator==( const Key &other ) const
            { return fileName == other.fileName && timestamp == other.timestamp; }

        bool operator<( const Key &other ) const
            {
                register bool res = fileName < other.fileName;
                if ( timestamp.isValid() && other.timestamp.isValid() )
                    res &= timestamp < other.timestamp;
                return res;
            }

        QString fileName;
        QDateTime timestamp;
    };

    /**
     * Default constructor. Creates a null image.
     */
    KoImage();

    /**
     * Constructs a KoImage object from the given key and
     * QImage. Note that KoImage will create a copy of the
     * provided QImage.
     */
    KoImage( const Key &key, const QImage &image );

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
     * Retrieve the Key structure describing the image in a unique way.
     */
    Key key() const;

    /**
     * Convenience method for retrieving the image's filename. Alias for
     * key().fileName
     */
    QString fileName() const; //convenience

    /**
     * Returns true if the image is null. A null image is created using the
     * default constructor.
     */
    bool isNull() const;

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

/**
 * @internal
 */
class KoImagePrivate : public QShared
{
public:
    QImage m_image;
    KoImage m_originalImage;
    KoImage::Key m_key;
    mutable QPixmap m_cachedPixmap;
};

inline KoImage::KoImage( const KoImage &other )
{
    d = 0;
    (*this) = other;
}

inline QImage KoImage::image() const
{
    if ( !d) return QImage();
    return d->m_image;
}

inline QPixmap KoImage::pixmap() const
{
    if ( !d ) return QPixmap();

    if ( d->m_cachedPixmap.isNull() )
        d->m_cachedPixmap = d->m_image; // automatic conversion using assignment operator
    return d->m_cachedPixmap;
}

inline KoImage::Key KoImage::key() const
{
    if ( !d ) return KoImage::Key();

    return d->m_key;
}

inline QString KoImage::fileName() const
{
    return key().fileName;
}

inline bool KoImage::isNull() const
{
    return d != 0;
}

#endif
