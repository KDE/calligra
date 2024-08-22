/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "CSThumbProviderKarbon.h"

#include <KarbonDocument.h>
#include <KarbonPart.h>

#include <QApplication>
#include <QEventLoop>
#include <QPainter>
#include <QPixmap>

void processEvents()
{
    int i = 100;
    // while (QCoreApplication::eventDispatcher()->hasPendingEvents() && i > 0) {
    //     --i;
    //     QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    // }
}

CSThumbProviderKarbon::CSThumbProviderKarbon(KarbonDocument *doc)
    : m_doc(doc)
{
}

CSThumbProviderKarbon::~CSThumbProviderKarbon() = default;

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
