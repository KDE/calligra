/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Kaushal M <kshlmster@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "FoImageSelectionWidget.h"

#include <KoShape.h>
#include <KoDocumentResourceManager.h>
#include <KoImageData.h>
#include <KoImageCollection.h>
#include <klocale.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>

#include <QFileDialog>
#include <QDesktopServices>
#include <QFile>

KoShape *FoImageSelectionWidget::selectImageShape(KoDocumentResourceManager *documentResources, QWidget *parent)
{
    QString imageFilename = QFileDialog::getOpenFileName(parent,i18n("Select Image"), QDesktopServices::storageLocation(QDesktopServices::PicturesLocation), "Images (*.jpeg *.jpg *.png *.gif)");
    if(imageFilename.isEmpty())
        return 0;
    QFile imageFile(imageFilename);
    imageFile.open(QFile::ReadOnly);
    QByteArray imageData = imageFile.readAll();
    imageFile.close();
    KoImageData *koImageData = documentResources->imageCollection()->createImageData(imageData);
    if(!koImageData->isValid())
        return 0;
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("PictureShape");
    KoShape *shape = factory->createDefaultShape(documentResources);
    shape->setUserData(koImageData);
    shape->setSize(koImageData->imageSize());
    return shape;
}
