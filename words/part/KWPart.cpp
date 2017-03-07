/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>

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

#include "KWPart.h"

#include "KWView.h"
#include "KWDocument.h"
#include "KWCanvasItem.h"
#include "KWFactory.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include "dialogs/KWStartupWidget.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoToolManager.h>
#include <KoInteractionTool.h>
#include <KoShapeRegistry.h>
#include <KoShapeManager.h>
#include <KoComponentData.h>

#include <kmessagebox.h>

KWPart::KWPart(QObject *parent)
    : KoPart(KWFactory::componentData(), parent)
    , m_document(0)
{
    setTemplatesResourcePath(QLatin1String("calligrawords/templates/"));
}

KWPart::KWPart(const KoComponentData &componentData, QObject *parent)
    : KoPart(componentData, parent)
    , m_document(0)
{
    setTemplatesResourcePath(QLatin1String("calligrawords/templates/"));
}

KWPart::~KWPart()
{
}

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
    KWView *view = new KWView(this, qobject_cast<KWDocument*>(document), parent);
    setupViewInstance(document, view);
    return view;
}

void KWPart::setupViewInstance(KoDocument *document, KWView *view)
{
    connect(document, SIGNAL(shapeAdded(KoShape*,KoShapeManager::Repaint)), view->canvasBase()->shapeManager(), SLOT(addShape(KoShape*,KoShapeManager::Repaint)));
    connect(document, SIGNAL(shapeRemoved(KoShape*)), view->canvasBase()->shapeManager(), SLOT(remove(KoShape*)));
    connect(document, SIGNAL(resourceChanged(int,QVariant)), view->canvasBase()->resourceManager(), SLOT(setResource(int,QVariant)));

    bool switchToolCalled = false;
    foreach (KWFrameSet *fs, qobject_cast<KWDocument*>(document)->frameSets()) {
        if (fs->shapeCount() == 0)
            continue;
        foreach (KoShape *shape, fs->shapes())
            view->canvasBase()->shapeManager()->addShape(shape, KoShapeManager::AddWithoutRepaint);
        if (switchToolCalled)
            continue;
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs && tfs->textFrameSetType() == Words::MainTextFrameSet) {
            KoSelection *selection = view->canvasBase()->shapeManager()->selection();
            selection->select(fs->shapes().first());

            KoToolManager::instance()->switchToolRequested(
                KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes()));
            switchToolCalled = true;
        }
    }
    if (!switchToolCalled)
        KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);
}

QGraphicsItem *KWPart::createCanvasItem(KoDocument *document)
{
    // caller owns the canvas item
    KWCanvasItem *item = new KWCanvasItem(QString(), qobject_cast<KWDocument*>(document));
    foreach (KWFrameSet *fs, qobject_cast<KWDocument*>(document)->frameSets()) {
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
        QTimer::singleShot(0, this, SLOT(showErrorAndDie()));
    else
        KoPart::showStartUpWidget(parent, alwaysShow);
}

void KWPart::showErrorAndDie()
{
    KMessageBox::error(0,
                       i18n("Can not find needed text component, Words will quit now"),
                       i18n("Installation Error"));
    QCoreApplication::exit(10);
}
