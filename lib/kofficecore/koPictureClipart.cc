/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (c) 2001 David Faure <faure@kde.org>
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

#include <qpainter.h>
#include <qpicture.h>
#include <qpixmap.h>

#include <kdebug.h>
#include <kdebugclasses.h>

#include "koPictureKey.h"
#include "koPictureBase.h"
#include "koPictureClipart.h"

KoPictureClipart::KoPictureClipart(void) : m_clipart(KoPictureType::formatVersionQPicture)
{
}

KoPictureClipart::~KoPictureClipart(void)
{
}

KoPictureBase* KoPictureClipart::newCopy(void) const
{
    return new KoPictureClipart(*this);
}

KoPictureType::Type KoPictureClipart::getType(void) const
{
    return KoPictureType::TypeClipart;
}

bool KoPictureClipart::isNull(void) const
{
    return m_clipart.isNull();
}

void KoPictureClipart::drawQPicture(QPicture& clipart, QPainter& painter,
    int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool /*fastMode*/)
{
    kdDebug(30003) << "Drawing KoPictureClipart " << this << endl;
    kdDebug(30003) << "  x=" << x << " y=" << y << " width=" << width << " height=" << height << endl;
    kdDebug(30003) << "  sx=" << sx << " sy=" << sy << " sw=" << sw << " sh=" << sh << endl;
    painter.save();
    // Thanks to Harri, Qt3 makes it much easier than Qt2 ;)
    QRect br = clipart.boundingRect();
    kdDebug(30003) << "  Bounding rect. " << br << endl;

    painter.translate(x,y); // Translating must be done before scaling!
    if ( br.width() && br.height() )
        painter.scale(double(width)/double(br.width()),double(height)/double(br.height()));
    else
        kdWarning(30003) << "Null bounding rectangle: " << br.width() << " x " << br.height() << endl;
    painter.drawPicture(0,0,clipart);
    painter.restore();
}

void KoPictureClipart::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh)
{
    drawQPicture(m_clipart, painter, x, y, width, height, sx, sy, sw, sh);
}

bool KoPictureClipart::load(QIODevice* io)
{
    // First, read the raw data
    m_rawData=io->readAll();

    // Second, create the original clipart
    kdDebug(30003) << "Trying to load clipart... (Size:" << m_rawData.size() << ")" << endl;
    QBuffer buffer(m_rawData);
    buffer.open(IO_ReadWrite);
    bool check = true;
    if (m_extension=="svg")
    {
        if (!m_clipart.load(&buffer, "svg"))
        {
            kdWarning(30003) << "Loading SVG has failed! (KoPictureClipart::load)" << endl;
            check = false;
        }
    }
    else
    {
        if (!m_clipart.load(&buffer, NULL))
        {
            kdWarning(30003) << "Loading QPicture has failed! (KoPictureClipart::load)" << endl;
            check = false;
        }
    }
    buffer.close();
    return check;
}

bool KoPictureClipart::save(QIODevice* io)
{
    // We save the raw data, as the SVG supposrt in QPicture is poor
    Q_ULONG size=io->writeBlock(m_rawData); // WARNING: writeBlock returns Q_LONG but size() Q_ULONG!
    return (size==m_rawData.size());
}

bool KoPictureClipart::loadQPicture(QPicture& picture)
{
    m_clipart=picture;
    return true;
}

void KoPictureClipart::setRawData(const QByteArray& newRawData)
{
    m_rawData=newRawData;
}

QSize KoPictureClipart::getOriginalSize(void) const
{
    return m_clipart.boundingRect().size();
}

QPixmap KoPictureClipart::generatePixmap(const QSize& size)
{
    // Not sure if it works, but it worked for KoPictureFilePreviewWidget::setClipart
    QPixmap pixmap(size);
    QPainter p;

    p.begin( &pixmap );
    p.setBackgroundColor( Qt::white );
    pixmap.fill( Qt::white );

    QRect br = m_clipart.boundingRect();
    if ( br.width() && br.height() )
        p.scale( (double)pixmap.width() / (double)br.width(), (double)pixmap.height() / (double)br.height() );
    p.drawPicture( m_clipart );
    p.end();
    return pixmap;
}
