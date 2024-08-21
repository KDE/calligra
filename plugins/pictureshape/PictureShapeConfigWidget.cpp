/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PictureShapeConfigWidget.h"
#include "PictureDebug.h"
#include "PictureShape.h"

#include <KoImageCollection.h>
#include <KoImageData.h>

#include <KIO/StoredTransferJob>
#include <kfilewidget.h>
#include <kjobuidelegate.h>

#include <QGridLayout>
#include <QImageReader>
#include <QUrl>

void PictureShapeLoadWaiter::setImageData(KJob *job)
{
    if (job->error()) { // e.g. file not found
        job->uiDelegate()->showErrorMessage();
        if (m_pictureShape && !m_pictureShape->imageData()) {
            // Don't leave an empty broken shape, the rest of the code isn't ready for null imageData
            if (m_pictureShape->parent()) {
                m_pictureShape->parent()->removeShape(m_pictureShape);
            }
            delete m_pictureShape;
        }
        deleteLater();
        return;
    }

    deleteLater();

    if (m_pictureShape == nullptr)
        return; // ugh, the shape got deleted meanwhile (## err, who would set the pointer to null?)

    KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob *>(job);
    Q_ASSERT(transferJob);

    if (m_pictureShape->imageCollection()) {
        KoImageData *data = m_pictureShape->imageCollection()->createImageData(transferJob->data());
        if (data) {
            m_pictureShape->setUserData(data);
            // check if the shape still size of the default shape and resize in that case
            if (qFuzzyCompare(m_pictureShape->size().width(), 50.0)) {
                m_pictureShape->setSize(data->imageSize());
            }
            // trigger repaint as the userData was changed
            m_pictureShape->update();
        }
    }
}

// ---------------------------------------------------- //

PictureShapeConfigWidget::PictureShapeConfigWidget()
    : m_shape(nullptr)
    , m_fileWidget(nullptr)
{
}

PictureShapeConfigWidget::~PictureShapeConfigWidget()
{
    delete m_fileWidget;
}

void PictureShapeConfigWidget::open(KoShape *shape)
{
    m_shape = dynamic_cast<PictureShape *>(shape);
    Q_ASSERT(m_shape);
    delete m_fileWidget;
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_fileWidget = new KFileWidget(QUrl(/* QT5TODO:"kfiledialog:///OpenDialog"*/), this);
    m_fileWidget->setMode(KFile::Files | KFile::ExistingOnly);
    m_fileWidget->setOperationMode(KFileWidget::Opening);
    QList<KFileFilter> imageFilters;
    for (const QByteArray &mimeType : QImageReader::supportedMimeTypes()) {
        imageFilters.append(KFileFilter::fromMimeType(QLatin1String(mimeType)));
    }
    m_fileWidget->setFilters(imageFilters);
    layout->addWidget(m_fileWidget);
    setLayout(layout);
    connect(m_fileWidget, &KFileWidget::accepted, this, &PictureShapeConfigWidget::slotAccept);
}

// The page dialog's own accept() is called by the OK button
// This makes exec() return, then we get here.
// For KFileWidget, nothing happened yet. It still needs to process things in slotOk.
void PictureShapeConfigWidget::save()
{
    if (!m_shape)
        return;
    m_fileWidget->slotOk(); // emits accepted, possibly async
}

// Called by slotOk, possibly async
void PictureShapeConfigWidget::slotAccept()
{
    m_fileWidget->accept();
    const QUrl url = m_fileWidget->selectedUrl();
    if (!url.isEmpty()) {
        KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, {});
        PictureShapeLoadWaiter *waiter = new PictureShapeLoadWaiter(m_shape);
        connect(job, &KJob::result, waiter, &PictureShapeLoadWaiter::setImageData);
    }
    Q_EMIT accept();
}

bool PictureShapeConfigWidget::showOnShapeCreate()
{
    return true;
}

bool PictureShapeConfigWidget::showOnShapeSelect()
{
    return false;
}
