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

#ifndef kpclipartcollection_h
#define kpclipartcollection_h

#include <qmap.h>
#include <qdatetime.h>
#include <qpicture.h>

class QDomElement;
class QDomDocument;

/******************************************************************/
/* Class: KPClipartCollection                                     */
/******************************************************************/

class KPClipartCollection
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

        QDomElement saveXML( QDomDocument &doc );
        void setAttributes( QDomElement &elem );

        QString filename;
        QDateTime lastModified;
    };

    KPClipartCollection()
        : allowChangeRef( true )
    { date = QDate::currentDate(); time = QTime::currentTime(); }
    ~KPClipartCollection();

    QPicture *findClipart( const Key &key );
    QPicture *insertClipart( const Key &key, const QPicture &pic );

    void addRef( const Key &key );
    void removeRef( const Key &key );

    QMap< Key, QPicture >::Iterator begin() { return data.begin(); }
    QMap< Key, QPicture >::Iterator end() { return data.end(); }

    int references( const Key &key ) { return refs.contains( key ) ? refs.find( key ).data() : -1; }

    QDate tmpDate() { return date; }
    QTime tmpTime() { return time; }

    void setAllowChangeRef( bool b )
    { allowChangeRef = b ; }

protected:
    QMap< Key, QPicture > data;
    QMap< Key, int > refs;

    QDate date;
    QTime time;

    bool allowChangeRef;

};

#endif
