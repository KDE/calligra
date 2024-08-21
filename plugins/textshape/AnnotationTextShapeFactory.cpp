/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AnnotationTextShapeFactory.h"
#include "AnnotationTextShape.h"

#include <KoDocumentResourceManager.h>
#include <KoImageCollection.h>
#include <KoInlineTextObjectManager.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeLoadingContext.h>
#include <KoStyleManager.h>
#include <KoTextDocument.h>
#include <KoTextRangeManager.h>
#include <KoTextShapeData.h>
#include <KoXmlNS.h>
#include <changetracker/KoChangeTracker.h>

#include <KoIcon.h>

#include <KLocalizedString>
#include <QDebug>
#include <QTextCursor>
#include <kundo2stack.h>

AnnotationTextShapeFactory::AnnotationTextShapeFactory()
    : KoShapeFactoryBase(AnnotationShape_SHAPEID, i18n("Annotation"))
{
    setToolTip(i18n("Annotation shape to show annotation content"));
    QList<QPair<QString, QStringList>> odfElements;
    odfElements.append(QPair<QString, QStringList>(KoXmlNS::office, QStringList("annotation")));
    setXmlElements(odfElements);

    KoShapeTemplate t;
    t.name = i18n("Annotation");
    t.iconName = koIconName("x-shape-text"); // Any icon for now :)
    t.toolTip = i18n("Annotation Shape");
    KoProperties *props = new KoProperties();
    t.properties = props;
    props->setProperty("demo", true);
    addTemplate(t);

    setHidden(true);
}

KoShape *AnnotationTextShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    KoInlineTextObjectManager *manager = nullptr;
    KoTextRangeManager *locationManager = nullptr;
    if (documentResources && documentResources->hasResource(KoText::InlineTextObjectManager)) {
        QVariant variant = documentResources->resource(KoText::InlineTextObjectManager);
        if (variant.isValid()) {
            manager = variant.value<KoInlineTextObjectManager *>();
        }
    }
    if (documentResources && documentResources->hasResource(KoText::TextRangeManager)) {
        QVariant variant = documentResources->resource(KoText::TextRangeManager);
        if (variant.isValid()) {
            locationManager = variant.value<KoTextRangeManager *>();
        }
    }
    if (!manager) {
        manager = new KoInlineTextObjectManager();
    }
    if (!locationManager) {
        locationManager = new KoTextRangeManager();
    }
    AnnotationTextShape *annotation = new AnnotationTextShape(manager, locationManager);
    if (documentResources) {
        KoTextDocument document(annotation->textShapeData()->document());

        if (documentResources->hasResource(KoText::StyleManager)) {
            KoStyleManager *styleManager = documentResources->resource(KoText::StyleManager).value<KoStyleManager *>();
            document.setStyleManager(styleManager);
        }

        // this is needed so the shape can reinitialize itself with the stylemanager
        annotation->textShapeData()->setDocument(annotation->textShapeData()->document(), true);

        document.setUndoStack(documentResources->undoStack());

        if (documentResources->hasResource(KoText::PageProvider)) {
            KoPageProvider *pp = static_cast<KoPageProvider *>(documentResources->resource(KoText::PageProvider).value<void *>());
            annotation->setPageProvider(pp);
        }
        if (documentResources->hasResource(KoText::ChangeTracker)) {
            KoChangeTracker *changeTracker = documentResources->resource(KoText::ChangeTracker).value<KoChangeTracker *>();
            document.setChangeTracker(changeTracker);
        }

        document.setShapeController(documentResources->shapeController());

        // update the resources of the document
        annotation->updateDocumentData();
        annotation->setImageCollection(documentResources->imageCollection());
    }

    // Should set if we don't set id it will set to TextShapeID.
    annotation->setShapeId(AnnotationShape_SHAPEID);

    annotation->setAnnotationTextData(annotation->textShapeData());
    return annotation;
}

KoShape *AnnotationTextShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(params);
    AnnotationTextShape *shape = static_cast<AnnotationTextShape *>(createDefaultShape(documentResources));
    shape->textShapeData()->document()->setUndoRedoEnabled(false);

    if (documentResources) {
        shape->setImageCollection(documentResources->imageCollection());
    }
    shape->textShapeData()->document()->setUndoRedoEnabled(true);
    return shape;
}

bool AnnotationTextShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return (e.localName() == "annotation" && e.namespaceURI() == KoXmlNS::office);
}
