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

#include <koClipartCollection.h>

class QDomElement;
class QDomDocument;

typedef KoClipartKey KPClipartKey;
typedef KoClipart KPClipart;

/******************************************************************/
/* Class: KPClipartCollection                                     */
/******************************************************************/
class KPClipartCollection : public KoClipartCollection
{
public:
    typedef KoClipartKey Key; // to avoid porting everything. Get rid if you want to.

    KPClipartCollection()
        : allowChangeRef( true )
    { date = QDate::currentDate(); time = QTime::currentTime(); }
    ~KPClipartCollection() {}

    // KPresenter uses dateTime.isValid() for images in the collection and
    // !isValid() for images to be loaded from disk.
    // This method handles both cases.
    KPClipart findOrLoad( const QString & fileName, const QDateTime & dateTime );

    //void addRef( const Key &key );
    //void removeRef( const Key &key );

    //int references( const Key &key ) { return refs.contains( key ) ? refs.find( key ).data() : -1; }

    //void setAllowChangeRef( bool b )
    //{ allowChangeRef = b ; }

    QDate tmpDate() { return date; }
    QTime tmpTime() { return time; }

protected:
    //QMap< Key, int > refs;

    QDate date;
    QTime time;

    bool allowChangeRef;
};

#endif
