/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrNotes.h"

#include <QtGui/QPainter>

#include <KDebug>

#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoPAMasterPage.h>
#include <KoShape.h>
#include <KoShapeFactory.h>
#include <KoShapePainter.h>
#include <KoShapeRegistry.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

#include "KPrPage.h"

KPrNotes::KPrNotes(KPrPage *page)
    : KoShapeContainer()
    , m_page( page )
{
    // All sizes and positions are hardcoded for now
    KoShapeFactory *factory = KoShapeRegistry::instance()->value("TextShapeID");
    Q_ASSERT(factory);
    m_textShape = factory->createDefaultShape(0);
    m_textShape->setLocked(true);
    m_textShape->setAddtionalAttribute( "presentation:class", "notes" );
    m_pageLayout = KoPageLayout::standardLayout();
    m_textShape->setPosition(QPointF(62.22, 374.46));
    m_textShape->setSize(QSizeF(489.57, 356.37));

    factory = KoShapeRegistry::instance()->value("PictureShape");
    Q_ASSERT(factory);
    m_thumbnailShape = factory->createDefaultShape(0);
    m_thumbnailShape->setLocked(true);
    m_thumbnailShape->setAddtionalAttribute( "presentation:class", "page" );
    m_thumbnailShape->setPosition(QPointF(108.00, 60.18));
    m_thumbnailShape->setSize(QSizeF(396.28, 296.96));

    addChild(m_textShape);
    addChild(m_thumbnailShape);
}

KPrNotes::~KPrNotes()
{
}

KoShape *KPrNotes::textShape()
{
    return m_textShape;
}

KoShape *KPrNotes::thumbnailShape()
{
    KoImageData *imageData = new KoImageData(new KoImageCollection());
    imageData->setImage(createPageThumbnail());
    m_thumbnailShape->setUserData(imageData);
    return m_thumbnailShape;
}

void KPrNotes::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter *writer = &context.xmlWriter();
    writer->startElement("presentation:notes");

    context.addOption( KoShapeSavingContext::PresentationShape );
    m_textShape->saveOdf(context);
    context.removeOption( KoShapeSavingContext::PresentationShape );
    writer->startElement("draw:page-thumbnail");
    m_thumbnailShape->saveOdfFrameAttributes(context);
    writer->addAttribute("draw:page-number", static_cast<KoPASavingContext &>(context).page());
    writer->endElement();

    writer->endElement();
}

bool KPrNotes::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    // TODO
    return true;
}

void KPrNotes::paintComponent(QPainter& painter, const KoViewConverter& converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

KoShape *KPrNotes::cloneShape() const
{
    return 0;
}

QImage KPrNotes::createPageThumbnail() const
{
    // TODO: use createPageThumbnail from KoPageApp
    QSize size = m_thumbnailShape->size().toSize();
    KoShapePainter shapePainter;

    QList<KoShape*> shapes;
    double zoom = (double) size.height() / m_page->pageLayout().width;

    shapes = m_page->iterator();
    shapes += m_page->masterPage()->iterator();
    shapePainter.setShapes(shapes);

    QImage image(size, QImage::Format_RGB32);
    image.fill(QColor(Qt::white).rgb());
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipRect(QRect(0, 0, size.width(), size.height()));
    KoZoomHandler zoomHandler;
    zoomHandler.setZoom(zoom);
    shapePainter.paintShapes(painter, zoomHandler);

    return image;
}

