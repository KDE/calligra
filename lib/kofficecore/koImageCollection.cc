
#include "koImageCollection.h"

#include <qmap.h>

class KoImageCollection::KoImageCollectionPrivate
{
public:
};

KoImageCollection::KoImageCollection()
{
    d = new KoImageCollectionPrivate;
}

KoImageCollection::~KoImageCollection()
{
    delete d;
}

KoImage KoImageCollection::insertImage( const KoImage::Key &key, const QImage &image )
{
    // prevent double insertion
    KoImage koImg = KoImageCollection::image( key );

    if ( !koImg.isValid() )
    {
        koImg = KoImage( key, image );

        m_images.insert( key, koImg );
    }

    return koImg;
}


