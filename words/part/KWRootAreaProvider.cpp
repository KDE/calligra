/* This file is part of the KDE project
 * Copyright (C) 2010-2011 Casper Boemann <cbo@boemann.dk>
 * Copyright (C) 2006,2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2006-2007, 2010 Thomas Zander <zander@kde.org>
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
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutObstruction.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>

#include <QTimer>
#include <kdebug.h>

class KWTextLayoutRootArea : public KoTextLayoutRootArea
{
    public:
        KWTextLayoutRootArea(KoTextDocumentLayout *documentLayout, KWTextFrameSet *frameSet, KWFrame *frame, const KWPage &page) : KoTextLayoutRootArea(documentLayout), m_frameSet(frameSet), m_frame(frame), m_page(page) {
            kDebug(32001);
        }
        virtual ~KWTextLayoutRootArea() {
            kDebug();
        }
        virtual bool layout(FrameIterator *cursor) {
            kDebug(32001) << "pageNumber=" << m_page.pageNumber() << "frameSetType=" << KWord::frameSetTypeName(m_frameSet->textFrameSetType()) << "isDirty=" << isDirty();
            bool ok = KoTextLayoutRootArea::layout(cursor);
            return ok;
        }
        KWTextFrameSet *m_frameSet;
        KWFrame *m_frame;
        KWPage m_page;
};

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
{
}

KWRootAreaProvider::~KWRootAreaProvider()
{
}

void KWRootAreaProvider::addDependentProvider(KWRootAreaProvider *provider, int pageNumber)
{
    kDebug(32001);
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(provider->m_textFrameSet->document()->documentLayout());
    Q_ASSERT(lay);
    lay->setContinuousLayout(false); // to abort the current layout-loop
    lay->setBlockLayout(true); // to prevent a new layout-loop from being started

    m_dependentProviders.append(QPair<KWRootAreaProvider *, int>(provider, pageNumber));
}

void KWRootAreaProvider::handleDependentProviders(int pageNumber)
{
    QList<KoTextDocumentLayout *> layouts;
    for(int i = m_dependentProviders.count() - 1; i >= 0; --i) {
        QPair<KWRootAreaProvider *, int> p = m_dependentProviders[i];
        if (p.second > pageNumber) { // only handle providers which would continue layouting at the page we just processed
            continue;
        }

        m_dependentProviders.removeAt(i); // this one is handled now

        Q_ASSERT(pageNumber - 1 < p.first->m_pages.count());
        KoTextLayoutRootArea *rootArea = p.first->m_pages[pageNumber - 1];
        rootArea->setDirty();

        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(p.first->m_textFrameSet->document()->documentLayout());
        Q_ASSERT(lay);
        if (!layouts.contains(lay))
            layouts.append(lay);
    }

    foreach(KoTextDocumentLayout *lay, layouts) {
        lay->setContinuousLayout(true); // to not abort the current layout-loop any longer
        lay->setBlockLayout(false); // allow layouting again
        lay->layout(); // continue layouting but don't schedule so we are sure it's done instantly
    }
}

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout *documentLayout)
{
    KWPageManager *pageManager = m_textFrameSet->kwordDocument()->pageManager();
    Q_ASSERT(pageManager);
    if (pageManager->pageCount() == 0) // not ready yet (may happen e.g. on loading a document)
        return 0;

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

    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);

    int pageNumber = m_pages.count() + 1;

    kDebug(32001) << "pageNumber=" << pageNumber << "frameSetType=" << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType());

    if (m_textFrameSet->textFrameSetType() == KWord::MainTextFrameSet) {
        // Create missing KWPage's (they will also create a KWFrame and TextShape per page)
        for(int i = pageManager->pageCount(); i <= m_pages.count(); ++i) {
            KWPage page = kwdoc->appendPage();
            Q_ASSERT(page.isValid());
        }
        handleDependentProviders(pageNumber);
    } else if (pageNumber > pageManager->pageCount()) {
//         KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(m_textFrameSet->document()->documentLayout());
//         Q_ASSERT(lay);
// Q_ASSERT(!lay->layoutBlocked());
//         lay->scheduleLayout();
        return 0;
    }

    Q_ASSERT(pageNumber <= pageManager->pageCount());

    KWPage page = pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());

    KWFrame *frame = kwdoc->frameLayout()->frameOn(m_textFrameSet, pageNumber);

    if (m_textFrameSet->textFrameSetType() != KWord::OtherTextFrameSet) {
        KWTextFrameSet* tfs = kwdoc->frameLayout()->getFrameSet(m_textFrameSet->textFrameSetType(), m_textFrameSet->pageStyle());
        Q_ASSERT_X(tfs == m_textFrameSet, __FUNCTION__, QString("frameLayout vs rootAreaProvider error, frameSetType=%1 pageNumber=%2 frameCount=%3 pageCount=%4").arg(KWord::frameSetTypeName(m_textFrameSet->textFrameSetType())).arg(pageNumber).arg(m_textFrameSet->frameCount()).arg(pageManager->pageCount()).toLocal8Bit());
    }

    KWTextLayoutRootArea *area = new KWTextLayoutRootArea(documentLayout, m_textFrameSet, frame, page);
    area->setAcceptsPageBreak(true);

    if (frame) {
        KoShape *shape = frame->shape();
        Q_ASSERT(shape);
        Q_ASSERT_X(pageNumber == pageManager->page(shape).pageNumber(), __FUNCTION__, QString("KWPageManager is out-of-sync, pageNumber=%1 vs pageNumber=%2 with offset=%3 vs offset=%4 on frameSetType=%3").arg(pageNumber).arg(pageManager->page(shape).pageNumber()).arg(shape->absolutePosition().y()).arg(pageManager->page(shape).offsetInDocument()).arg(KWord::frameSetTypeName(m_textFrameSet->textFrameSetType())).toLocal8Bit());
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
        Q_ASSERT(data);
        area->setAssociatedShape(shape);
        data->setRootArea(area);
        area->setPage(new KWPage(page));
    }

    m_pages.append(area);
    return area;
}

// afterThis==NULL means delete everything
void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    int afterPageNumber = afterThis ? m_pages.indexOf(afterThis) : -1;

    //kDebug(32001) << "afterPageNumber=" << afterPageNumber;

    if (afterPageNumber >= 1) {
        for(int i = m_pages.count(); i > afterPageNumber; --i) {
            KoTextLayoutRootArea *area = m_pages.takeLast();
            //delete area;
        }
    } else {
        //qDeleteAll(m_pages);
        m_pages.clear();
    }
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    KWPageManager *pageManager = kwdoc->pageManager();
    Q_ASSERT(pageManager);

    if (m_textFrameSet->textFrameSetType() != KWord::MainTextFrameSet) {
        if (m_pages.count() > pageManager->pageCount()) {
            // we need to wait for the mainFrameSet to finish till we are able to continue
            kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider()->addDependentProvider(this, m_pages.count());
        }
    }

    KoShape *shape = rootArea->associatedShape();
    if (!shape)
        return;

    KWPage page = pageManager->page(shape);
    Q_ASSERT(page.isValid());
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);
    bool isHeaderFooter = KWord::isHeaderFooter(m_textFrameSet);

    kDebug(32001) << "pageNumber=" << page.pageNumber() << "frameSetType=" << KWord::frameSetTypeName(m_textFrameSet->textFrameSetType()) << "isNewRootArea=" << isNewRootArea << "rootArea=" << rootArea << "size=" << rootArea->associatedShape()->size();

    QRectF updateRect = rootArea->associatedShape()->outlineRect();
    //rootArea->associatedShape()->update(updateRect);

    QSizeF newSize = rootArea->associatedShape()->size();
    if (isHeaderFooter
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        newSize.setHeight(rootArea->bottom() - rootArea->top());
    }
    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidth) {
        newSize.setWidth(rootArea->right() - rootArea->left());
    }
    if (newSize != rootArea->associatedShape()->size()) {
        //QPointF centerpos = rootArea->associatedShape()->absolutePosition();
        rootArea->associatedShape()->setSize(newSize);
        rootArea->setBottom(rootArea->top() + newSize.height());
        //rootArea->associatedShape()->setAbsolutePosition(centerpos);

        if (isHeaderFooter) {
            // adjust the minimum frame height for headers and footer
            Q_ASSERT(m_textFrameSet->frameCount() > 0);
            KWFrame *frame = static_cast<KWFrame*>(m_textFrameSet->frames().first());
            if (frame->minimumFrameHeight() != newSize.height()) {
                frame->setMinimumFrameHeight(newSize.height());

                // cause the header/footer's height changed we have to relayout the whole page
                m_textFrameSet->kwordDocument()->frameLayout()->layoutFramesOnPage(page.pageNumber());
            }
        }
    }

#if 0
    qreal newBottom = rootArea->top() + rootArea->associatedShape()->size().height();
    if (data->verticalAlignment() & Qt::AlignBottom) {
        if (true /*FIXME test no page based shapes interfering*/) {
            rootArea->setVerticalAlignOffset(newBottom - rootArea->bottom());
        }
    }
    if (data->verticalAlignment() & Qt::AlignVCenter) {
        if (true /*FIXME test no page based shapes interfering*/) {
            rootArea->setVerticalAlignOffset((newBottom - rootArea->bottom()) / 2);
        }
    }
