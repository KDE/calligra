/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2002 Simon MacMullen <calligra@babysimon.co.uk>
    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "calligracreator.h"

// Calligra
#include <KoPart.h>
#include <KoStore.h>
#include <KoDocument.h>
#include <KoDocumentEntry.h>

// Qt
#include <QPainter>
#include <QTimer>
#include <QMimeDatabase>
#include <QMimeType>

static const int minThumbnailSize = 400;
static const int timeoutTime = 5000; // in msec

extern "C"
{
    Q_DECL_EXPORT ThumbCreator *new_creator()
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
    delete m_doc;
}

bool CalligraCreator::create(const QString &path, int width, int height, QImage &image)
{
    // try to use any embedded thumbnail
    KoStore *store = KoStore::createStore(path, KoStore::Read);
    if (store && store->isEncrypted()) {
        // Trying to open an encrypted file will trigger a password dialog
        delete store;
        return false;
    }
    if (store &&
         // ODF thumbnail?
        (store->open(QLatin1String("Thumbnails/thumbnail.png")) ||
         // old KOffice/Calligra thumbnail?
         store->open(QLatin1String("preview.png")) ||
         // OOXML?
         store->open(QLatin1String("docProps/thumbnail.jpeg")))) {
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
    const QString mimetype = QMimeDatabase().mimeTypeForFile(path).name();
    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    m_part = documentEntry.createKoPart(&error);


    if (!m_part) return false;

    m_doc = m_part->document();

    // prepare the document object
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(false); // don't show message boxes
    connect(m_doc, &KoDocument::completed, this, &CalligraCreator::onLoadingCompleted);

    // load the document content
    m_loadingCompleted = false;

    const QUrl url = QUrl::fromLocalFile(path);
    if (!m_doc->openUrl(url)) {
        delete m_doc;
        m_doc = 0;
        return false;
    }

    if (! m_loadingCompleted) {
        // loading is done async, so wait here for a while
        // Using a QEventLoop here seems fine, thumbnailers are only used inside the
        // thumbnail protocol slave, it seems
        QTimer::singleShot(timeoutTime, &m_eventLoop, &QEventLoop::quit);
        m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    if (m_loadingCompleted) {
        // render the page on a bigger pixmap and use smoothScale,
        // looks better than directly scaling with the QPainter (malte)
        const bool usePassedSize = (width > minThumbnailSize && height > minThumbnailSize);
        const QSize size = usePassedSize ? QSize(width, height) : QSize(minThumbnailSize, minThumbnailSize);
        image = m_doc->generatePreview(size).toImage();
    }

    m_doc->closeUrl();
    delete m_doc;
    m_doc = 0;

    return m_loadingCompleted;
}

void CalligraCreator::onLoadingCompleted()
{
    m_loadingCompleted = true;
    m_eventLoop.quit();
}
