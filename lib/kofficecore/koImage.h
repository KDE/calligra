#ifndef __koImage_h__
#define __koImage_h__

#include <qimage.h>
#include <qshared.h>

#include <kurl.h>

class KoImagePrivate;

class KoImage
{
public:
    KoImage();

    KoImage( const QString &key, const QImage &image, const KURL &url = KURL() );

    KoImage( const KoImage &other );

    ~KoImage();

    KoImage &operator=( const KoImage &other );

    QImage image() const;

    QPixmap pixmap() const;

    KURL url() const;

    QString key() const;

    bool isValid() const;

    KoImage scaleImage( const QSize &size ) const;

private:
    KoImagePrivate *d;
};

class KoImagePrivate : public QShared
{
public:
    KoImagePrivate()
        {
            m_valid = true;
        }

    bool m_valid;
    QImage m_image;
    KoImage m_originalImage;
    KURL m_url;
    QString m_key;
    mutable QPixmap m_cachedPixmap;
};

inline KoImage::KoImage( const KoImage &other )
{
    (*this) = other;
}

inline QImage KoImage::image() const
{
    return d->m_image;
}

inline QPixmap KoImage::pixmap() const
{
    if ( d->m_cachedPixmap.isNull() )
        d->m_cachedPixmap = d->m_image; // automatic conversion using assignment operator
    return d->m_cachedPixmap;
}

inline KURL KoImage::url() const
{
    return d->m_url;
}

inline QString KoImage::key() const
{
    return d->m_key;
}

inline bool KoImage::isValid() const
{
    return d->m_valid;
}

#endif