#endif

    /* already done in KWDocument::addFrame on shapeManager()->addShape

    // emits KWDocument::pageSetupChanged which calls KWViewMode::updatePageCache
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->kwordDocument());
    Q_ASSERT(kwdoc);
    kwdoc->firePageSetupChanged();
    */

    updateRect |= rootArea->associatedShape()->outlineRect();
    rootArea->associatedShape()->update(updateRect);

    // temporary sanity-check
    for(int i = 1; i <= pageManager->pageCount(); ++i) {
        KWPage page = pageManager->page(i);
        Q_ASSERT(i == page.pageNumber());

        KWTextFrameSet *mainFrameSet       = kwdoc->frameLayout()->mainFrameSet();
        KWTextFrameSet *oddHeaderFrameSet  = kwdoc->frameLayout()->getFrameSet(KWord::OddPagesHeaderTextFrameSet, page.pageStyle());
        KWTextFrameSet *evenHeaderFrameSet = kwdoc->frameLayout()->getFrameSet(KWord::EvenPagesHeaderTextFrameSet, page.pageStyle());
        KWTextFrameSet *oddFooterFrameSet  = kwdoc->frameLayout()->getFrameSet(KWord::OddPagesFooterTextFrameSet, page.pageStyle());
        KWTextFrameSet *evenFooterFrameSet = kwdoc->frameLayout()->getFrameSet(KWord::EvenPagesFooterTextFrameSet, page.pageStyle());

        KWRootAreaProvider *mainProvider       = mainFrameSet->rootAreaProvider();
        KWRootAreaProvider *oddHeaderProvider  = oddHeaderFrameSet ? oddHeaderFrameSet->rootAreaProvider() : 0;
        KWRootAreaProvider *evenHeaderProvider = evenHeaderFrameSet ? evenHeaderFrameSet->rootAreaProvider() : 0;
        KWRootAreaProvider *oddFooterProvider  = oddFooterFrameSet ? oddFooterFrameSet->rootAreaProvider() : 0;
        KWRootAreaProvider *evenFooterProvider = evenFooterFrameSet ? evenFooterFrameSet->rootAreaProvider() : 0;

        KWFrame *mainFrame       = kwdoc->frameLayout()->frameOn(mainFrameSet, i);
        KWFrame *oddHeaderFrame  = kwdoc->frameLayout()->frameOn(oddHeaderFrameSet, i);
        KWFrame *evenHeaderFrame = kwdoc->frameLayout()->frameOn(evenHeaderFrameSet, i);
        KWFrame *oddFooterFrame  = kwdoc->frameLayout()->frameOn(oddFooterFrameSet, i);
        KWFrame *evenFooterFrame = kwdoc->frameLayout()->frameOn(evenFooterFrameSet, i);

        KWTextLayoutRootArea* mainArea       = mainProvider && i <= mainProvider->m_pages.count() ? dynamic_cast<KWTextLayoutRootArea*>(mainProvider->m_pages[i-1]) : 0;
        KWTextLayoutRootArea* oddHeaderArea  = oddHeaderProvider && i <= oddHeaderProvider->m_pages.count() ? dynamic_cast<KWTextLayoutRootArea*>(oddHeaderProvider->m_pages[i-1]) : 0;
        KWTextLayoutRootArea* evenHeaderArea = evenHeaderProvider && i <= evenHeaderProvider->m_pages.count() ? dynamic_cast<KWTextLayoutRootArea*>(evenHeaderProvider->m_pages[i-1]) : 0;
        KWTextLayoutRootArea* oddFooterArea  = oddFooterProvider && i <= oddFooterProvider->m_pages.count() ? dynamic_cast<KWTextLayoutRootArea*>(oddFooterProvider->m_pages[i-1]) : 0;
        KWTextLayoutRootArea* evenFooterArea = evenFooterProvider && i <= evenFooterProvider->m_pages.count() ? dynamic_cast<KWTextLayoutRootArea*>(evenFooterProvider->m_pages[i-1]) : 0;

        /*
        qDebug() << "pageNumber=" << i
                 << "main=" << mainFrame << (mainArea ? mainArea->associatedShape() : 0)
                 << "oddHeader=" << oddHeaderFrame << (oddHeaderArea ? oddHeaderArea->associatedShape() : 0) << (oddHeaderArea ? oddHeaderArea->isDirty() : true)
                 << "evenHeader=" << evenHeaderFrame << (evenHeaderArea ? evenHeaderArea->associatedShape() : 0) << (evenHeaderArea ? evenHeaderArea->isDirty() : true)
                 << "oddFooter=" << oddFooterFrame << (oddFooterArea ? oddFooterArea->associatedShape() : 0) << (oddFooterArea ? oddFooterArea->isDirty() : true)
                 << "evenFooter=" << evenFooterFrame << (evenFooterArea ? evenFooterArea->associatedShape() : 0) << (evenFooterArea ? evenFooterArea->isDirty() : true);
        */

        Q_ASSERT(!mainArea || mainFrame == mainArea->m_frame);
        Q_ASSERT(!mainFrame || !mainArea || mainFrame->shape() == mainArea->associatedShape());
        if (oddHeaderArea) {
            Q_ASSERT(oddHeaderFrame == oddHeaderArea->m_frame);
            Q_ASSERT(!oddHeaderFrame || oddHeaderFrame->shape() == oddHeaderArea->associatedShape());
        }
        if (evenHeaderArea) {
            Q_ASSERT(evenHeaderFrame == evenHeaderArea->m_frame);
            Q_ASSERT(!evenHeaderFrame || evenHeaderFrame->shape() == evenHeaderArea->associatedShape());
        }
        if (oddFooterArea) {
            Q_ASSERT(oddFooterFrame == oddFooterArea->m_frame);
            Q_ASSERT(!oddFooterFrame || oddFooterFrame->shape() == oddFooterArea->associatedShape());
        }
        if (evenFooterArea) {
            Q_ASSERT(evenFooterFrame == evenFooterArea->m_frame);
            Q_ASSERT(!evenFooterFrame || evenFooterFrame->shape() == evenFooterArea->associatedShape());
        }
    }

    if (m_textFrameSet->textFrameSetType() == KWord::MainTextFrameSet) {
        handleDependentProviders(page.pageNumber());
    }

}

