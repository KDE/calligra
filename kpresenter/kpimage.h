/* This file is part of the KDE project
   Copyright (C) 2001 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kpimage_h__
#define __kpimage_h__

#include <koImageCollection.h>

class QDomElement;
class QDomDocument;

struct KPImageKey
{
    KPImageKey()
        : filename(), lastModified()
        {}

    KPImageKey( const QString &fn, const QDateTime &mod )
        : filename( fn ), lastModified( mod )
        {}
    KPImageKey( const KPImageKey &key )
        : filename( key.filename ), lastModified( key.lastModified )
        {}

    KPImageKey &operator=( const KPImageKey &key ) {
        filename = key.filename;
        lastModified = key.lastModified;
        return *this;
    }

    bool operator==( const KPImageKey &key ) const {
        return ( key.filename == filename &&
                 key.lastModified == lastModified );
    }

    bool operator<( const KPImageKey &key ) const {
        return key.toString() < toString();
    }

    QString toString() const {
        return QString::fromLatin1( "%1_%2" ).arg( filename ).arg( lastModified.toString() );
    }

    QDomElement saveXML( QDomDocument &doc );
    void setAttributes( QDomElement &elem );

    QString filename;
    QDateTime lastModified;
};

class KPImageCollection : public KoImageCollection<KPImageKey>
{
public:
    KPImageCollection()
        { m_tmpDate = QDate::currentDate(); m_tmpTime = QTime::currentTime(); }

    KoImage<KPImageKey> loadImage( const KPImageKey &key );

    KoImage<KPImageKey> loadImage( const KPImageKey &key, const QString &rawData );

    // this is ugly, but it was in KPPixmapCollection
    QDate tmpDate() const { return m_tmpDate; }
    QTime tmpTime() const { return m_tmpTime; }

private:
    QDate m_tmpDate;
    QTime m_tmpTime;
};

typedef KPImageCollection::Image KPImage;

#endif
