/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "CSThumbProviderKarbon.h"

#include <KarbonPart.h>
#include <KarbonDocument.h>

#include <QApplication>
#include <QEventLoop>
#include <QPixmap>
#include <QPainter>

void processEvents()
{
    int i = 100;
    while (QCoreApplication::hasPendingEvents() && i > 0) {
        --i;
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

CSThumbProviderKarbon::CSThumbProviderKarbon(KarbonDocument *doc)
: m_doc(doc)
{
}

CSThumbProviderKarbon::~CSThumbProviderKarbon()
{
}

QVector<QImage> CSThumbProviderKarbon::createThumbnails(const QSize &thumbSize)
{
    // make sure everything is rendered before painting
    processEvents();

    QImage thumbnail(thumbSize, QImage::Format_RGB32);
    thumbnail.fill(QColor(Qt::white).rgb());
    QPainter thumbPainter(&thumbnail);
    m_doc->paintContent(thumbPainter, QRect(QPoint(0, 0), thumbSize));

    // make sure there are no events; this fixes a crash on shutdown
    processEvents();

    return QVector<QImage>() << thumbnail;
}
