
#include "koImage.h"

#include <qshared.h>

#include <assert.h>

class KoImageCollection;


KoImage::KoImage()
{
    d = 0;
}

KoImage::KoImage( const Key &key, const QImage &image )
{
    d = new KoImagePrivate;
    d->m_image = image.copy();
    d->m_key = key;
}

KoImage::~KoImage()
{
    if ( d && d->deref() )
        delete d;
}

KoImage &KoImage::operator=( const KoImage &_other )
{
    KoImage &other = const_cast<KoImage &>( _other );

    if ( other.d )
        other.d->ref();

    if ( d && d->deref() )
        delete d;

    d = other.d;

    return *this;
}

KoImage KoImage::scale( const QSize &size ) const
{
    if ( !d )
        return *this;

    KoImage originalImage;

    if ( d->m_originalImage.isNull() )
        originalImage = d->m_originalImage;
    else
        originalImage = *this;

    if ( originalImage.image().size() == size )
        return originalImage;

    QImage scaledImg = originalImage.image().smoothScale( size.width(), size.height() );

    KoImage result( d->m_key, scaledImg );
    assert( result.d );
    result.d->m_originalImage = originalImage;

    return result;
}

