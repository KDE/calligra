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
    KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
    Q_ASSERT(pageManager);
    QList<KoTextLayoutRootArea *> rootAreas = documentLayout->rootAreas();

    kDebug() << rootAreas.count() << m_textFrameSet->frameCount() << pageManager->pageCount();

    // add missing pages
    for(int i = pageManager->pageCount(); i <= rootAreas.count(); ++i) {
        // First add a new KWPage
        const KWPageStyle pageStyle(pageManager->pageCount() > 0 ? pageManager->page(pageManager->pageCount() - 1).pageStyle() : pageManager->defaultPageStyle());
        KWPage page = pageManager->appendPage(pageStyle);
        Q_ASSERT(page.isValid());
        kDebug() << "Appended new page with pageNumber=" << page.pageNumber();
#if 0
        // Then create the TextShape for the new KWPage
        KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
        Q_ASSERT(factory);
        KoResourceManager *rm = m_textFrameSet->kwordDocument()->resourceManager();
        KoShape *shape = factory->createDefaultShape(rm);
        Q_ASSERT(shape);
#endif
        // And finally create the KWTextFrame's for the new KWPage
        KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
        KWFrameLayout *frlay = kwdoc->frameLayout();
        frlay->createNewFramesForPage(page.pageNumber());
    }

//     Q_ASSERT(rootAreas.count() < m_textFrameSet->frames().count());
    KWFrame *frame = m_textFrameSet->frames()[ qMax(0,rootAreas.count()-1) ];
    KoShape *shape = frame->shape();
    Q_ASSERT(shape);

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    KoTextLayoutRootArea *area = new KoTextLayoutRootArea(documentLayout);
    area->setAssociatedShape(shape);
    data->setRootArea(area);
    return area;
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
