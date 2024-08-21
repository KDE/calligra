/*
 * Assists in creating thumbnails for Gemini's file views
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "ThumbnailHelperImpl.h"
#include <QMimeDatabase>

#include <KoDocument.h>
#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KoStore.h>

#include <QApplication>
#include <QImage>
#include <QMimeType>
#include <QPainter>
#include <QTimer>

// static const int minThumbnailSize = 400;

ThumbnailHelperImpl::ThumbnailHelperImpl(QObject *parent)
    : QObject(parent)
    , m_part(nullptr)
    , m_doc(nullptr)
{
}

ThumbnailHelperImpl::~ThumbnailHelperImpl()
{
    if (m_doc)
        m_doc->deleteLater();
}

bool ThumbnailHelperImpl::convert(const QString &in, const QString &out, int width, int height)
{
    // Other locations for thumbnails use the embedded thumbnails. This
    // application specifically exists to ensure less crashes in situations
    // where that embedded image doesn't exist or is invalid for whatever
    // reason. As a direct consequence, we do not attempt to load that image.
    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);

    QMimeDatabase db;
    const QString mimetype = db.mimeTypeForFile(in).name();
    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    m_part = documentEntry.createKoPart(&error);

    if (!m_part)
        return false;

    m_doc = m_part->document();

    // prepare the document object
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(false); // don't show message boxes

    // load the document content
    QUrl url = QUrl::fromLocalFile(in);
    if (!m_doc->openUrl(url)) {
        delete m_doc;
        qDebug() << "Load failure! Document did not open" << url;
        return false;
    }

    while (m_doc->isLoading()) {
        qApp->processEvents();
    }

    // render the page on a bigger pixmap and use smoothScale,
    // looks better than directly scaling with the QPainter (malte)
    // const bool usePassedSize = (width > minThumbnailSize && height > minThumbnailSize);
    // const QSize size = usePassedSize ? QSize(width, height) : QSize(minThumbnailSize, minThumbnailSize);
    const QSize size = QSize(width, height);
    image = m_doc->generatePreview(size * 2).toImage().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_doc->closeUrl();

    image.save(out);

    return m_loadingCompleted;
}
