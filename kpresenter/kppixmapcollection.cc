/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: pixmap collection                                      */
/******************************************************************/

#include "kppixmapcollection.h"

#include <qstring.h>
#include <qwmatrix.h>

#include <stdio.h>

/******************************************************************/
/* Class: KPPixmapDataCollection                                  */
/******************************************************************/

/*================================================================*/
KPPixmapDataCollection::~KPPixmapDataCollection()
{
    data.clear();
    refs.clear();
}

/*================================================================*/
QImage *KPPixmapDataCollection::findPixmapData( const Key &key )
{
    //printf("  KPPixmapDataCollection::findPixmapData( key = %s )\n", key.toString().latin1() );
    //printf("    data.count = %d\n", data.count() );

    QMap< Key, QImage >::Iterator it = data.find ( key );

    if ( it != data.end() && it.key() == key )
    {
        //printf( "    fond pixmap data %s\n", it.key().toString().latin1() );
        return &it.data();
    }
    else
    {
        //printf( "    did NOT find pixmap data\n" );
        return 0L;
    }
}

/*================================================================*/
QImage *KPPixmapDataCollection::insertPixmapData( const Key &key, const QImage &img )
{
    //printf("  KPPixmapDataCollection::insertPixmapData( key = %s, img = %d )\n", key.toString().latin1(), !img.isNull() );

    QImage *image = new QImage( img );

    image->detach();
    data.insert( Key( key ), *image );

    int ref = 1;
    refs.insert( Key( key ), ref );

    return image;
}

/*================================================================*/
void KPPixmapDataCollection::setPixmapOldVersion( const Key &key, const char *_data )
{
    if ( data.contains( key ) )
        return;

    QCString s( _data );
    int i = s.find( ( char )1, 0 );

    while ( i != -1 )
    {
        s[ i ] = '\"';
        i = s.find( ( char )1, i + 1 );
    }

    QImage img;
    img.loadFromData( s, "XPM" );
    insertPixmapData( key, img );
}

/*================================================================*/
void KPPixmapDataCollection::setPixmapOldVersion( const Key &key )
{
    if ( data.contains( key ) )
        return;

    QImage img( key.filename );
    insertPixmapData( key, img );
}

/*================================================================*/
void KPPixmapDataCollection::addRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

    //printf( "  KPPixmapDataCollection::addRef( key = %s )\n", key.toString().latin1() );

    if ( refs.contains( key ) )
    {
        int ref = refs[ key ];
        refs[ key ] = ++ref;
        //printf( "    ref: %d\n", refs[ key ] );
    }
}

/*================================================================*/
void KPPixmapDataCollection::removeRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

    //printf( "  KPPixmapDataCollection::removeRef( key = %s )\n", key.toString().latin1() );

    if ( refs.contains( key ) )
    {
        int ref = refs[ key ];
        refs[ key ] = --ref;
        //printf( "    ref: %d\n", refs[ key ] );

//         if ( ref == 0 )
//         {
//             refs.remove( key );
//             data.remove( key );
//             printf( "    remove %s\n", key.toString().latin1() );
//         }
    }
}

/******************************************************************/
/* Class: KPPixmapCollection                                      */
/******************************************************************/

/*================================================================*/
KPPixmapCollection::~KPPixmapCollection()
{
    pixmaps.clear();
}

/*================================================================*/
QPixmap* KPPixmapCollection::findPixmap( Key &key )
{
    //printf( "  KPPixmapCollection::findPixmap( key = %s )\n", key.toString().latin1() );

    if ( key.size == orig_size )
    {
        QImage *i = dataCollection.findPixmapData( key.dataKey );
        if ( i )
            key.size = i->size();
        else
        {
            QImage img( key.dataKey.filename );
            key.size = img.size();
        }
    }

    //printf( "    key = %s )\n", key.toString().latin1() );

    QMap< Key, QPixmap >::Iterator it = pixmaps.begin();
    it = pixmaps.find( key );

    if ( it != pixmaps.end() && it.key() == key )
    {
        //printf( "    pixmap found in pixmaps: %s\n", it.key().toString().latin1() );
        addRef( key );
        return &it.data();
    }
    else
    {
        QImage *img = dataCollection.findPixmapData( key.dataKey );
        if ( img )
        {
            //printf( "    pixmap found in data collection: %s\n", key.dataKey.toString().latin1() );
            dataCollection.addRef( key.dataKey );
            return loadPixmap( *img, key, true );
        }
        else
        {
            QImage image( key.dataKey.filename );

            //printf( "    pixmap not found anywhere\n" );
            dataCollection.insertPixmapData( key.dataKey, image );
            return loadPixmap( image, key, true );
        }
    }
}

