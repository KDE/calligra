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

    switch(m_textFrameSet->textFrameSetType()) {
        case KWord::OddPagesHeaderTextFrameSet:
        case KWord::EvenPagesHeaderTextFrameSet:
        case KWord::OddPagesFooterTextFrameSet:
        case KWord::EvenPagesFooterTextFrameSet:
            kDebug() << m_textFrameSet << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType()) << "rootAreasCount=" << rootAreas.count() << "frameCount=" << m_textFrameSet->frameCount() << "pageCount=" << pageManager->pageCount();
            break;
        case KWord::MainTextFrameSet: {
            // Create missing KWPage's (they will also create a KWTextFrame and TextShape per page)
            KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
            Q_ASSERT(kwdoc);
            int framesCountBefore = m_textFrameSet->frameCount();
            QList<int> pagesCreated;
            for(int i = pageManager->pageCount(); i <= rootAreas.count(); ++i) {
                KWPage page = kwdoc->appendPage();
                Q_ASSERT(page.isValid());

                pagesCreated << page.pageNumber();
            }
            kDebug() << m_textFrameSet << "(MainTextFrameSet) rootAreasCount=" << rootAreas.count() << "frameCount=" << m_textFrameSet->frameCount() << "frameCountBefore=" << framesCountBefore << "pageCount=" << pageManager->pageCount() << "pagesCreated=" << pagesCreated;
        } break;
        default: break;
    }

    /*
    // Create missing frames for each KWPage
    for(int i = m_textFrameSet->frameCount() + 1; i <= pageManager->pageCount(); ++i) {
        // Create the KWTextFrame's for the new KWPage
        KWFrameLayout *framelayout = kwdoc->frameLayout();
        int fc = m_textFrameSet->frameCount();
        framelayout->createNewFramesForPage(i);
        kDebug() << "createFrame " << i << fc << m_textFrameSet->frameCount();
    }
    */

    //FIXME don't use m_textFrameSet->frames() cause it can contain other frames too
    Q_ASSERT(m_textFrameSet->frameCount() >= 1);
    KWFrame *frame = m_textFrameSet->frames()[ m_textFrameSet->frameCount() - 1 ];

//Q_ASSERT(!m_rootAreas.contains(frame));
//if (m_rootAreas.contains(frame)) return m_rootAreas[frame];
if (m_rootAreas.contains(frame)) return 0;

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

// afterThis==0 means delete everything
void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    kDebug();
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
    Q_ASSERT(pageManager);
    KoShape *shape = rootArea->associatedShape();
    Q_ASSERT(shape);
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);
    KWPage page = pageManager->page(shape);
    Q_ASSERT(page.isValid());
    bool isHeaderFooter = KWord::isHeaderFooter(m_textFrameSet);

    kDebug() << "pageNumber=" << page.pageNumber() << "frameSetType=" << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType()) << "isNewRootArea=" << isNewRootArea;

    if (isHeaderFooter || data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight || data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        // adjust the size of the shape
        rootArea->associatedShape()->setSize(QSize(rootArea->associatedShape()->size().width(), qMax(rootArea->associatedShape()->size().height(), rootArea->bottom() - rootArea->top())));

        if (isHeaderFooter) {
            // adjust the minimum frame height for headers and footer
            const qreal h = rootArea->associatedShape()->size().height();
            Q_ASSERT(m_textFrameSet->frameCount() > 0);
            KWTextFrame *frame = static_cast<KWTextFrame*>(m_textFrameSet->frames().first());
            if (frame->minimumFrameHeight() != h) {
                frame->setMinimumFrameHeight(h);
                // cause the header/footer's height changed we have to relayout the whole page
                m_textFrameSet->kwordDocument()->frameLayout()->layoutFramesOnPage(page.pageNumber());
            }
        }
    } else {
        // header and footer should always have AutoGrowHeight see the KWTextFrame ctor
        Q_ASSERT(!isHeaderFooter);

        // adjust the rootArea to the new shape size
        rootArea->setBottom(rootArea->top() + rootArea->associatedShape()->size().height());
    }

    /* already done in KWDocument::addFrame on shapeManager()->addShape

    // emits KWDocument::pageSetupChanged which calls KWViewMode::updatePageCache
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);
    kwdoc->firePageSetupChanged();
    */

    // force repaint
//     rootArea->associatedShape()->update();
}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    KoShape *shape = rootArea->associatedShape();
    Q_ASSERT(shape);
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight || data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        QSizeF size = shape->size();
        size.setHeight(1E6);
        return size;
    } else {
        return shape->size();
    }
}
