/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (c) 2001 David Faure <faure@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <goutte@kde.org>

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

#include "PictureClipart.h"

#include <QBuffer>
#include <QPainter>
#include <QPixmap>

#include <kdebug.h>


#include "PictureKey.h"
#include "PictureBase.h"


PictureClipart::PictureClipart(void) : m_clipart(PictureType::formatVersionQPicture)
{
}

PictureClipart::~PictureClipart(void)
{
}

PictureBase* PictureClipart::newCopy(void) const
{
    return new PictureClipart(*this);
}

PictureType::Type PictureClipart::getType(void) const
{
    return PictureType::TypeClipart;
}

bool PictureClipart::isNull(void) const
{
    return m_clipart.isNull();
}

void PictureClipart::drawQPicture(QPicture& clipart, QPainter& painter,
                                    int x, int y, int width, int height, int sx, int sy, int sw, int sh)
{
    kDebug(30508) << "Drawing PictureClipart" << this;
    kDebug(30508) << "  x=" << x << " y=" << y << " width=" << width << " height=" << height;
    kDebug(30508) << "  sx=" << sx << " sy=" << sy << " sw=" << sw << " sh=" << sh;
    painter.save();
    // Thanks to Harri, Qt3 makes it much easier than Qt2 ;)
    QRect br = clipart.boundingRect();
    kDebug(30508) << "  Bounding rect." << br;

    painter.translate(x, y); // Translating must be done before scaling!
    if (br.width() && br.height())
        painter.scale(qreal(width) / qreal(br.width()), qreal(height) / qreal(br.height()));
    else
        kWarning(30508) << "Null bounding rectangle: " << br.width() << " x " << br.height();
    painter.drawPicture(0, 0, clipart);
    painter.restore();
}

void PictureClipart::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool /*fastMode*/)
{
    drawQPicture(m_clipart, painter, x, y, width, height, sx, sy, sw, sh);
}

bool PictureClipart::loadData(const QByteArray& array, const QString& extension)
{
    // Second, create the original clipart
    kDebug(30508) << "Trying to load clipart... (Size:" << m_rawData.size() << ")";
    m_rawData = array;
    QBuffer buffer(&m_rawData);
    buffer.open(QIODevice::ReadOnly);
    bool check = true;
    if (extension == "svg") {
        if (!m_clipart.load(&buffer, "svg")) {
            kWarning(30508) << "Loading SVG has failed! (PictureClipart::load)";
            check = false;
        }
    } else {
        if (!m_clipart.load(&buffer, NULL)) {
            kWarning(30508) << "Loading QPicture has failed! (PictureClipart::load)";
            check = false;
        }
    }
    buffer.close();
    return check;
}

bool PictureClipart::save(QIODevice* io) const
{
    // We save the raw data, as the SVG supposrt in QPicture is poor
    qint64 size = io->write(m_rawData); // WARNING: writeBlock returns Q_LONG but size() Q_ULONG!
    return (size == m_rawData.size());
}

QSize PictureClipart::getOriginalSize(void) const
{
    return m_clipart.boundingRect().size();
}

QPixmap PictureClipart::generatePixmap(const QSize& size, bool /*smoothScale*/)
{
    // Not sure if it works, but it worked for PictureFilePreviewWidget::setClipart
    QPixmap pixmap(size);
    QPainter p;

    p.begin(&pixmap);
    p.setBackground(QBrush(Qt::white));
    pixmap.fill(Qt::white);

    QRect br = m_clipart.boundingRect();
    if (br.width() && br.height())
        p.scale((qreal)pixmap.width() / (qreal)br.width(), (qreal)pixmap.height() / (qreal)br.height());
    p.drawPicture(0, 0, m_clipart);
    p.end();
    return pixmap;
}

QString PictureClipart::getMimeType(const QString& extension) const
{
    if (extension == "svg")
        return "image/svg+xml";
    else
        return "image/x-vnd.trolltech.qpicture";
}