/*================================================================*/
void KPPixmapCollection::addRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

    //printf( "  KPPixmapCollection::addRef( key = %s )\n", key.toString().latin1() );

    if ( refs.contains( key ) )
    {
        int ref = refs[ key ];
        refs[ key ] = ++ref;
        //printf( "    ref: %d\n", refs[ key ] );
    }

    dataCollection.addRef( key.dataKey );
}

/*================================================================*/
void KPPixmapCollection::removeRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

    //printf( "  KPPixmapCollection::removeRef( key = %s )\n", key.toString().latin1() );

    if ( refs.contains( key ) )
    {
        int ref = refs[ key ];
        refs[ key ] = --ref;
        //printf( "    ref: %d\n", refs[ key ] );

        if ( ref == 0 )
        {
            refs.remove( key );
            pixmaps.remove( key );
            //printf( "    remove: %s\n", key.toString().latin1() );
        }
    }

    dataCollection.removeRef( key.dataKey );
}

/*================================================================*/
QPixmap *KPPixmapCollection::loadPixmap( const QImage &image, const Key &key, bool insert )
{
    //printf( "  KPPixmapCollection::loadPixmap( image = %d, key = %s, insert = %d )\n",
    //        !image.isNull(), key.toString().latin1(), insert );

    QPixmap *pixmap = new QPixmap;
    pixmap->convertFromImage( image );

    KSize size = key.size;
    if ( size != pixmap->size() && size != orig_size && pixmap->width() != 0 && pixmap->height() != 0 )
    {
        QWMatrix m;
        m.scale( static_cast<float>( size.width() ) / static_cast<float>( pixmap->width() ),
                 static_cast<float>( size.height() ) / static_cast<float>( pixmap->height() ) );
        *pixmap = pixmap->xForm( m );
    }

    if ( insert )
    {
        pixmaps.insert( Key( key ), *pixmap );
        int ref = 1;
        refs.insert( Key( key ), ref );
    }

    return pixmap;
}

/*================================================================*/
ostream& operator<<( ostream &out, KPPixmapDataCollection::Key &key )
{
    QDate date = key.lastModified.date();
    QTime time = key.lastModified.time();

    out << " filename=\"" << key.filename.latin1() << "\" year=\"" << date.year()
        << "\" month=\"" << date.month() << "\" day=\"" << date.day()
        << "\" hour=\"" << time.hour() << "\" minute=\"" << time.minute()
        << "\" second=\"" << time.second() << "\" msec=\"" << time.msec() << "\" ";

    return out;
}

/*================================================================*/
ostream& operator<<( ostream &out, KPPixmapCollection::Key &key )
{
    out << key.dataKey << " width=\"" << key.size.width() << "\" height=\""
        << key.size.height() << "\" ";

    return out;
}

// /*======================= get Pixmap =============================*/
// QPixmap* KPPixmapCollection::getPixmap( QString _filename, KSize _size, QString &_data, bool orig = false, bool addref = true )
// {
//     int num = inPixmapList( _filename, _size );

//     if ( num == -1 )
//     {
//         KPPixmap *kppixmap = new KPPixmap( _filename, _size );
//         pixmapList.append( kppixmap );
//         _data = kppixmap->getPixDataNative();

// #ifdef SHOW_INFO
//         debug( "-------------" );
//         debug( "add Pixmap '%s'", _filename.data() );
//         debug( "PixmapCollection count: %d", pixmapList.count() );
//         debug( "%d: ", pixmapList.count() - 1 );
// #endif

//         if ( addref )
//             kppixmap->addRef();

// #ifdef SHOW_INFO
//         debug( "-------------" );
// #endif

//         if ( orig )
//             return kppixmap->getOrigPixmap();
//         else
//             return kppixmap->getPixmap();
//     }
//     else
//     {

// #ifdef SHOW_INFO
//         debug( "-------------" );
//         debug( "%d: ", num );
// #endif

//         if ( addref )
//             pixmapList.at( num )->addRef();

// #ifdef SHOW_INFO
//         debug( "-------------" );
// #endif

//         _data = pixmapList.at( num )->getPixDataNative();
//         if ( orig )
//             return pixmapList.at( num )->getOrigPixmap();
//         else
//             return pixmapList.at( num )->getPixmap();
//     }
// }

// /*======================= get Pixmap =============================*/
// QPixmap* KPPixmapCollection::getPixmap( QString _filename, QString _data, KSize _size, bool orig = false, bool addref = true )
// {
//     int num = inPixmapList( _filename, _data, _size );

//     if ( num == -1 )
//     {
//         KPPixmap *kppixmap = new KPPixmap( _filename, _data, _size );
//         pixmapList.append( kppixmap );

// #ifdef SHOW_INFO
//         debug( "-------------" );
//         debug( "add Pixmap '%s'", _filename.data() );
//         debug( "PixmapCollection count: %d", pixmapList.count() );
//         debug( "%d: ", pixmapList.count() - 1 );
// #endif

//         if ( addref )
//             kppixmap->addRef();

