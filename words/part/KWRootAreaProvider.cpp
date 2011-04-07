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
#include "frames/KWTextFrameSet.h"
#include "frames/KWTextFrame.h"
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>

#include <kdebug.h>

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
{
}

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout *documentLayout)
{
#if 0
    Q_ASSERT(m_textFrameSet->pageManager());
//     if (m_textFrameSet->pageManager()->pageCount() > 0) { //FIXME remove this workaround
        const KWPageStyle pageStyle;
        KWPage page = m_textFrameSet->pageManager()->appendPage(pageStyle);

        kDebug() << "pageNumber=" << page.pageNumber();
        Q_ASSERT(page.isValid());

        KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
        KWFrameLayout *frlay = kwdoc->frameLayout();
        frlay->createNewFramesForPage(page.pageNumber());
//     }

    //KWPage page = m_pageManager->page(pageNumber);
    //if(m_area) return 0;

    m_area = new KoTextLayoutRootArea(documentLayout);
    m_area->setAssociatedShape(m_shape);
    //m_area->setReferenceRect(0, m_shape->size().width(), 0, m_shape->size().height());
    m_data->setRootArea(m_area);
    return m_area;
#else

#if 0
    int frameCount = m_textFrameSet->frameCount();
    //int pageCount = m_textFrameSet->pageManager()->pageCount();
    QList<KoTextLayoutRootArea *> rootAreas = documentLayout->rootAreas();
kDebug()<<">>>>>>"<<frameCount<<m_textFrameSet->pageManager()->pageCount();
    KWFrame* frame = 0;
    if (rootAreas.count() > frameCount) { // new frame for rootArea needed
//         frame = m_textFrameSet->appendTextFrame();
        Q_ASSERT(false);
    } else if (rootAreas.count() < frameCount) { // new rootArea for frame needed
        frame = m_textFrameSet->frames()[rootAreas.count()];
    } else {
        Q_ASSERT_X(false, __FUNCTION__, "Unexpected state");
        return 0;
    }

    KoShape *shape = frame->shape();
    Q_ASSERT(shape);

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    KoTextLayoutRootArea *area = new KoTextLayoutRootArea(documentLayout);
    area->setAssociatedShape(shape);
    data->setRootArea(area);

    return area;
#else
    int frameCount = m_textFrameSet->frameCount();
    //int pageCount = m_textFrameSet->pageManager()->pageCount();
    QList<KoTextLayoutRootArea *> rootAreas = documentLayout->rootAreas();

    KWFrame* frame = 0;
    if (rootAreas.count() >= frameCount) { // new frame for rootArea needed

        // First add a new KWPage
        KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
        Q_ASSERT(pageManager);
        const KWPageStyle pageStyle(pageManager->pageCount() > 0 ? pageManager->page(pageManager->pageCount() - 1).pageStyle() : pageManager->defaultPageStyle());
        KWPage page = pageManager->appendPage(pageStyle);
        Q_ASSERT(page.isValid());

        // Then create the TextShape for it
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
        Q_ASSERT(factory);
        KoResourceManager *rm = m_textFrameSet->kwordDocument()->resourceManager();
        KoShape *shape = factory->createDefaultShape(rm);
        Q_ASSERT(shape);

        frame = new KWTextFrame(shape, m_textFrameSet);
    } else if (rootAreas.count() < frameCount) { // new rootArea for frame needed
        frame = m_textFrameSet->frames()[rootAreas.count()];
    }

    KoShape *shape = frame->shape();
    Q_ASSERT(shape);

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    KoTextLayoutRootArea *area = new KoTextLayoutRootArea(documentLayout);
    area->setAssociatedShape(shape);
    data->setRootArea(area);

    return area;
#endif
#endif
}

void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    kDebug();
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea)
{
    kDebug();

    //rootArea->setTop(rootArea->top() + rootArea->associatedShape()->size().height());
    //rootArea->setBottom(rootArea->top() + rootArea->associatedShape()->size().height());
    //rootArea->setReferenceRect(0, rootArea->associatedShape()->size().width(), 0, rootArea->associatedShape()->size().height());

    rootArea->associatedShape()->update();
}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    return rootArea->associatedShape()->size();
}
