#ifndef __koImageCollection_h__
#define __koImageCollection_h__

#include <qmap.h>

#include <koImage.h>

/**
 * This is a little extension to a QMap holding a bunch of @ref KoImage
 * objects. It actually inherits from QMap, but it provides to additional
 * methods. See @ref findImage and @ref insertImage
 */
template <class Key>
class KoImageCollection : public QMap<Key, KoImage<Key> >
{
public:
    typedef KoImage<Key> Image;

    /**
     * Convenience method to QMap::find . Returns the data directly.
     */
    Image findImage( const Key &key ) const;

    /**
     * Similar to QMap::insert, however it doesn't overwrite the
     * existing entry if it exists already.
     */
    Image insertImage( const Key &key, const QImage &image );
};

template <class Key>
KoImageCollection<Key>::Image KoImageCollection<Key>::findImage( const Key &key ) const
{
    // this gives a 'parse error before '='' . !@#$$! compiler
//    ConstIterator it = find( key );
    QMapConstIterator<Key, KoImage<Key> > it = find( key );
    if ( it == end() )
        return Image();

    return *it;
}

template <class Key>
KoImageCollection<Key>::Image KoImageCollection<Key>::insertImage( const Key &key, const QImage &image )
{
    Image res = findImage( key );

    if ( res.isNull() )
    {
        res = Image( key, image );

        res = insert( key, res ).data();
    }

    return res;
}

#endif
