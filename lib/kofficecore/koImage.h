#ifndef __koImage_h__
#define __koImage_h__

#include <qimage.h>
#include <qshared.h>
#include <qstring.h>
#include <qdatetime.h>

class KoImagePrivate;

class KoImage
{
public:
    struct Key
    {
        Key() {}

        Key( const QString &filename, const QDateTime &timestamp )
            : m_filename( filename ), m_timestamp( timestamp ) {}

        bool operator==( const Key &other ) const
            { return m_filename == other.m_filename && m_timestamp == other.m_timestamp; }

        bool operator<( const Key &other ) const
            { return m_filename < other.m_filename && m_timestamp < other.m_timestamp; }

        QString m_filename;
        QDateTime m_timestamp;
    };

    KoImage();

    KoImage( const Key &key, const QImage &image );

    KoImage( const KoImage &other );

    ~KoImage();

    KoImage &operator=( const KoImage &other );

    QImage image() const;

    QPixmap pixmap() const;

    Key key() const;

    bool isValid() const;

    KoImage scale( const QSize &size ) const;

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
    KoImage::Key m_key;
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

inline KoImage::Key KoImage::key() const
{
    return d->m_key;
}

inline bool KoImage::isValid() const
{
    return d->m_valid;
}

#endif
