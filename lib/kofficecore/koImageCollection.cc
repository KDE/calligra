
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

KoImage KoImageCollection::insertImage( const QString &key, const QImage &image, const KURL &url )
{
    // prevent double insertion
    KoImage koImg = KoImageCollection::image( key );

    if ( !koImg.isValid() )
    {
        koImg = KoImage( key, image, url );

        m_images.insert( key, koImg );
    }

    return koImg;
}


