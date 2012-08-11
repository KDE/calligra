/*  This file is part of the KDE libraries
    Copyright (C) 2002 Simon MacMullen <calligra@babysimon.co.uk>
    Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
#include "calligracreator.h"

// KDE
#include <KoPart.h>
#include <KoStore.h>
#include <KoDocument.h>
#include <KMimeTypeTrader>
#include <KMimeType>
// Qt
#include <QPainter>
#include <QTimer>

static const int minThumbnailSize = 400;
static const int timeoutTime = 5000; // in msec

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
    {
        return new CalligraCreator;
    }
}

CalligraCreator::CalligraCreator()
    : m_part(0)
    , m_doc(0)
{
}

CalligraCreator::~CalligraCreator()
{
    delete m_part;
    delete m_doc;
}

bool CalligraCreator::create(const QString &path, int width, int height, QImage &image)
{
    // try to use any embedded thumbnail
    KoStore *store = KoStore::createStore(path, KoStore::Read);

    if (store &&
        (store->open(QLatin1String("Thumbnails/thumbnail.png")) ||
         store->open(QLatin1String("preview.png")))) {
        // Hooray! No long delay for the user...
        const QByteArray thumbnailData = store->read(store->size());

        QImage thumbnail;
        if (thumbnail.loadFromData(thumbnailData) &&
            thumbnail.width() >= width && thumbnail.height() >= height) {
            // put a white background behind the thumbnail
            // as lots of old(?) OOo files have thumbnails with transparent background
            image = QImage(thumbnail.size(), QImage::Format_RGB32);
            image.fill(QColor(Qt::white).rgb());
            QPainter p(&image);
            p.drawImage(QPoint(0, 0), thumbnail);
            delete store;
            return true;
        }
    }
    delete store;

    // load document and render the thumbnail ourselves
    const QString mimetype = KMimeType::findByPath(path)->name();
    m_part = KMimeTypeTrader::self()->createInstanceFromQuery<KoPart>(mimetype, QLatin1String("CalligraPart"));

    if (!m_part) return false;

    m_doc = m_part->document();

    // prepare the document object
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(false); // don't show message boxes
    connect(m_part, SIGNAL(completed()), SLOT(onLoadingCompleted()));

    // load the document content
    m_loadingCompleted = false;

    KUrl url;
    url.setPath(path);
    if (!m_doc->openUrl(url)) {
        delete m_doc;
        return false;
    }

    if (! m_loadingCompleted) {
        // loading is done async, so wait here for a while
        // Using a QEventLoop here seems fine, thumbnailers are only used inside the
        // thumbnail protocol slave, it seems
        QTimer::singleShot(timeoutTime, &m_eventLoop, SLOT(quit()));
        m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    if (m_loadingCompleted) {
        // render the page on a bigger pixmap and use smoothScale,
        // looks better than directly scaling with the QPainter (malte)
        const bool usePassedSize = (width > minThumbnailSize && height > minThumbnailSize);
        const QSize size = usePassedSize ? QSize(width, height) : QSize(minThumbnailSize, minThumbnailSize);
        image = m_doc->generatePreview(size).toImage();
    }

    m_part->closeUrl();

    return m_loadingCompleted;
}

void CalligraCreator::onLoadingCompleted()
{
    m_loadingCompleted = true;
    m_eventLoop.quit();
}

ThumbCreator::Flags CalligraCreator::flags() const
{
#ifdef NO_ICON_BLENDING
    return DrawFrame;
#else
    return (Flags)(DrawFrame | BlendIcon);
#endif
}
