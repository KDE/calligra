
#include "kpimage.h"

KoImage<KPImageKey> KPImageCollection::loadImage( const KPImageKey &key )
{
    KoImage<KPImageKey> res = findImage( key );

    if ( !res.isNull() )
        return res;

    QImage img( key.filename );
    if ( img.isNull() )
        return res;

    return insertImage( key, img );
}

KoImage<KPImageKey> KPImageCollection::loadImage( const KPImageKey &key, const QString &rawData )
{
    KoImage<KPImageKey> res = findImage( key );

    if ( !res.isNull() )
        return res;

    QCString s(rawData.latin1());
    int i = s.find( ( char )1, 0 );

    while ( i != -1 ) {
        s[ i ] = '\"';
        i = s.find( ( char )1, i + 1 );
    }

    QImage img;
    img.loadFromData( s, "XPM" );

    if ( img.isNull() )
        return res;

    return insertImage( key, img );
}

QTextStream &operator<<( QTextStream &out, KPImageKey &key )
{
    QDate date = key.lastModified.date();
    QTime time = key.lastModified.time();

    out << " filename=\"" << key.filename << "\" year=\""
        << date.year()
        << "\" month=\"" << date.month() << "\" day=\"" << date.day()
        << "\" hour=\"" << time.hour() << "\" minute=\"" << time.minute()
        << "\" second=\"" << time.second() << "\" msec=\"" << time.msec() << "\" ";

    return out;
}

QTextStream &operator<<( QTextStream &out, KPImage &img )
{
    QSize sz = img.size();
    KPImageKey k = img.key();
    out << k
        << " width=\"" << sz.width()
        << "\" height=\"" << sz.height() << "\" ";

    return out;
}
