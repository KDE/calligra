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

#include "PictureBase.h"

#include <KoXmlWriter.h>

#include <kdebug.h>
#include <ksharedconfig.h>
#include <kglobal.h>

#include <kcodecs.h>
#include <QPainter>
#include <QMimeData>
#include <QPixmap>
#include <QBuffer>
#include <kconfiggroup.h>

static int s_useSlowResizeMode = -1; // unset

PictureBase::PictureBase(void)
{
    // Slow mode can be very slow, especially at high zoom levels -> configurable
    if (s_useSlowResizeMode == -1) {
        KConfigGroup group(KGlobal::config(), "KOfficeImage");
        s_useSlowResizeMode = group.readEntry("HighResolution", 1);
        kDebug(30508) << "HighResolution =" << s_useSlowResizeMode;
    }
}

PictureBase::~PictureBase(void)
{
}

PictureBase* PictureBase::newCopy(void) const
{
    return new PictureBase(*this);
}

PictureType::Type PictureBase::getType(void) const
{
    return PictureType::TypeUnknown;
}

bool PictureBase::isNull(void) const
{
    return true;    // A PictureBase is always null.
}

void PictureBase::draw(QPainter& painter, int x, int y, int width, int height, int, int, int, int, bool /*fastMode*/)
{
    // Draw a light red box (easier DEBUG)
    kWarning(30508) << "Drawing light red rectangle! (PictureBase::draw)";
    painter.save();
    painter.setBrush(QColor(128, 0, 0));
    painter.drawRect(x, y, width, height);
    painter.restore();
}

bool PictureBase::load(QIODevice* io, const QString& extension)
{
    return loadData(io->readAll(), extension);
}

bool PictureBase::loadData(const QByteArray&, const QString&)
{
    // Nothing to load!
    return false;
}

bool PictureBase::save(QIODevice*) const
{
    // Nothing to save!
    return false;
}

bool PictureBase::saveAsBase64(KoXmlWriter& writer) const
{
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    if (!save(&buffer))
        return false;
    QByteArray encoded = buffer.buffer().toBase64();
    writer.addTextNode(encoded);
    return true;
}

QSize PictureBase::getOriginalSize(void) const
{
    return QSize(0, 0);
}

QPixmap PictureBase::generatePixmap(const QSize&, bool /*smoothScale*/)
{
    return QPixmap();
}

QString PictureBase::getMimeType(const QString&) const
{
    return QString(NULL_MIME_TYPE);
}

bool PictureBase::isSlowResizeModeAllowed(void) const
{
    return s_useSlowResizeMode != 0;
}

QMimeData* PictureBase::dragObject(QWidget * dragSource, const char * name)
{
    Q_UNUSED(dragSource);
    QImage image(generateImage(getOriginalSize()));
    if (image.isNull())
        return 0;
    else {
        QMimeData* mimeData = new QMimeData();
        mimeData->setImageData(image);
        mimeData->setObjectName(name);
        return mimeData; // XXX: Qt3 use dragsource here?
    }
}

QImage PictureBase::generateImage(const QSize& size)
{
    return generatePixmap(size, true).toImage();
}

void PictureBase::clearCache(void)
{
    // Nothign to do!
}
