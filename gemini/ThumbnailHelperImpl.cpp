/*
 * Assists in creating thumbnails for Gemini's file views
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "ThumbnailHelperImpl.h"
#include <QMimeDatabase>

#include <KoDocument.h>
#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KoStore.h>

#include <QTimer>
#include <QImage>
#include <QApplication>
#include <QPainter>
#include <QMimeType>

static const int minThumbnailSize = 400;
static const int timeoutTime = 5000; // in msec

ThumbnailHelperImpl::ThumbnailHelperImpl(QObject* parent)
    : QObject(parent)
    , m_part(0)
    , m_doc(0)
{
}

ThumbnailHelperImpl::~ThumbnailHelperImpl()
{
    if(m_doc)
        m_doc->deleteLater();
}

bool ThumbnailHelperImpl::convert(const QString& in, const QString& out, int width, int height)
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
    connect(m_doc, SIGNAL(completed()), SLOT(onLoadingCompleted()));

    // load the document content
    QUrl url = QUrl::fromLocalFile(in);
    if (!m_doc->openUrl(url)) {
        delete m_doc;
        qDebug() << "Load failure! Document did not open" << url;
        return false;
    }

    while(m_doc->isLoading()) {
        qApp->processEvents();
    }

    // render the page on a bigger pixmap and use smoothScale,
    // looks better than directly scaling with the QPainter (malte)
    //const bool usePassedSize = (width > minThumbnailSize && height > minThumbnailSize);
    //const QSize size = usePassedSize ? QSize(width, height) : QSize(minThumbnailSize, minThumbnailSize);
    const QSize size = QSize(width, height);
    image = m_doc->generatePreview(size * 2).toImage().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_doc->closeUrl();

    image.save(out);

    return m_loadingCompleted;
}
