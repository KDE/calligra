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

#include <kglobal.h>
#include <kmessagebox.h>

KWPart::KWPart(QObject *parent)
    : KoPart(parent)
{
    setComponentData(KWFactory::componentData(), false);

    setTemplateType("words_template");
}

KWPart::~KWPart()
{
}

void KWPart::setDocument(KWDocument *document)
{
    KoPart::setDocument(document);
    m_document = document;
}

KoView *KWPart::createViewInstance(QWidget *parent)
{
    KWView *view = new KWView(this, m_document, parent);
    connect(m_document, SIGNAL(shapeAdded(KoShape *)), view->canvasBase()->shapeManager(), SLOT(addShape(KoShape *)));
    connect(m_document, SIGNAL(resourceChanged(int, const QVariant &)), view->canvasBase()->resourceManager(), SLOT(setResource(int, const QVariant &)));

    bool switchToolCalled = false;
    foreach (KWFrameSet *fs, m_document->frameSets()) {
        if (fs->frameCount() == 0)
            continue;
        foreach (KWFrame *frame, fs->frames())
            view->canvasBase()->shapeManager()->addShape(frame->shape(), KoShapeManager::AddWithoutRepaint);
        if (switchToolCalled)
            continue;
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs && tfs->textFrameSetType() == Words::MainTextFrameSet) {
            KoSelection *selection = view->canvasBase()->shapeManager()->selection();
            selection->select(fs->frames().first()->shape());

            KoToolManager::instance()->switchToolRequested(
                KoToolManager::instance()->preferredToolForSelection(selection->selectedShapes()));
            switchToolCalled = true;
        }
    }
    if (!switchToolCalled)
        KoToolManager::instance()->switchToolRequested(KoInteractionTool_ID);
    return view;
}

QGraphicsItem *KWPart::createCanvasItem()
{
    // caller owns the canvas item
    KWCanvasItem *item = new KWCanvasItem(QString(), m_document);
    foreach (KWFrameSet *fs, m_document->frameSets()) {
        if (fs->frameCount() == 0) {
            continue;
        }
        foreach (KWFrame *frame, fs->frames()) {
            item->shapeManager()->addShape(frame->shape(), KoShapeManager::AddWithoutRepaint);
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
