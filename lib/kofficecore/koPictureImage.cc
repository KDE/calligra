/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <nicog@snafu.de>

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

#include <qbuffer.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qapplication.h>

#include <kconfig.h>
#include <kglobal.h>

#include "koPictureKey.h"
#include "koPictureBase.h"
#include "koPictureImage.h"

class KoPictureImagePrivate : public QShared
{
public:
    KoPictureImagePrivate() {}
public:
    QImage  m_originalImage;
    QByteArray m_rawData;
    // No idea why it is a mutable, but as it was in KoImage I suppose that there is a reason.
    mutable QPixmap m_cachedPixmap;
    QSize m_cachedSize;
    QSize m_size;
    QString m_extension;
};


KoPictureImage::KoPictureImage(void) : d(NULL)
{
}

KoPictureImage::~KoPictureImage(void)
{
    if ( d && d->deref() )
        delete d;
}

KoPictureImage::KoPictureImage(const KoPictureImage& other)
{
    d = 0;
    (*this) = other;
}

KoPictureImage& KoPictureImage::operator=(const KoPictureImage& other)
{
    if (other.d)
        other.d->ref();

    if (d && d->deref())
        delete d;

    d=other.d;

    return *this;
}

KoPictureBase* KoPictureImage::newCopy(void) const
{
    return new KoPictureImage(*this);
}

KoPictureType::Type KoPictureImage::getType(void) const
{
    return KoPictureType::TypeImage;
}

bool KoPictureImage::isNull(void) const
{
    if (!d)
        return true;
    return d->m_originalImage.isNull();
}

void KoPictureImage::scaleAndCreatePixmap(const QSize& size, bool fastMode) const
{
    if ( !d )
        return;

    if (size==d->m_cachedSize)
    {
        // The cached pixmap has already the right size
        return;
    }

    // Slow mode can be very slow, especially at high zoom levels -> configurable
    static int s_useSlowResizeMode = -1; // unset
    if ( s_useSlowResizeMode == -1 )
    {
        KConfigGroup group( KGlobal::config(), "KOfficeImage" );
        s_useSlowResizeMode = group.readNumEntry( "HighResolution", 1 );
    }
    if ( s_useSlowResizeMode == 0 )
        fastMode = true;

    QImage image;
    // Use QImage::scale if we have fastMode==true
    if ( fastMode )
    {
        image = d->m_originalImage.smoothScale( size );
    }
    else
    {
        //kdDebug() << "KoPictureImage::scale loading from raw data" << endl;
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
        image = io.image();
        if ( image.size() != size ) // this can happen due to rounding problems
        {
            image = image.scale( size );
            //kdDebug() << "fixing size to " << size.width() << "x" << size.height() << endl;
        }
        QApplication::restoreOverrideCursor();
    }

    // Now create and cache the new pixmap
    d->m_cachedPixmap=image;
    d->m_cachedSize=size;
}

void KoPictureImage::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh)
{
    if ( !d )
        return;
    //kdDebug() << "KoImage::draw currentSize:" << currentSize.width() << "x" << currentSize.height() << endl;
    if ( !width || !height )
        return;
    QSize origSize = getOriginalSize();
    const bool scaleImage = painter.device()->isExtDev() // we are printing
        && ((width <= origSize.width()) || (height <= origSize.height()));
    if( scaleImage )
    {
        // use full resolution of image
        double xScale = double(width) / double(origSize.width());
        double yScale = double(height) / double(origSize.height());

        painter.save();
        painter.translate( x, y );
        painter.scale( xScale, yScale );
         // Note that sx, sy, sw and sh are unused in this case. Not a problem, since it's about printing.
        painter.drawPixmap(0, 0, QPixmap(d->m_originalImage));
        painter.restore();

    }
    else
    {
        QSize screenSize( width, height );
        //kdDebug() << "KoPictureImage::draw screenSize=" << screenSize.width() << "x" << screenSize.height() << endl;
        if  (screenSize!=d->m_cachedSize)
        {
            scaleAndCreatePixmap(screenSize);
        }

        // sx,sy,sw,sh is meant to be used as a cliprect on the pixmap, but drawPixmap
        // translates it to the (x,y) point -> we need (x+sx, y+sy).
        painter.drawPixmap( x + sx, y + sy, d->m_cachedPixmap, sx, sy, sw, sh );
    }
}

bool KoPictureImage::load(QIODevice* io)
{
    if ( d && d->deref() )
        delete d;

    d = new KoPictureImagePrivate;

    // First, read the raw data
    d->m_rawData=io->readAll();

    // Second, create the original image
    QBuffer buffer(d->m_rawData);
    buffer.open(IO_ReadWrite);
    QImageIO imageIO(&buffer,NULL); // JPEG

    if (!imageIO.read())
    {
        delete d;
        d=NULL;
        return false;
    }
    buffer.close();
    d->m_originalImage=imageIO.image();

    return true;
}

bool KoPictureImage::save(QIODevice* io)
{
    if (!d)
        return false;
    // We save the raw data, to avoid damaging the file by many load/save cyvles (especially for JPEG)
    Q_ULONG size=io->writeBlock(d->m_rawData); // WARNING: writeBlock returns Q_LONG but size() Q_ULONG!
    return (size==d->m_rawData.size());
}

QSize KoPictureImage::getOriginalSize(void) const
{
    if (!d)
        return QSize(0,0);
    return d->m_originalImage.size();
}

QPixmap KoPictureImage::generatePixmap(const QSize& size)
{
    if (d)
    {
        scaleAndCreatePixmap(size,true); // Alwas fast mode!
        return d->m_cachedPixmap;
    }
    return QPixmap();
}

QString KoPictureImage::getExtension(void) const
{
    if ( !d )
        return "null";
    return d->m_extension;
}

void KoPictureImage::setExtension(const QString& extension)
{
    if ( d )
        d->m_extension = extension;
}

QSize KoPictureImage::getSize(void) const
{
    if ( !d )
        return QSize( -1, -1 );
    return d->m_size;
}

void KoPictureImage::setSize(const QSize& size)
{
    if ( d )
        d->m_size = size;
}
