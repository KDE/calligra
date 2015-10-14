/* This file is part of the KDE project
   Copyright (C) 2010 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrPicturesImport.h"

#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoShapeLayer.h>
#include <KoImageData.h>
#include <KoImageCollection.h>
#include <KoPAPage.h>
#include <KoPAMasterPage.h>
#include <KoPAPageInsertCommand.h>

#include "KPrDocument.h"
#include "KPrView.h"
#include "StageDebug.h"

#include <kio/job.h>
#include <kundo2command.h>
#include <KoDocumentResourceManager.h>

#include <QFileDialog>
#include <QUrl>
#include <QImageReader>


KPrPicturesImport::KPrPicturesImport()
{
}

void KPrPicturesImport::import(KPrView *view)
{
    m_factory = KoShapeRegistry::instance()->value("PictureShape");
    Q_ASSERT(m_factory);
    if (m_factory) {
        // TODO: think about using KoFileDialog everywhere, after extending it to support remote urls
        QFileDialog *dialog = new QFileDialog();
        QStringList imageMimeTypes;
        foreach(const QByteArray &mimeType, QImageReader::supportedMimeTypes()) {
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
        KoPAPage *activePage = dynamic_cast<KoPAPage*>(m_currentPage);
        if (activePage) {
            m_masterPage = activePage->masterPage();

            m_doc = view->kprDocument();
            m_cmd = new KUndo2Command(kundo2_i18n("Insert Pictures"));
            import();
        }
    }
    else {
        warnStage << "picture shape factory not found";
    }
}

void KPrPicturesImport::import()
{
    if (m_urls.isEmpty()) {
        //  TODO check that a picture was added.
        m_doc->addCommand(m_cmd);
        // TODO activate first added page doUpdateActivePage(page);
    }
    else {
        QUrl url(m_urls.takeAt(0));
        // todo calculate the correct size so that the image is centered to
        KIO::StoredTransferJob *job(KIO::storedGet(url, KIO::NoReload, 0));
        connect(job, SIGNAL(result(KJob*)), this, SLOT(pictureImported(KJob*)));
        job->exec();
    }
}

void KPrPicturesImport::pictureImported(KJob *job)
{
    KoShape *shape = m_factory->createDefaultShape();
    if (shape) {
        KIO::StoredTransferJob *transferJob = qobject_cast<KIO::StoredTransferJob*>(job);
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
            QPointF pos( pageSize.width() / 2- imageSize.width() / 2, pageSize.height() / 2 - imageSize.height() / 2 );
            shape->setPosition(pos);

            KoPAPageBase *page = m_doc->newPage(m_masterPage);
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(page->shapes().first());
            if (layer) {
                layer->addShape(shape);
                new KoPAPageInsertCommand(m_doc, page, m_currentPage, m_cmd);
                m_currentPage = page;
            }
            else {
                delete page;
                delete shape;
            }
        }
        else {
            warnStage << "imageData not valid";
            delete shape;
        }
    }
    else {
        warnStage << "shape not created";
    }
    import();
}
