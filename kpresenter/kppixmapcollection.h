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

#ifndef kppixmapcollection_h
#define kppixmapcollection_h

#include <qlist.h>
#include <qmap.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qcstring.h>

#include "global.h"

#include <qtextstream.h>

/******************************************************************/
/* Class: KPPixmapDataCollection                                  */
/******************************************************************/

class KPPixmapDataCollection
{
public:
    struct Key
    {
        Key()
            : filename(), lastModified()
        {}

        Key( const QString &fn, const QDateTime &mod )
            : filename( fn ), lastModified( mod )
        {}
        Key( const Key &key )
            : filename( key.filename ), lastModified( key.lastModified )
        {}

        Key &operator=( const Key &key ) {
            filename = key.filename;
            lastModified = key.lastModified;
            return *this;
        }

        bool operator==( const Key &key ) const {
            return ( key.filename == filename &&
                     key.lastModified == lastModified );
        }

        bool operator<( const Key &key ) const {
            QString s1( key.toString() );
            QString s2( toString() );
            return ( s1 < s2 );
        }

        QString toString() const {
            QString s = QString( "%1_%2" ).arg( filename ).arg( lastModified.toString() );
            return QString( s );
        }

        QString filename;
        QDateTime lastModified;
    };

    KPPixmapDataCollection()
        : allowChangeRef( true )
    {}
    ~KPPixmapDataCollection();

    QImage *findPixmapData( const Key &key );
    QImage *insertPixmapData( const Key &key, const QImage &img );

    void setPixmapOldVersion( const Key &key, const QString &_data );
    void setPixmapOldVersion( const Key &key );

    void addRef( const Key &key );
    void removeRef( const Key &key );

    QMap< Key, QImage >::Iterator begin() { return data.begin(); }
    QMap< Key, QImage >::Iterator end() { return data.end(); }

    int references( const Key &key ) { return refs.contains( key ) ? refs.find( key ).data() : -1; }

    void setAllowChangeRef( bool b )
    { allowChangeRef = b ; }

protected:
    QMap< Key, QImage > data;
    QMap< Key, int > refs;

    bool allowChangeRef;

};

/******************************************************************/
/* Class: KPPixmapCollection                                      */
/******************************************************************/

class KPPixmapCollection
{
public:
    struct Key
    {
        Key()
            : dataKey(), size( orig_size )
        {}

        Key( const KPPixmapDataCollection::Key &dk, const QSize &sz )
            : dataKey( dk ), size( sz )
        {}

        Key( const Key &key )
            : dataKey( key.dataKey ), size( key.size )
        {}

        Key &operator=( const Key &key ) {
            dataKey = key.dataKey;
            size = key.size;
            return *this;
        }

        bool operator==( const Key &key ) const {
            return ( dataKey == key.dataKey &&
                     size == key.size );
        }

        bool operator<( const Key &key ) const {
            QString s1( key.toString() );
            QString s2( toString() );
            return s1 < s2;
        }

        QString toString() const {
            QString s = QString( "%1_%2_%3" ).arg( dataKey.toString() ).arg( size.width() ).arg( size.height() );
            return QString( s );
        }

        KPPixmapDataCollection::Key dataKey;
        QSize size;
    };

    KPPixmapCollection()
        : allowChangeRef( true )
    { date = QDate::currentDate(); time = QTime::currentTime(); }
    ~KPPixmapCollection();

    QPixmap* findPixmap( Key &key );

    void addRef( const Key &key );
    void removeRef( const Key &key );

    KPPixmapDataCollection &getPixmapDataCollection() { return dataCollection; }

    QDate tmpDate() { return date; }
    QTime tmpTime() { return time; }

    void setAllowChangeRef( bool b )
    { allowChangeRef = b ; }

protected:
    QPixmap *loadPixmap( const QImage &image, const Key &key, bool insert );

    QMap< Key, QPixmap > pixmaps;
    QMap< Key, int > refs;
    KPPixmapDataCollection dataCollection;

    QDate date;
    QTime time;

    bool allowChangeRef;

};

QTextStream& operator<<( QTextStream &out, KPPixmapDataCollection::Key &key );
QTextStream& operator<<( QTextStream &out, KPPixmapCollection::Key &key );

#endif
