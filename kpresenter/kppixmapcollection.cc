/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kppixmapcollection.h"

#include <qstring.h>
#include <qwmatrix.h>

#include <stdio.h>

#ifndef QT_NO_ASCII_CAST
#define QT_NO_ASCII_CAST
#endif
#ifndef QT_NO_CAST_ASCII
#define QT_NO_CAST_ASCII
#endif

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
    QMap< Key, QImage >::Iterator it = data.find ( key );

    if ( it != data.end() && it.key() == key ) {
        return &it.data();
    } else {
        //kdDebug() << "    did NOT find pixmap data" << endl;
        return 0L;
    }
}

/*================================================================*/
QImage *KPPixmapDataCollection::insertPixmapData( const Key &key, const QImage &img )
{
    //kdDebug() << "    KPPixmapDataCollection::insertPixmapData( key = " << key.toString() << ", img = " << !img.isNull() << " )" << endl;

    QImage *tmp = findPixmapData( key );
    if ( tmp )
        return tmp;
    QImage *image = new QImage( img );

    image->detach();
    data.insert( Key( key ), *image );

    int ref = 1;
    refs.insert( Key( key ), ref );

    return image;
    return &data[ key ];
}

/*================================================================*/
void KPPixmapDataCollection::setPixmapOldVersion( const Key &key, const QString &_data )
{
    if ( data.contains( key ) )
        return;

    QCString s(_data.latin1());
    int i = s.find( ( char )1, 0 );

    while ( i != -1 ) {
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

//     kdDebug() << "    KPPixmapDataCollection::addRef( key = " << key.toString() << " )" << endl;

    if ( refs.contains( key ) ) {
        int ref = refs[ key ];
        refs[ key ] = ++ref;
//      kdDebug() << "    ref: " << refs[ key ] << endl;
    }
}

/*================================================================*/
void KPPixmapDataCollection::removeRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

//     kdDebug() << "    KPPixmapDataCollection::removeRef( key = " << key.toString() << " )" << endl;

    if ( refs.contains( key ) ) {
        int ref = refs[ key ];
        refs[ key ] = --ref;
//      kdDebug() << "    ref: " << refs[ key ] << endl;

        if ( ref == 0 ) {
            refs.remove( key );
            data.remove( key );
//          kdDebug() << "    remove " << key.toString() << endl;
        }
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
    //kdDebug() << "     KPPixmapCollection::findPixmap( key = " << key.toString() << " )" << endl;

    if ( key.size == orig_size ) {
        QImage *i = dataCollection.findPixmapData( key.dataKey );
        if ( i )
            key.size = i->size();
        else {
            QImage img( key.dataKey.filename );
            key.size = img.size();
        }
    }

    //kdDebug() << "       key = " << key.toString() << " )" << endl;

    QMap< Key, QPixmap >::Iterator it = pixmaps.begin();
    it = pixmaps.find( key );

    if ( it != pixmaps.end() && it.key() == key ) {
        //kdDebug() << "    pixmap found in pixmaps: " << it.key().toString() << endl;
        addRef( key );
        return &it.data();
    } else {
        QImage *img = dataCollection.findPixmapData( key.dataKey );
        if ( img ) {
            //kdDebug() << "       pixmap found in data collection: " << key.dataKey.toString() << "\n" << endl;
            dataCollection.addRef( key.dataKey );
            return loadPixmap( *img, key, true );
        } else {
            QImage image( key.dataKey.filename );

            //kdDebug() << "       pixmap not found anywhere\n" << endl;
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

//     kdDebug() << "    KPPixmapCollection::addRef( key = " << key.toString() << " )\n" << endl;

    if ( refs.contains( key ) ) {
        int ref = refs[ key ];
        refs[ key ] = ++ref;
//      kdDebug() << "    ref: " << refs[ key ] << "\n" << endl;
    }

    dataCollection.addRef( key.dataKey );
}

/*================================================================*/
void KPPixmapCollection::removeRef( const Key &key )
{
    if ( !allowChangeRef )
        return;

//     kdDebug() << "    KPPixmapCollection::removeRef( key = " << key.toString() << " )\n" << endl;

    if ( refs.contains( key ) ) {
        int ref = refs[ key ];
        refs[ key ] = --ref;
//      kdDebug() << "    ref: " << refs[ key ] << "\n" << endl;

        if ( ref == 0 ) {
            refs.remove( key );
            pixmaps.remove( key );
//          kdDebug() << "         remove: " << key.toString() << endl;
        }
    }

    dataCollection.removeRef( key.dataKey );
}

/*================================================================*/
QPixmap *KPPixmapCollection::loadPixmap( const QImage &image, const Key &key, bool insert )
{
    //kdDebug() << "     KPPixmapCollection::loadPixmap( image = " << !image.isNull() << ", key = " << key.toString() << ", insert = " << insert << " )" << endl;

    QPixmap *pixmap = new QPixmap;
    pixmap->convertFromImage( image );

    if ( !pixmap->isNull() ) {
        QSize size = key.size;
        if ( size != pixmap->size() && size != orig_size && pixmap->width() != 0 && pixmap->height() != 0 ) {
            QWMatrix m;
            m.scale( static_cast<float>( size.width() ) / static_cast<float>( pixmap->width() ),
                     static_cast<float>( size.height() ) / static_cast<float>( pixmap->height() ) );
            *pixmap = pixmap->xForm( m );
        }
    }

    if ( insert ) {
        pixmaps.insert( Key( key ), *pixmap );
        int ref = 1;
        refs.insert( Key( key ), ref );
        return pixmap;
        return &pixmaps[ key ];
    }

    return pixmap;
}

/*================================================================*/
QTextStream& operator<<( QTextStream &out, KPPixmapDataCollection::Key &key )
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

/*================================================================*/
QTextStream& operator<<( QTextStream &out, KPPixmapCollection::Key &key )
{
    out << key.dataKey << " width=\"" << key.size.width() << "\" height=\""
        << key.size.height() << "\" ";

    return out;
}
