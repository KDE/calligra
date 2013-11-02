/* This file is part of the Calligra project, made with-in the KDE community

   Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
   Copyright (C) 2013 Friedrich W. H. Kossebau <kossebau@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoContactSemanticItemFactory.h"
#if 0
#include "PictureShape.h"
#include "PictureShapeConfigWidget.h"

#include <QByteArray>
#include <QBuffer>
#include <QImage>

#include <KoXmlNS.h>
#include "KoShapeBasedDocumentBase.h"
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoDocumentResourceManager.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoProperties.h>
#include <KoIcon.h>
#include <klocale.h>
#include <kdebug.h>
#endif

KoContactSemanticItemFactory::KoContactSemanticItemFactory()
    : KoRdfSemanticItemFactoryBase("Contact")//PICTURESHAPEID, i18n("Image"))
{
#if 0
    setToolTip(i18n("Image shape that can display jpg, png etc."));
    setIconName(koIconNameCStr("x-shape-image"));
    setLoadingPriority(1);

    QList<QPair<QString, QStringList> > elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::draw), QStringList("image")));
    elementNamesList.append(qMakePair(QString(KoXmlNS::svg), QStringList("image")));
    setXmlElements(elementNamesList);
#endif
}
#if 0
KoShape *KoContactSemanticItemFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    PictureShape * defaultShape = new PictureShape();
    defaultShape->setShapeId(PICTURESHAPEID);
    if (documentResources) {
        defaultShape->setImageCollection(documentResources->imageCollection());
    }
    return defaultShape;
}

KoShape *KoContactSemanticItemFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    PictureShape *shape = static_cast<PictureShape*>(createDefaultShape(documentResources));
    if (params->contains("qimage")) {
        QImage image = params->property("qimage").value<QImage>();
        Q_ASSERT(!image.isNull());

        if (shape->imageCollection()) {
            KoImageData *data = shape->imageCollection()->createImageData(image);
            shape->setUserData(data);
            shape->setSize(data->imageSize());
            shape->update();
        }
    }
    return shape;

}

bool KoContactSemanticItemFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    if (e.localName() == "image" && e.namespaceURI() == KoXmlNS::draw) {
        QString href = e.attribute("href");
        if (!href.isEmpty()) {
            // check the mimetype
            if (href.startsWith(QLatin1String("./"))) {
                href.remove(0, 2);
            }
            QString mimetype = context.odfLoadingContext().mimeTypeForPath(href);
            if (!mimetype.isEmpty()) {
                return mimetype.startsWith("image");
            }
            else {
                return ( href.endsWith("bmp") ||
                         href.endsWith("jpg") ||
                         href.endsWith("gif") ||
                         href.endsWith("eps") ||
                         href.endsWith("png") ||
                         href.endsWith("tif") ||
                         href.endsWith("tiff"));
            }
        }
        else {
            return !KoXml::namedItemNS(e, KoXmlNS::office, "binary-data").isNull();
        }
    }
    return false;
}

QList<KoShapeConfigWidgetBase*> KoContactSemanticItemFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*> panels;
    panels.append( new PictureShapeConfigWidget() );
    return panels;
}

void KoContactSemanticItemFactory::newDocumentResourceManager(KoDocumentResourceManager *manager) const
{
    if (!manager->imageCollection())
        manager->setImageCollection(new KoImageCollection(manager));
}
#endif
