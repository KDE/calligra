#ifndef __koImageCollection_h__
#define __koImageCollection_h__

#include <qmap.h>

#include <koImage.h>

class KoImageCollection
{
public:
    KoImageCollection();
    ~KoImageCollection();

    KoImage insertImage( const QString &key, const QImage &image, const KURL &url = KURL() );

    KoImage image( const QString &key );

    // ### removeme
    QMap<QString,KoImage> data() const { return m_images; }

private:
    QMap<QString,KoImage> m_images;
    class KoImageCollectionPrivate;
    KoImageCollectionPrivate *d;
};

inline KoImage KoImageCollection::image( const QString &key )
{
    QMap<QString,KoImage>::ConstIterator it = m_images.find( key );
    if ( it == m_images.end() )
        return KoImage();

    return *it;
}

#endif
