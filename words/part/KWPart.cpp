/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KWPart.h"

#include "KWCanvasItem.h"
#include "KWDocument.h"
#include "KWFactory.h"
#include "KWView.h"
#include "dialogs/KWStartupWidget.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"

#include <KoCanvasBase.h>
#include <KoComponentData.h>
#include <KoInteractionTool.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoToolManager.h>

#include <KMessageBox>

#include <QTimer>

KWPart::KWPart(QObject *parent)
    : KoPart(KWFactory::componentData(), parent)
    , m_document(nullptr)
{
    setTemplatesResourcePath(QLatin1String("calligrawords/templates/"));
}

KWPart::KWPart(const KoComponentData &componentData, QObject *parent)
    : KoPart(componentData, parent)
    , m_document(nullptr)
{
    setTemplatesResourcePath(QLatin1String("calligrawords/templates/"));
}

KWPart::~KWPart() = default;

void KWPart::setDocument(KWDocument *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KWDocument *KWPart::document() const
{
    return m_document;
}

KoView *KWPart::createViewInstance(KoDocument *document, QWidget *parent)
{
    auto kwdocument = qobject_cast<KWDocument *>(document);
    KWView *view = new KWView(this, kwdocument, parent);
    setupViewInstance(kwdocument, view);
    return view;
}

void KWPart::setupViewInstance(KWDocument *document, KWView *view)
{
    connect(document, &KWDocument::shapeAdded, view->canvasBase()->shapeManager(), &KoShapeManager::addShape);
    connect(document, &KWDocument::shapeRemoved, view->canvasBase()->shapeManager(), &KoShapeManager::remove);
    connect(document,
            &KWDocument::resourceChanged,
            view->canvasBase()->resourceManager(),
            QOverload<int, const QVariant &>::of(&KoCanvasResourceManager::setResource));

    bool switchToolCalled = false;
    foreach (KWFrameSet *fs, qobject_cast<KWDocument *>(document)->frameSets()) {
        if (fs->shapeCount() == 0)
            continue;
        foreach (KoShape *shape, fs->shapes())
            view->canvasBase()->shapeManager()->addShape(shape, KoShapeManager::AddWithoutRepaint);
        if (switchToolCalled)
            continue;
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet *>(fs);
        if (tfs && tfs->textFrameSetType() == Words::MainTextFrameSet) {
            KoSelection *selection = view->canvasBase()->shapeManager()->selection();
            selection->select(fs->shapes().first());

            KoToolManager::instance()->switchToolRequested(KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes()));
            switchToolCalled = true;
        }
    }
    if (!switchToolCalled)
        KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);
}

QGraphicsItem *KWPart::createCanvasItem(KoDocument *document)
{
    // caller owns the canvas item
    KWCanvasItem *item = new KWCanvasItem(QString(), qobject_cast<KWDocument *>(document));
    foreach (KWFrameSet *fs, qobject_cast<KWDocument *>(document)->frameSets()) {
        if (fs->shapeCount() == 0) {
            continue;
        }
        foreach (KoShape *shape, fs->shapes()) {
            item->shapeManager()->addShape(shape, KoShapeManager::AddWithoutRepaint);
        }
    }
    return item;
}

QList<KoPart::CustomDocumentWidgetItem> KWPart::createCustomDocumentWidgets(QWidget *parent)
{
    KoColumns columns;
    columns.count = 1;
    columns.gapWidth = 20;

    QList<KoPart::CustomDocumentWidgetItem> widgetList;
    KoPart::CustomDocumentWidgetItem item;
    item.widget = new KWStartupWidget(parent, m_document, columns);
    widgetList << item;
    return widgetList;
}

KoMainWindow *KWPart::createMainWindow()
{
    return new KoMainWindow(WORDS_MIME_TYPE, componentData());
}

void KWPart::showStartUpWidget(KoMainWindow *parent, bool alwaysShow)
{
    // print error if kotext not available
    if (KoShapeRegistry::instance()->value(TextShape_SHAPEID) == 0)
        // need to wait 1 event since exiting here would not work.
        QTimer::singleShot(0, this, &KWPart::showErrorAndDie);
    else
        KoPart::showStartUpWidget(parent, alwaysShow);
}

void KWPart::showErrorAndDie()
{
    KMessageBox::error(nullptr, i18n("Can not find needed text component, Words will quit now"), i18n("Installation Error"));
    QCoreApplication::exit(10);
}
