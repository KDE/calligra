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
#include <qdom.h>
#include <qfileinfo.h>
#include <assert.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qbuffer.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>

class KoImagePrivate : public QShared
{
public:
    QImage m_image;
    KoImage m_originalImage;
    KoImageKey m_key;
    mutable QPixmap m_cachedPixmap;
    QByteArray m_rawData; // for EPS images
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

KoImage::KoImage( const KoImageKey &key, const QByteArray &rawData )
{
    d = new KoImagePrivate;
    d->m_rawData = rawData;
    d->m_key = key;
}

KoImage::KoImage( const KoImageKey &key, const QByteArray &rawData, const QImage &image )
{
    //kdDebug() << "KoImage::KoImage ctor size:" << image.width() << "x" << image.height() << endl;
    d = new KoImagePrivate;
    d->m_image = image.copy();
    d->m_rawData = rawData;
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
    // On-demand loading of image from raw data
    if ( d->m_image.isNull() && !d->m_rawData.isNull() )
    {
        //kdDebug() << "KoImage::image loading from raw data" << endl;
        QBuffer buffer( d->m_rawData );
        buffer.open( IO_ReadOnly );
        QImageIO io( &buffer, 0 );
        //io.setParameters( QString::number( width ) + ':' + QString::number( height ) );
        io.read();
        /*const_cast<KoImage *>(this)->*/d->m_image = io.image();
    }
    return d->m_image;
}

QByteArray KoImage::rawData() const
{
    if ( !d ) return QByteArray();
    return d->m_rawData;
}

QPixmap KoImage::pixmap() const
{
    if ( !d ) return QPixmap();

    if ( d->m_cachedPixmap.isNull() )
        d->m_cachedPixmap = image(); // automatic conversion using assignment operator
    return d->m_cachedPixmap;
}

KoImageKey KoImage::key() const
{
    if ( !d ) return KoImageKey();

    return d->m_key;
}

bool KoImage::isNull() const
{
    return d == 0 || ( d->m_image.isNull() && d->m_rawData.isNull() );
}

QSize KoImage::size() const
{
    if ( !d ) return QSize();

    return image().size();
}

QSize KoImage::originalSize() const
{
    if ( !d ) return QSize();

    // ### empty size in case of rawdata passed.

    KoImage originalImage;

    if ( !d->m_originalImage.isNull() )
        originalImage = d->m_originalImage;
    else
        originalImage = *this;

    return originalImage.size();
}

KoImage KoImage::scale( const QSize &size, bool fastMode /*=false*/ ) const
{
    if ( !d )
        return *this;
    //kdDebug() << "KoImage::scale " << size.width() << "x" << size.height() << " fastMode=" << fastMode << " hasRawData=" << !d->m_rawData.isNull() << " hasImage=" << !d->m_image.isNull() << endl;

    // Slow mode can be very slow, especially at high zoom levels -> configurable
    static int s_useSlowResizeMode = -1; // unset
    if ( s_useSlowResizeMode == -1 )
    {
        KConfigGroup group( KGlobal::config(), "KOfficeImage" );
        s_useSlowResizeMode = group.readNumEntry( "HighResolution", 1 );
    }
    if ( s_useSlowResizeMode == 0 )
        fastMode = true;

    // Use QImage::scale if we have no raw data, or if we have an image and fastMode=true
    if ( d->m_rawData.isNull() || ( !d->m_image.isNull() && fastMode ) )
    {
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
        result.d->m_rawData = d->m_rawData;
        return result;
    }
    else
    {
        //kdDebug() << "KoImage::scale loading from raw data" << endl;
        QApplication::setOverrideCursor( Qt::waitCursor );
        QBuffer buffer( d->m_rawData );
        buffer.open( IO_ReadOnly );
        QImageIO io( &buffer, 0 );
        QCString params;
        params.setNum( size.width() );
        params += ':';
        QCString height;
        height.setNum( size.height() );
        params += height;
        io.setParameters( params );
        io.read();
        QImage img = io.image();
        if ( img.size() != size ) // this can happen due to rounding problems
        {
            img = img.scale( size );
            //kdDebug() << "fixing size to " << size.width() << "x" << size.height() << endl;
        }
        QApplication::restoreOverrideCursor();
        return KoImage( d->m_key, d->m_rawData, img );
    }
}

void KoImage::draw( QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh )
{
    if ( !d )
        return;
    QSize currentSize = size();
    //kdDebug() << "KoImage::draw currentSize:" << currentSize.width() << "x" << currentSize.height() << endl;
    if ( currentSize.width() == 0 || currentSize.height() == 0 )
        return;
    if ( width == 0 || height == 0 )
        return;
    QSize origSize = originalSize();
    bool scaleImage = painter.device()->isExtDev() // we are printing
                      && ( currentSize.width() < origSize.width()
                           || currentSize.height() < origSize.height() );
    if( scaleImage ) {
        // use full resolution of image
        double xScale = double(width) / double(origSize.width());
        double yScale = double(height) / double(origSize.height());

        painter.save();
        painter.translate( x, y );
        painter.scale( xScale, yScale );
         // Note that sx, sy, sw and sh are unused in this case. Not a problem, since it's about printing.
        painter.drawPixmap( 0, 0, d->m_originalImage.pixmap() );
        painter.restore();

    } else {
        QSize screenSize( width, height );
        //kdDebug() << "KoImage::draw screenSize=" << screenSize.width() << "x" << screenSize.height() << endl;
        if ( screenSize != currentSize )
            *this = scale( screenSize );
        // sx,sy,sw,sh is meant to be used as a cliprect on the pixmap, but drawPixmap
        // translates it to the (x,y) point -> we need (x+sx, y+sy).
        painter.drawPixmap( x + sx, y + sy, pixmap(), sx, sy, sw, sh );
    }
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
