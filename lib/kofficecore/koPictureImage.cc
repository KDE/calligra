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

KoPictureImage::KoPictureImage(void)
{
}

KoPictureImage::~KoPictureImage(void)
{
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
    return m_originalImage.isNull();
}

void KoPictureImage::scaleAndCreatePixmap(const QSize& size, bool fastMode)
{
    if (size==m_cachedSize)
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
        image = m_originalImage.smoothScale( size );
    }
    else
    {
        //kdDebug() << "KoPictureImage::scale loading from raw data" << endl;
        QApplication::setOverrideCursor( Qt::waitCursor );
        QBuffer buffer( m_rawData );
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
    m_cachedPixmap=image;
    m_cachedSize=size;
}

void KoPictureImage::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool fastMode)
{
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
        painter.drawPixmap(0, 0, QPixmap(m_originalImage));
        painter.restore();

    }
    else
    {
        QSize screenSize( width, height );
        //kdDebug() << "KoPictureImage::draw screenSize=" << screenSize.width() << "x" << screenSize.height() << endl;
        if  (screenSize!=m_cachedSize)
        {
            scaleAndCreatePixmap(screenSize);
        }

        // sx,sy,sw,sh is meant to be used as a cliprect on the pixmap, but drawPixmap
        // translates it to the (x,y) point -> we need (x+sx, y+sy).
        painter.drawPixmap( x + sx, y + sy, m_cachedPixmap, sx, sy, sw, sh );
    }
}

bool KoPictureImage::load(QIODevice* io)
{
    // First, read the raw data
    m_rawData=io->readAll();

    // Second, create the original image
    QBuffer buffer(m_rawData);
    buffer.open(IO_ReadWrite);
    QImageIO imageIO(&buffer,NULL); // JPEG

    if (!imageIO.read())
    {
        buffer.close();
        return false;
    }
    buffer.close();
    m_originalImage=imageIO.image();

    return true;
}

bool KoPictureImage::save(QIODevice* io)
{
    // We save the raw data, to avoid damaging the file by many load/save cyvles (especially for JPEG)
    Q_ULONG size=io->writeBlock(m_rawData); // WARNING: writeBlock returns Q_LONG but size() Q_ULONG!
    return (size==m_rawData.size());
}

QSize KoPictureImage::getOriginalSize(void) const
{
    return m_originalImage.size();
}

QPixmap KoPictureImage::generatePixmap(const QSize& size)
{
    scaleAndCreatePixmap(size,true); // Alwas fast mode!
    return m_cachedPixmap;
}
