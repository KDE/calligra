/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPicturesImport.h"

#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoPAMasterPage.h>
#include <KoPAPage.h>
#include <KoPAPageInsertCommand.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLayer.h>
#include <KoShapeRegistry.h>

#include "KPrDocument.h"
#include "KPrView.h"
#include "StageDebug.h"

#include <KIO/StoredTransferJob>
#include <KoDocumentResourceManager.h>
#include <kundo2command.h>

#include <QFileDialog>
#include <QImageReader>
#include <QUrl>

KPrPicturesImport::KPrPicturesImport() = default;

void KPrPicturesImport::import(KPrView *view)
{
    m_factory = KoShapeRegistry::instance()->value("PictureShape");
    Q_ASSERT(m_factory);
    if (m_factory) {
        // TODO: think about using KoFileDialog everywhere, after extending it to support remote urls
        QFileDialog *dialog = new QFileDialog();
        QStringList imageMimeTypes;
        foreach (const QByteArray &mimeType, QImageReader::supportedMimeTypes()) {
            imageMimeTypes << QLatin1String(mimeType);
        }
        dialog->setMimeTypeFilters(imageMimeTypes);
        dialog->setFileMode(QFileDialog::ExistingFiles);
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
        if (dialog->exec() != QFileDialog::Accepted) {
            return;
        }
        m_urls = dialog->selectedUrls();

        // TODO there should be a progress bar
        // instead of the progress bar opening for each loaded picture
        m_currentPage = view->activePage();
        KoPAPage *activePage = dynamic_cast<KoPAPage *>(m_currentPage);
        if (activePage) {
            m_masterPage = activePage->masterPage();

            m_doc = view->kprDocument();
            m_cmd = new KUndo2Command(kundo2_i18n("Insert Pictures"));
            import();
        }
    } else {
        warnStage << "picture shape factory not found";
    }
}

void KPrPicturesImport::import()
{
    if (m_urls.isEmpty()) {
        //  TODO check that a picture was added.
        m_doc->addCommand(m_cmd);
        // TODO activate first added page doUpdateActivePage(page);
    } else {
        QUrl url(m_urls.takeAt(0));
        // todo calculate the correct size so that the image is centered to
        KIO::StoredTransferJob *job(KIO::storedGet(url, KIO::NoReload, {}));
        connect(job, &KJob::result, this, &KPrPicturesImport::pictureImported);
        job->exec();
    }
}

void KPrPicturesImport::pictureImported(KJob *job)
{
    KoShape *shape = m_factory->createDefaultShape();
    if (shape) {
        KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob *>(job);
        Q_ASSERT(transferJob);
        KoImageData *imageData = m_doc->resourceManager()->imageCollection()->createImageData(transferJob->data());
        if (imageData->isValid()) {
            shape->setUserData(imageData);

            // make sure the picture fits on the page
            QSizeF imageSize = imageData->imageSize();
            QSizeF pageSize = m_masterPage->size();
            qreal zoom = 1;
            if (imageSize.width() > pageSize.width() || imageSize.height() > pageSize.height()) {
                zoom = pageSize.width() / imageSize.width();
                zoom = qMin(zoom, pageSize.height() / imageSize.height());
            }
            imageSize *= zoom;
            shape->setSize(imageSize);

            // center the picture on the page
            QPointF pos(pageSize.width() / 2 - imageSize.width() / 2, pageSize.height() / 2 - imageSize.height() / 2);
            shape->setPosition(pos);

            KoPAPageBase *page = m_doc->newPage(m_masterPage);
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(page->shapes().first());
            if (layer) {
                layer->addShape(shape);
                new KoPAPageInsertCommand(m_doc, page, m_currentPage, m_cmd);
                m_currentPage = page;
            } else {
                delete page;
                delete shape;
            }
        } else {
            warnStage << "imageData not valid";
            delete shape;
        }
    } else {
        warnStage << "shape not created";
    }
    import();
}