bool KWRootAreaProvider::suggestPageBreak(KoTextLayoutRootArea *beforeThis)
{
    if (m_textFrameSet->textFrameSetType() != KWord::MainTextFrameSet)
        return false;
    int index = m_pages.indexOf(beforeThis);
//     Q_ASSERT_X(index >= 0, __FUNCTION__, QString("Unknown area on page=%1").arg(m_textFrameSet->kwordDocument()->pageManager()->page(beforeThis->associatedShape()).pageNumber()).toLocal8Bit());
    if (index <= 0)
        return false;
    KoTextLayoutRootArea *areaBefore = m_pages[index - 1];
    KoTextPage *pageCurrent = beforeThis->page();
    Q_ASSERT(pageCurrent);
    KoTextPage *pageBefore = areaBefore->page();
    Q_ASSERT(pageBefore);
    return pageCurrent->masterPageName() != pageBefore->masterPageName();
}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    KoShape *shape = rootArea->associatedShape();
    if (!shape) { // no shape => nothing to draw => no space needed
        return QSizeF(0.,0.);
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight || data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {
        QSizeF size = shape->size();
        size.setHeight(1E6);
        return size;
    }

    return shape->size();
}

QList<KoTextLayoutObstruction *> KWRootAreaProvider::relevantObstructions(const QRectF &textRect, const QList<KoTextLayoutObstruction *> &excludingThese)
{
    QList<KoTextLayoutObstruction*> obstructions;

    KoTextLayoutRootArea *rootArea = 0;
    foreach (KoTextLayoutRootArea *area, m_pages) {
        if (area->boundingRect().intersects(textRect)) {
            rootArea = area;
            break;
        }
    }
    if (!rootArea) {
        return obstructions;
    }

    KoShape *currentShape = rootArea->associatedShape();

    // let's convert into canvas/KWDocument coords
    QRectF rect = textRect;
    rect.moveTop(rect.top() - rootArea->top());
    QTransform transform = currentShape->absoluteTransformation(0);
    rect = transform.mapRect(rect);

    //TODO would probably be faster if we could use the RTree of the shape manager
    foreach (KWFrameSet *fs, m_textFrameSet->kwordDocument()->frameSets()) {
        if (fs  == m_textFrameSet)
            continue;
        foreach (KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if (shape == currentShape)
                continue;
            if (! shape->isVisible(true))
                continue;
            if (shape->textRunAroundSide() == KoShape::RunThrough)
                continue;
            if (shape->zIndex() <= currentShape->zIndex())
                continue;
            if (! rect.intersects(shape->boundingRect()))
                continue;
            bool isChild = false;
            KoShape *parent = shape->parent();
            while (parent && !isChild) {
                if (parent == currentShape)
                    isChild = true;
                parent = parent->parent();
            }
            if (isChild)
                continue;
            QTransform matrix = shape->absoluteTransformation(0);
            matrix = matrix * currentShape->absoluteTransformation(0).inverted();
            matrix.translate(0, rootArea->top());
            KoTextLayoutObstruction *obstruction = new KoTextLayoutObstruction(shape, matrix);
            obstructions.append(obstruction);
        }
    }

    return obstructions;
}

