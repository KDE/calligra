#ifndef __koImageCollection_h__
#define __koImageCollection_h__

#include <qmap.h>

#include <koImage.h>

class KoImageCollection
{
public:
    KoImageCollection();
    ~KoImageCollection();

    KoImage insertImage( const KoImage::Key &key, const QImage &image );

    KoImage image( const KoImage::Key &key );

    // ### removeme
    QMap<KoImage::Key,KoImage> data() const { return m_images; }

private:
    QMap<KoImage::Key,KoImage> m_images;
    class KoImageCollectionPrivate;
    KoImageCollectionPrivate *d;
};

inline KoImage KoImageCollection::image( const KoImage::Key &key )
{
    QMap<KoImage::Key,KoImage>::ConstIterator it = m_images.find( key );
    if ( it == m_images.end() )
        return KoImage();

    return *it;
}

#endif
