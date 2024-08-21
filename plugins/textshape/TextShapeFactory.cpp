/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TextShapeFactory.h"
#include "TextShape.h"

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
#include <QTextCursor>
#include <kundo2stack.h>

TextShapeFactory::TextShapeFactory()
    : KoShapeFactoryBase(TextShape_SHAPEID, i18n("Text"))
{
    setToolTip(i18n("A shape that shows text"));
    QList<QPair<QString, QStringList>> odfElements;
    odfElements.append(QPair<QString, QStringList>(KoXmlNS::draw, QStringList("text-box")));
    odfElements.append(QPair<QString, QStringList>(KoXmlNS::table, QStringList("table")));
    setXmlElements(odfElements);
    setLoadingPriority(1);

    KoShapeTemplate t;
    t.name = i18n("Text");
    t.iconName = koIconName("x-shape-text");
    t.toolTip = i18n("Text Shape");
    KoProperties *props = new KoProperties();
    t.properties = props;
    props->setProperty("demo", true);
    addTemplate(t);
}

KoShape *TextShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
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
    TextShape *text = new TextShape(manager, locationManager);
    if (documentResources) {
        KoTextDocument document(text->textShapeData()->document());

        if (documentResources->hasResource(KoText::StyleManager)) {
            KoStyleManager *styleManager = documentResources->resource(KoText::StyleManager).value<KoStyleManager *>();
            document.setStyleManager(styleManager);
        }

        // this is needed so the shape can reinitialize itself with the stylemanager
        text->textShapeData()->setDocument(text->textShapeData()->document(), true);

        document.setUndoStack(documentResources->undoStack());

        if (documentResources->hasResource(KoText::PageProvider)) {
            KoPageProvider *pp = static_cast<KoPageProvider *>(documentResources->resource(KoText::PageProvider).value<void *>());
            text->setPageProvider(pp);
        }
        if (documentResources->hasResource(KoText::ChangeTracker)) {
            KoChangeTracker *changeTracker = documentResources->resource(KoText::ChangeTracker).value<KoChangeTracker *>();
            document.setChangeTracker(changeTracker);
        }

        document.setShapeController(documentResources->shapeController());

        // update the resources of the document
        text->updateDocumentData();

        text->setImageCollection(documentResources->imageCollection());
    }

    return text;
}

KoShape *TextShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(params)
    TextShape *shape = static_cast<TextShape *>(createDefaultShape(documentResources));
    shape->textShapeData()->document()->setUndoRedoEnabled(false);
    shape->setSize(QSizeF(300, 200));
    /*
    QString text("text");
    if (params->contains(text)) {
        KoTextShapeData *shapeData = qobject_cast<KoTextShapeData*>(shape->userData());
    }
    */
    if (documentResources) {
        shape->setImageCollection(documentResources->imageCollection());
    }
    shape->textShapeData()->document()->setUndoRedoEnabled(true);
    return shape;
}

bool TextShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return (e.localName() == "text-box" && e.namespaceURI() == KoXmlNS::draw) || (e.localName() == "table" && e.namespaceURI() == KoXmlNS::table);
}

void TextShapeFactory::newDocumentResourceManager(KoDocumentResourceManager *manager) const
{
    QVariant variant;
    variant.setValue<KoInlineTextObjectManager *>(new KoInlineTextObjectManager(manager));
    manager->setResource(KoText::InlineTextObjectManager, variant);

    variant.setValue<KoTextRangeManager *>(new KoTextRangeManager());
    manager->setResource(KoText::TextRangeManager, variant);

    if (!manager->hasResource(KoDocumentResourceManager::UndoStack)) {
        //        qWarning() << "No KUndo2Stack found in the document resource manager, creating a new one";
        manager->setUndoStack(new KUndo2Stack(manager));
    }
    if (!manager->hasResource(KoText::StyleManager)) {
        variant.setValue(new KoStyleManager(manager));
        manager->setResource(KoText::StyleManager, variant);
    }
    if (!manager->imageCollection())
        manager->setImageCollection(new KoImageCollection(manager));
}