// #ifdef SHOW_INFO
//         debug( "-------------" );
// #endif

//         if ( orig )
//             return kppixmap->getOrigPixmap();
//         else
//             return kppixmap->getPixmap();
//     }
//     else
//     {

// #ifdef SHOW_INFO
//         debug( "%d: ", num );
// #endif

//         if ( addref )
//             pixmapList.at( num )->addRef();

// #ifdef SHOW_INFO
//         debug( "-------------" );
// #endif

//         if ( orig )
//             return pixmapList.at( num )->getOrigPixmap();
//         else
//             return pixmapList.at( num )->getPixmap();
//     }
// }

// /*======================= get Pixmap =============================*/
// QPixmap* KPPixmapCollection::getPixmap( QString _filename, QString _data, QPixmap *_pixmap, KSize _size, bool orig = false, bool addref = true )
// {
//     int num = inPixmapList( _filename, _data, _size );

//     if ( num == -1 )
//     {
//         KPPixmap *kppixmap = new KPPixmap( _filename, _data, _pixmap, _size );
//         pixmapList.append( kppixmap );

// #ifdef SHOW_INFO
//         debug( "-------------" );
//         debug( "add Pixmap '%s'", _filename.data() );
//         debug( "PixmapCollection count: %d", pixmapList.count() );
//         debug( "%d: ", pixmapList.count() - 1 );
// #endif

//         if ( addref )
//             kppixmap->addRef();

// #ifdef SHOW_INFO
//         debug( "-------------" );
// #endif

//         if ( orig )
//             return kppixmap->getOrigPixmap();
//         else
//             return kppixmap->getPixmap();
//     }
//     else
//     {

// #ifdef SHOW_INFO
//         debug( "%d: ", num );
// #endif

//         if ( addref )
//             pixmapList.at( num )->addRef();

// #ifdef SHOW_INFO
//         debug( "-------------" );
// #endif

//         if ( orig )
//             return pixmapList.at( num )->getOrigPixmap();
//         else
//             return pixmapList.at( num )->getPixmap();
//     }
// }

// /*====================== remove ref =============================*/
// void KPPixmapCollection::removeRef( QString _filename, KSize _size )
// {
//     int num = inPixmapList( _filename, _size );

//     if ( num != -1 )
//     {
//         if ( pixmapList.at( num )->removeRef() )
//         {
//             pixmapList.remove( num );

// #ifdef SHOW_INFO
//             debug( "remove Pixmap '%s'", _filename.data() );
//             debug( "PixmapCollection count: %d\n", pixmapList.count() );
// #endif

//         }
//     }
// }

// /*====================== remove ref =============================*/
// void KPPixmapCollection::removeRef( QString _filename, QString _data, KSize _size )
// {
//     int num = inPixmapList( _filename, _data, _size );

//     if ( num != -1 )
//     {
//         if ( pixmapList.at( num )->removeRef() )
//         {
//             pixmapList.remove( num );

// #ifdef SHOW_INFO
//             debug( "remove Pixmap '%s'", _filename.data() );
//             debug( "PixmapCollection count: %d\n", pixmapList.count() );
// #endif

//         }
//     }
// }

// /*========================== in pixmap list? ====================*/
// int KPPixmapCollection::inPixmapList( QString _filename, KSize _size )
// {
//     if ( !pixmapList.isEmpty() )
//     {
//         KPPixmap *kppixmap = 0;
//         for ( int i = 0; i < static_cast<int>( pixmapList.count() ); i++ )
//         {
//             kppixmap = pixmapList.at( i );
//             if ( kppixmap->getFilename() == _filename && ( kppixmap->getSize() == _size ||
//                                                            _size == orig_size &&
//                                                            kppixmap->getSize() == kppixmap->getOrigSize() ) )
//             {
//                 QImage img1, img2;
//                 img1 = *kppixmap->getOrigPixmap();
//                 img2 = QPixmap( _filename );

//                 if ( img1 != img2 )
//                     continue;

//                 return i;
//             }
//         }
//         return -1;
//     }
//     else return -1;
// }

// /*========================== in pixmap list? ====================*/
// int KPPixmapCollection::inPixmapList( QString _filename, QString _data, KSize _size )
// {
//     if ( !pixmapList.isEmpty() )
//     {
//         KPPixmap *kppixmap = 0;
//         for ( int i = 0; i < static_cast<int>( pixmapList.count() ); i++ )
//         {
//             kppixmap = pixmapList.at( i );
//             if ( kppixmap->getFilename() == _filename && ( kppixmap->getPixData() == _data || kppixmap->getPixDataNative() == _data ) &&
//                  ( kppixmap->getSize() == _size || _size == orig_size && kppixmap->getSize() == kppixmap->getOrigSize() ) )
//                 return i;
//         }
//         return -1;
//     }
//     else return -1;
// }




