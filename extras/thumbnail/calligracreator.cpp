/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2002 Simon MacMullen <calligra@babysimon.co.uk>
    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "calligracreator.h"

// Calligra
#include <KoDocument.h>
#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KoStore.h>

// Qt
#include <QMimeDatabase>
#include <QMimeType>
#include <QPainter>
#include <QTimer>
#include <KPluginFactory>

static const int minThumbnailSize = 400;
static const int timeoutTime = 5000; // in msec

#if TARGETTYPES == DOCS
K_PLUGIN_CLASS_WITH_JSON(CalligraCreator, "ooxml_odf_thumbnail.json")
#elif TARGETTYPES == IMAGES
K_PLUGIN_CLASS_WITH_JSON(CalligraCreator, "images_thumbnail.json")
#endif

CalligraCreator::CalligraCreator(QObject *parent, const QVariantList &args)
    : KIO::ThumbnailCreator(parent, args)
    , m_part(nullptr)
    , m_doc(nullptr)
{
}

CalligraCreator::~CalligraCreator()
{
    delete m_doc;
}

KIO::ThumbnailResult CalligraCreator::create(const KIO::ThumbnailRequest &request)
{
    QImage image;
    // try to use any embedded thumbnail
    auto store = std::unique_ptr<KoStore>(KoStore::createStore(request.url().toLocalFile(), KoStore::Read));
    if (store && store->isEncrypted()) {
        // Trying to open an encrypted file will trigger a password dialog
        return KIO::ThumbnailResult::fail();
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
        if (thumbnail.loadFromData(thumbnailData) && thumbnail.width() >= request.targetSize().width() && thumbnail.height() >= request.targetSize().height()) {
            // put a white background behind the thumbnail
            // as lots of old(?) OOo files have thumbnails with transparent background
            image = QImage(thumbnail.size(), QImage::Format_RGB32);
            image.fill(QColor(Qt::white).rgb());
            QPainter p(&image);
            p.drawImage(QPoint(0, 0), thumbnail);
            return KIO::ThumbnailResult::pass(image);
        }
    }

    // load document and render the thumbnail ourselves
    const QString mimetype = request.mimeType();
    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    m_part = documentEntry.createKoPart(&error);

    if (!m_part)
        return KIO::ThumbnailResult::fail();

    m_doc = m_part->document();

    // prepare the document object
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(false); // don't show message boxes
    connect(m_doc, &KoDocument::completed, this, &CalligraCreator::onLoadingCompleted);

    // load the document content
    m_loadingCompleted = false;

    if (!m_doc->openUrl(request.url())) {
        delete m_doc;
        m_doc = nullptr;
        return KIO::ThumbnailResult::fail();
    }

    if (!m_loadingCompleted) {
        // loading is done async, so wait here for a while
        // Using a QEventLoop here seems fine, thumbnailers are only used inside the
        // thumbnail protocol slave, it seems
        QTimer::singleShot(timeoutTime, &m_eventLoop, &QEventLoop::quit);
        m_eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

    if (m_loadingCompleted) {
        // render the page on a bigger pixmap and use smoothScale,
        // looks better than directly scaling with the QPainter (malte)
        const bool usePassedSize = (request.targetSize().width() > minThumbnailSize && request.targetSize().height() > minThumbnailSize);
        const QSize size = usePassedSize ? QSize(request.targetSize().width(), request.targetSize().height()) : QSize(minThumbnailSize, minThumbnailSize);
        image = m_doc->generatePreview(size).toImage();
    }

    m_doc->closeUrl();
    delete m_doc;
    m_doc = nullptr;

    if (m_loadingCompleted) {
        return KIO::ThumbnailResult::pass(image);
    }
    return KIO::ThumbnailResult::fail();
}

void CalligraCreator::onLoadingCompleted()
{
    m_loadingCompleted = true;
    m_eventLoop.quit();
}

#include "calligracreator.moc"
