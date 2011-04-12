/* This file is part of the KDE project
 * Copyright (C) 2011 Casper Boemann <cbo@boemann.dk>
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

#include "KWRootAreaProvider.h"
#include "KWPageManager.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWView.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWTextFrame.h"
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>

#include <kdebug.h>

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
{
}

KWRootAreaProvider::~KWRootAreaProvider()
{
}

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout *documentLayout, QString mastePageName)
{
    KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
    Q_ASSERT(pageManager);
    QList<KoTextLayoutRootArea *> rootAreas = documentLayout->rootAreas();

    // The page is created in KWTextFrameSet::setupFrame and the TextShape in KWFrameLayout::createTextShape
#if 0
    const KWPageStyle pageStyle(pageManager->pageCount() > 0 ? pageManager->page(pageManager->pageCount() - 1).pageStyle() : pageManager->defaultPageStyle());
    KWPage page = pageManager->appendPage(pageStyle);
    Q_ASSERT(page.isValid());

    KWPage page = m_textFrameSet->kwordDocument()->appendPage();
    Q_ASSERT(page.isValid());

    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoResourceManager *rm = m_textFrameSet->kwordDocument()->resourceManager();
    KoShape *shape = factory->createDefaultShape(rm);
    Q_ASSERT(shape);
#endif

    // Create missing KWPage's and KWTextFrame's
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);
    int framesCountBefore = m_textFrameSet->frameCount();
    QList<int> pagesCreated;
    for(int i = pageManager->pageCount(); i <= rootAreas.count(); ++i) {
        KWPage page = kwdoc->appendPage();
        Q_ASSERT(page.isValid());

        pagesCreated << page.pageNumber();
    }

    kDebug() << "rootAreasCount=" << rootAreas.count()+1 << "frameCount=" << m_textFrameSet->frameCount() << "frameCountBefore=" << framesCountBefore << "pageCount=" << pageManager->pageCount() << "pagesCreated=" << pagesCreated;

    //FIXME don't use m_textFrameSet->frames() cause it can contain other frames too
    Q_ASSERT(m_textFrameSet->frameCount() >= 1);
    KWFrame *frame = m_textFrameSet->frames()[ m_textFrameSet->frameCount() - 1 ];
    Q_ASSERT(!m_rootAreas.contains(frame));
    KoShape *shape = frame->shape();
    Q_ASSERT(shape);

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    KoTextLayoutRootArea *area = new KoTextLayoutRootArea(documentLayout);
    area->setAssociatedShape(shape);
    data->setRootArea(area);

    m_rootAreas[frame] = area;
    return area;
}

void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    kDebug();
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    kDebug();

    //rootArea->setTop(rootArea->top() + rootArea->associatedShape()->size().height());
    //rootArea->setBottom(rootArea->top() + rootArea->associatedShape()->size().height());
    //rootArea->setReferenceRect(0, rootArea->associatedShape()->size().width(), 0, rootArea->associatedShape()->size().height());

    // emits KWDocument::pageSetupChanged which calls KWViewMode::updatePageCache
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);
    kwdoc->firePageSetupChanged();

    // force repaint
    rootArea->associatedShape()->update();

#if 0
    // select the new shape
    foreach (KoView *view, m_textFrameSet->kwordDocument()->views()) {
        KoCanvasBase *canvas = static_cast<KWView*>(view)->canvasBase();
        KoSelection *selection = canvas->shapeManager()->selection();
        selection->deselectAll();
        selection->select(rootArea->associatedShape());
    }
#endif
}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    return rootArea->associatedShape()->size();
}
