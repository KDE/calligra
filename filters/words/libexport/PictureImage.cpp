/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002, 2003 Nicolas GOUTTE <goutte@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "PictureImage.h"

#include "PictureKey.h"

#include <kdebug.h>
#include <kmimetype.h>

#include <QBuffer>
#include <QPainter>
#include <QPrinter>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QApplication>
#include <QMimeData>

class PictureImage::Private
{
};

PictureImage::PictureImage(void) : m_cacheIsInFastMode(true), d(0)
{
}

PictureImage::~PictureImage(void)
{
    delete d;
}

PictureBase* PictureImage::newCopy(void) const
{
    return new PictureImage(*this);
}

PictureType::Type PictureImage::getType(void) const
{
    return PictureType::TypeImage;
}

bool PictureImage::isNull(void) const
{
    return m_originalImage.isNull();
}

void PictureImage::scaleAndCreatePixmap(const QSize& size, bool fastMode)
{
    if ((size == m_cachedSize)
            && ((fastMode) || (!m_cacheIsInFastMode))) {
        // The cached pixmap has already the right size
        // and:
        // - we are in fast mode (We do not care if the re-size was done slowly previously)
        // - the re-size was already done in slow mode
        return;
    }

    // Slow mode can be very slow, especially at high zoom levels -> configurable
    if (!isSlowResizeModeAllowed()) {
        kDebug(30508) << "User has disallowed slow mode!";
        fastMode = true;
    }

    // Use QImage::scale if we have fastMode==true
    if (fastMode) {
        m_cachedPixmap = QPixmap::fromImage(m_originalImage.scaled(size), Qt::ColorOnly);
        // Always color or else B/W can be reversed
        m_cacheIsInFastMode = true;
    } else {
        m_cachedPixmap = QPixmap::fromImage(m_originalImage.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), Qt::ColorOnly);
        // Always color or else B/W can be reversed
        m_cacheIsInFastMode = false;
    }
    m_cachedSize = size;
}

void PictureImage::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool fastMode)
{
    //kDebug(30508) <<"KoImage::draw currentSize:" << currentSize.width() <<"x" << currentSize.height();
    if (!width || !height)
        return;
    QSize origSize = getOriginalSize();
    const bool scaleImage = dynamic_cast<QPrinter*>(painter.device()) != 0 // we are printing
                            && ((width <= origSize.width()) || (height <= origSize.height()));
    if (scaleImage) {
        // use full resolution of image
        qreal xScale = qreal(width) / qreal(origSize.width());
        qreal yScale = qreal(height) / qreal(origSize.height());

        painter.save();
        painter.translate(x, y);
        painter.scale(xScale, yScale);
        // Note that sx, sy, sw and sh are unused in this case. Not a problem, since it's about printing.
        // Note 2: we do not cache the QPixmap. As we are printing, the next time we will probably
        //   need again the screen version.
        painter.drawImage(0, 0, m_originalImage);
        painter.restore();
    } else {
        QSize screenSize(width, height);
        //kDebug(30508) <<"PictureImage::draw screenSize=" << screenSize.width() <<"x" << screenSize.height();

        scaleAndCreatePixmap(screenSize, fastMode);

        // sx,sy,sw,sh is meant to be used as a cliprect on the pixmap, but drawPixmap
        // translates it to the (x,y) point -> we need (x+sx, y+sy).
        painter.drawPixmap(x + sx, y + sy, m_cachedPixmap, sx, sy, sw, sh);
    }
}

bool PictureImage::loadData(const QByteArray& array, const QString& /* extension*/)
{
    m_rawData = array;
    // Second, create the original image
    QBuffer buffer(&m_rawData);
    buffer.open(QIODevice::ReadWrite);
    QImageReader imageReader(&buffer);

    QImage image = imageReader.read();
    buffer.close();
    if (image.isNull()) {
        kError(30508) << "Image could not be loaded!" << endl;
        return false;
    }
    m_originalImage = image;

    return true;
}

bool PictureImage::save(QIODevice* io) const
{
    kDebug(30508) << "writing raw data. size=" << m_rawData.size();
    // We save the raw data, to avoid damaging the file by many load/save cycles (especially for JPEG)
    qint64 size = io->write(m_rawData); // WARNING: writeBlock returns Q_LONG but size() Q_ULONG!
    return (size == m_rawData.size());
}

QSize PictureImage::getOriginalSize(void) const
{
    return m_originalImage.size();
}

QPixmap PictureImage::generatePixmap(const QSize& size, bool smoothScale)
{
    scaleAndCreatePixmap(size, !smoothScale);
    return m_cachedPixmap;
}

QString PictureImage::getMimeType(const QString& extension) const
{
    QString fileName("/tmp/temp.");
    fileName += extension;
    // Find the mimetype only by the extension, not by file content (as the file is empty!)
    const QString mimetype(KMimeType::findByPath(fileName, 0 , true)->name());
    // ### TODO: use KMimeType::findByContent (but then the mimetype probably need to be cached)
    kDebug(30508) << "Image is mime type:" << mimetype;
    return mimetype;
}

QMimeData* PictureImage::dragObject(QWidget *dragSource, const char *name)
{
    Q_UNUSED(dragSource); // Was used in Qt3? 
    QMimeData* mimeData = new QMimeData();
    mimeData->setImageData(m_originalImage);
    mimeData->setObjectName(name);
    return mimeData;
}

QImage PictureImage::generateImage(const QSize& size)
{
    // We do not cache the image, as we will seldom need it again.
    return m_originalImage.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

void PictureImage::clearCache(void)
{
    m_cachedPixmap = QPixmap();
    m_cacheIsInFastMode = true;
    m_cachedSize = QSize();
}
