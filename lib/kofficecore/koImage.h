#ifndef __koImage_h__
#define __koImage_h__

#include <qimage.h>
#include <qshared.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qmap.h>

#include <assert.h>

template <class T> class KoImagePrivate;

/**
 * KoImage is a container class for holding a QImage, a cached QPixmap version
 * of it, right together associated with a Key (for storage in a
 * @ref KoImageCollection). While KoImage itself is implicitly shared, note
 * that the contained QImage is explicitly shared, as documented in the
 * Qt documentation.
 */
template <class Key>
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
     * Returns true if the image is null. A null image is created using the
     * default constructor.
     */
    bool isNull() const;

    /**
     * Convenience method, returns the size of the image.
     */
    QSize size() const;

    /**
     * Scales the image's width and height to the specified size and returns
     * a new KoImage object for it.
     * Note that KoImage is intelligent enough to always scale from the
     * very original image, to provide best scaling quality.
     */
    KoImage scale( const QSize &size ) const;

private:
    KoImagePrivate<Key> *d;
};

/**
 * @internal
 */
template <class Key>
class KoImagePrivate : public QShared
{
public:
    QImage m_image;
    KoImage<Key> m_originalImage;
    Key m_key;
    mutable QPixmap m_cachedPixmap;
};

template <class Key>
KoImage<Key>::KoImage()
{
    d = 0;
}

template <class Key>
KoImage<Key>::KoImage( const Key &key, const QImage &image )
{
    d = new KoImagePrivate<Key>;
    d->m_image = image.copy();
    d->m_key = key;
}

template <class Key>
KoImage<Key>::KoImage( const KoImage &other )
{
    d = 0;
    (*this) = other;
}

template <class Key>
KoImage<Key>::~KoImage()
{
    if ( d && d->deref() )
        delete d;
}

template <class Key>
KoImage<Key> &KoImage<Key>::operator=( const KoImage<Key> &_other )
{
    KoImage<Key> &other = const_cast<KoImage<Key> &>( _other );

    if ( other.d )
        other.d->ref();

    if ( d && d->deref() )
        delete d;

    d = other.d;

    return *this;
}

template <class Key>
QImage KoImage<Key>::image() const
{
    if ( !d) return QImage();
    return d->m_image;
}

template <class Key>
QPixmap KoImage<Key>::pixmap() const
{
    if ( !d ) return QPixmap();

    if ( d->m_cachedPixmap.isNull() )
        d->m_cachedPixmap = d->m_image; // automatic conversion using assignment operator
    return d->m_cachedPixmap;
}

template <class Key>
Key KoImage<Key>::key() const
{
    if ( !d ) return Key();

    return d->m_key;
}

template <class Key>
bool KoImage<Key>::isNull() const
{
    return d == 0 || d->m_image.isNull();
}

template <class Key>
QSize KoImage<Key>::size() const
{
    if ( !d ) return QSize();

    return d->m_image.size();
}

template <class Key>
KoImage<Key> KoImage<Key>::scale( const QSize &size ) const
{
    if ( !d )
        return *this;

    KoImage<Key> originalImage;

    if ( !d->m_originalImage.isNull() )
        originalImage = d->m_originalImage;
    else
        originalImage = *this;

    if ( originalImage.size() == size )
        return originalImage;

    QImage scaledImg = originalImage.image().smoothScale( size.width(), size.height() );

    KoImage<Key> result( d->m_key, scaledImg );
    assert( result.d );
    result.d->m_originalImage = originalImage;

    return result;
}

#endif
