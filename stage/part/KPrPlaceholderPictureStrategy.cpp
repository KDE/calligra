/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPlaceholderPictureStrategy.h"

#include <QString>
#include <QFileDialog>
#include <QUrl>

#include <KoImageCollection.h>
#include <KoDocumentResourceManager.h>
#include <KoImageData.h>
#include <KoShape.h>
#include <KoNetAccess.h>

#include "StageDebug.h"


KPrPlaceholderPictureStrategy::KPrPlaceholderPictureStrategy()
: KPrPlaceholderStrategy( "graphic" )
{
}

KPrPlaceholderPictureStrategy::~KPrPlaceholderPictureStrategy()
{
}

KoShape *KPrPlaceholderPictureStrategy::createShape(KoDocumentResourceManager *rm)
{
    KoShape * shape = 0;

    QUrl url = QFileDialog::getOpenFileUrl();
    if ( !url.isEmpty() ) {
        shape = KPrPlaceholderStrategy::createShape(rm);

        KoImageCollection *collection = rm->imageCollection();
        Q_ASSERT(collection);

        QString tmpFile;
        if (KIO::NetAccess::download(url, tmpFile, 0)) {
            QImage image(tmpFile);
            if (!image.isNull()) {
                //setSuffix(url.prettyUrl());
                KoImageData *data = collection->createImageData(image);
                if (data->isValid()) {
                    shape->setUserData( data );
                    // TODO the pic should be fit into the space provided
                    shape->setSize( data->imageSize() );
                }
            }
            KIO::NetAccess::removeTempFile(tmpFile);
        } else {
            warnStage << "open image" << url << "failed";
        }
    }
    return shape;
}
