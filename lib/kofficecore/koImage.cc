/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>

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

#include "koImage.h"
#include <qshared.h>
#include <qdom.h>
#include <qfileinfo.h>
#include <assert.h>

class KoImagePrivate : public QShared
{
public:
    QImage m_image;
    KoImage m_originalImage;
    KoImageKey m_key;
    mutable QPixmap m_cachedPixmap;
};


KoImage::KoImage()
{
    d = 0;
}

KoImage::KoImage( const KoImageKey &key, const QImage &image )
{
    d = new KoImagePrivate;
    d->m_image = image.copy();
    d->m_key = key;
}

KoImage::KoImage( const KoImage &other )
{
    d = 0;
    (*this) = other;
}

KoImage::~KoImage()
{
    if ( d && d->deref() )
        delete d;
}

KoImage &KoImage::operator=( const KoImage &_other )
{
    KoImage &other = const_cast<KoImage &>( _other );

    if ( other.d )
        other.d->ref();

    if ( d && d->deref() )
        delete d;

    d = other.d;

    return *this;
}

QImage KoImage::image() const
{
    if ( !d) return QImage();
    return d->m_image;
}

QPixmap KoImage::pixmap() const
{
    if ( !d ) return QPixmap();

    if ( d->m_cachedPixmap.isNull() )
        d->m_cachedPixmap = d->m_image; // automatic conversion using assignment operator
    return d->m_cachedPixmap;
}

KoImageKey KoImage::key() const
{
    if ( !d ) return KoImageKey();

    return d->m_key;
}

bool KoImage::isNull() const
{
    return d == 0 || d->m_image.isNull();
}

QSize KoImage::size() const
{
    if ( !d ) return QSize();

    return d->m_image.size();
}

QSize KoImage::originalSize() const
{
    if ( !d ) return QSize();

    KoImage originalImage;

    if ( !d->m_originalImage.isNull() )
        originalImage = d->m_originalImage;
    else
        originalImage = *this;

    return originalImage.size();
}

KoImage KoImage::scale( const QSize &size ) const
{
    if ( !d )
        return *this;

    KoImage originalImage;

    if ( !d->m_originalImage.isNull() )
        originalImage = d->m_originalImage;
    else
        originalImage = *this;

    if ( originalImage.size() == size )
        return originalImage;

    QImage scaledImg = originalImage.image().smoothScale( size.width(), size.height() );

    KoImage result( d->m_key, scaledImg );
    assert( result.d );
    result.d->m_originalImage = originalImage;

    return result;
}

//////////

void KoImageKey::saveAttributes( QDomElement &elem ) const
{
    QDate date = m_lastModified.date();
    QTime time = m_lastModified.time();
    elem.setAttribute( "filename", m_filename );
    elem.setAttribute( "year", date.year() );
    elem.setAttribute( "month", date.month() );
    elem.setAttribute( "day", date.day() );
    elem.setAttribute( "hour", time.hour() );
    elem.setAttribute( "minute", time.minute() );
    elem.setAttribute( "second", time.second() );
    elem.setAttribute( "msec", time.msec() );
}

void KoImageKey::loadAttributes( const QDomElement &elem, const QDate &dDate, const QTime &dTime )
{
    int year=dDate.year(), month=dDate.month(), day=dDate.day();
    int hour=dTime.hour(), minute=dTime.minute(), second=dTime.second(), msec=dTime.msec();
    if( elem.hasAttribute( "key" ) )
        m_filename=elem.attribute( "key" ); // Old KWord format (up to 1.1-beta2)
    else
        m_filename=elem.attribute( "filename" );

    if( elem.hasAttribute( "year" ) )
        year=elem.attribute( "year" ).toInt();
    if( elem.hasAttribute( "month" ) )
        month=elem.attribute( "month" ).toInt();
    if( elem.hasAttribute( "day" ) )
        day=elem.attribute( "day" ).toInt();
    if( elem.hasAttribute( "hour" ) )
        hour=elem.attribute( "hour" ).toInt();
    if( elem.hasAttribute( "minute" ) )
        minute=elem.attribute( "minute" ).toInt();
    if( elem.hasAttribute( "second" ) )
        second=elem.attribute( "second" ).toInt();
    if( elem.hasAttribute( "msec" ) )
        msec=elem.attribute( "msec" ).toInt();
    m_lastModified.setDate( QDate( year, month, day ) );
    m_lastModified.setTime( QTime( hour, minute, second, msec ) );
}

QString KoImageKey::format() const
{
    QString format = QFileInfo( m_filename ).extension().upper();
    if ( format == "JPG" )
        format = "JPEG";
    if ( QImage::outputFormats().find( format.latin1() ) == -1 )
        format = "PNG";
    return format;
}
