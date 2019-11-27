/* This file is part of the KDE project
 * Copyright (C) 2010-2015 C. Boemann <cbo@boemann.dk>
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
#include "KWView.h"
#include "KWPage.h"
#include "frames/KWCopyShape.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrameLayout.h"

#include <KoTextLayoutRootArea.h>
#include <KoShape.h>
#include <KoShapeContainer.h>
#include <KoShapeFactoryBase.h>
#include <KoTextShapeData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutObstruction.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoShapeAnchor.h>
#include <KoColumns.h>

#include <QTimer>
#include <WordsDebug.h>

class KWTextLayoutRootArea : public KoTextLayoutRootArea
{
    public:
        KWTextLayoutRootArea(KoTextDocumentLayout *documentLayout, KWTextFrameSet *frameSet, int pageNumber) : KoTextLayoutRootArea(documentLayout), m_frameSet(frameSet), m_pageNumber(pageNumber) {
            //debugWords;
        }
        ~KWTextLayoutRootArea() override {
            //debugWords;
        }
        virtual bool layout(FrameIterator *cursor) {
            //debugWords << "pageNumber=" << m_pageNumber << "frameSetType=" << Words::frameSetTypeName(m_frameSet->textFrameSetType()) << "isDirty=" << isDirty();
            bool ok = KoTextLayoutRootArea::layout(cursor);
            return ok;
        }
        KWTextFrameSet *m_frameSet;
        int m_pageNumber;
};

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet)
    : KWRootAreaProviderBase(textFrameSet)
{
}

KWRootAreaProvider::~KWRootAreaProvider()
{
    qDeleteAll(m_rootAreaCache);
    m_rootAreaCache.clear();
    qDeleteAll(m_pages);
    m_pages.clear();
}

void KWRootAreaProvider::clearPages(int pageNumber)
{
    if (pageNumber > pages().count()) {
        return;
    }

    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(frameSet()->document()->documentLayout());
    Q_ASSERT(lay);
    int prevPageIndex = pageNumber - 2;
    do {
        KWRootAreaPage *prevPage = prevPageIndex >= 0 && prevPageIndex < pages().count() ? pages()[prevPageIndex] : 0;
        if (prevPage) {
            if (prevPage->rootAreas.isEmpty()) {
                --prevPageIndex;
                continue; // this page doesn't have any root-areas so try the next previous page
            }
            QList<KoTextLayoutRootArea *> rootAreas = prevPage->rootAreas;
            foreach(KoTextLayoutRootArea *area, rootAreas) {
                releaseAllAfter(area);
                lay->removeRootArea(area);
            }
        } else {
            releaseAllAfter(0);
            lay->removeRootArea(0);
        }
    } while(false);
}

void KWRootAreaProvider::addDependentProvider(KWRootAreaProviderBase *provider, int pageNumber)
{
    debugWords;
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(provider->frameSet()->document()->documentLayout());
    Q_ASSERT(lay);
    lay->setContinuousLayout(false); // to abort the current layout-loop
    lay->setBlockLayout(true); // to prevent a new layout-loop from being started

    m_dependentProviders.append(QPair<KWRootAreaProviderBase *, int>(provider, pageNumber));
}

void KWRootAreaProvider::setPageDirty(int pageNumber)
{
    if (pageNumber - 1 < m_pages.count()) {
        KWRootAreaPage *page = m_pages[pageNumber - 1];
        foreach(KoTextLayoutRootArea *rootArea, page->rootAreas) {
            rootArea->setDirty(); // be sure the root-areas from the page are relayouted
        }
    }
}

void KWRootAreaProvider::handleDependentProviders(int pageNumber)
{
    QList<KoTextDocumentLayout *> layouts;
    for(int i = m_dependentProviders.count() - 1; i >= 0; --i) {
        QPair<KWRootAreaProviderBase *, int> p = m_dependentProviders[i];
        if (p.second > pageNumber) { // only handle providers which would continue layouting at the page we just processed
            continue;
        }
        m_dependentProviders.removeAt(i); // this one is handled now
        p.first->setPageDirty(pageNumber);
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(p.first->frameSet()->document()->documentLayout());
        Q_ASSERT(lay);
        if (!layouts.contains(lay)) {
            layouts.append(lay);
        }
    }

    foreach(KoTextDocumentLayout *lay, layouts) {
        lay->setContinuousLayout(true); // to not abort the current layout-loop any longer
        lay->setBlockLayout(false); // allow layouting again
        lay->layout(); // continue layouting but don't schedule so we are sure it's done instantly
    }
}

KoTextLayoutRootArea* KWRootAreaProvider::provideNext(KoTextDocumentLayout *documentLayout, const RootAreaConstraint &constraints)
{
    KWDocument *kwdoc = frameSet()->wordsDocument();
    KWPageManager *pageManager = kwdoc->pageManager();
    Q_ASSERT(pageManager);

    int pageNumber = 1;
    KWRootAreaPage *rootAreaPage = m_pages.isEmpty() ? 0 : m_pages.last();
    int requiredRootAreaCount = 1;
    if (rootAreaPage && frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
        Q_ASSERT(rootAreaPage->page.isValid());
        Q_ASSERT(rootAreaPage->page.pageStyle().isValid());
        requiredRootAreaCount = rootAreaPage->page.pageStyle().columns().count;
        if (constraints.newPageForced) {
            requiredRootAreaCount = 1;
        }
    }
    if (rootAreaPage && rootAreaPage->rootAreas.count() < requiredRootAreaCount) {
        pageNumber = m_pages.count(); // the root-area is still on the same page
    } else {
        pageNumber = m_pages.count() + 1; // the root-area is the first on a new page

        if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
            // Create missing KWPage's (they will also create a KWFrame and TextShape per page)
            for(int i = pageManager->pageCount(); i < pageNumber; ++i) {
                KWPage page = kwdoc->appendPage(constraints.masterPageName);
                Q_ASSERT(page.isValid());
                if (constraints.visiblePageNumber >= 0)
                    page.setVisiblePageNumber(constraints.visiblePageNumber);
            }
        } else if (pageNumber > pageManager->pageCount()) {
            if (Words::isHeaderFooter(frameSet())) {
                // Headers and footers are special in that they continue with every page what is why they depend on the mainframe.
                KWRootAreaProvider *provider = (KWRootAreaProvider *)kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider();
                provider->addDependentProvider(this, pageNumber);
            }
            return 0; // not ready to layout this yet
        }

        KWPage page = pageManager->page(pageNumber);
        Q_ASSERT(page.isValid());
        if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
            if (constraints.visiblePageNumber >= 0)
                page.setVisiblePageNumber(constraints.visiblePageNumber);
            else
                page.setVisiblePageNumber(0);
        }
        rootAreaPage = new KWRootAreaPage(page);
        m_pages.append(rootAreaPage);
    }

    debugWords << "pageNumber=" << pageNumber <<  "frameSet=" << Words::frameSetTypeName(frameSet()->textFrameSetType());
    if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
        handleDependentProviders(pageNumber);
    }
    // Determinate the frames that are on the page. Note that the KWFrameLayout only knows
    // about header, footer and the mainframes but not about all other framesets.
    QList<KoShape *> shapes;

    if (frameSet()->type() == Words::OtherFrameSet || frameSet()->textFrameSetType() == Words::OtherTextFrameSet) {
        if (KoShape *s = frameSet()->shapes().value(pageNumber - 1))
            shapes = QList<KoShape *>() << s;
    } else {
        shapes = kwdoc->frameLayout()->sequencedShapesOn(frameSet(), pageNumber);
    }

    // position OtherFrameSet's which are anchored to this page
    if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
        foreach(KWFrameSet* fs, kwdoc->frameSets()) {
            KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
            if (tfs && tfs->textFrameSetType() != Words::OtherTextFrameSet)
                continue;
            foreach (KWFrame *frame, fs->frames()) {
                KoShape *shape = frame->shape();
                int anchoredPageNumber = shape->anchor() ? shape->anchor()->pageNumber() : -1;
                if (anchoredPageNumber == pageNumber) {
                    qreal oldOffset = frame->anchoredFrameOffset();
                    qreal newOffset = rootAreaPage->page.offsetInDocument();
                    if (!qFuzzyCompare(1 + oldOffset, 1 + newOffset)) {
                        frame->setAnchoredFrameOffset(newOffset);
                        QPointF pos(shape->position().x(), newOffset - oldOffset + shape->position().y());
                        shape->setPosition(pos);
                    }

                    // During load we make page anchored shapes invisible, because otherwise
                    // they leave empty rects in the text if there is run-around
                    // now is the time to make them visible again
                    shape->setVisible(true);

                    QPointF delta;
                    KWFrameLayout::proposeShapeMove(shape, delta, rootAreaPage->page);
                    shape->setPosition(shape->position() + delta);
               }
            }
        }
    } else {
        if (!documentLayout->referencedLayout()) {
            KoTextDocumentLayout *reflay = dynamic_cast<KoTextDocumentLayout*>(kwdoc->frameLayout()->mainFrameSet()->document()->documentLayout());
            documentLayout->setReferencedLayout(reflay);
        }
    }

    KoShape *shape = rootAreaPage->rootAreas.count() < shapes.count() ? shapes[rootAreaPage->rootAreas.count()] : 0;

    KWTextLayoutRootArea *area = new KWTextLayoutRootArea(documentLayout, frameSet(), pageNumber);
    if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
        if (rootAreaPage->page.pageStyle().columns().count > 1) {
            area->setAcceptsColumnBreak(true);
        }
        area->setAcceptsPageBreak(true);
    }

    if (shape) { // Not every KoTextLayoutRootArea has a KoShape for display purposes.
        //Q_ASSERT_X(pageNumber == pageManager->page(shape).pageNumber(), __FUNCTION__, QString("KWPageManager is out-of-sync, pageNumber=%1 vs pageNumber=%2 with offset=%3 vs offset=%4 on frameSetType=%5").arg(pageNumber).arg(pageManager->page(shape).pageNumber()).arg(shape->absolutePosition().y()).arg(pageManager->page(shape).offsetInDocument()).arg(Words::frameSetTypeName(frameSet()->textFrameSetType())).toLocal8Bit());
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
        if (data) {
            data->setRootArea(area);
            area->setAssociatedShape(shape);
        } else {
            warnWords << "shape has no KoTextShapeData";
        }
        if ((!shape->anchor()) || shape->anchor()->anchorType() == KoShapeAnchor::AnchorPage) {
            area->setPage(new KWPage(rootAreaPage->page));
        }
    }

    if (frameSet()->type() != Words::OtherFrameSet && frameSet()->textFrameSetType() != Words::OtherTextFrameSet) {
        // Only header, footer and main-frames have an own KoTextPage. All other frames are embedded into them and inherit the KoTextPage from them.
        area->setPage(new KWPage(rootAreaPage->page));
        area->setLayoutEnvironmentResctictions(true, false);
    } else {
        area->setLayoutEnvironmentResctictions(true, true);
    }

    m_pageHash[area] = rootAreaPage;
    rootAreaPage->rootAreas.append(area);

    return area;
}

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout* documentLayout, const RootAreaConstraint& constraints, int requestedPosition, bool *isNewArea)
{
    KWPageManager *pageManager = frameSet()->wordsDocument()->pageManager();
    Q_ASSERT(pageManager);
    if (pageManager->pageCount() == 0) // not ready yet (may happen e.g. on loading a document)
        return 0;

    QString reallyNeededPageStyle = constraints.masterPageName;
    int visiblePageNumber = constraints.visiblePageNumber;
    bool newPageForced = constraints.newPageForced;
    if (m_rootAreaCache.size() > requestedPosition)
    {
        KoTextLayoutRootArea *rootArea = m_rootAreaCache[requestedPosition];
        Q_ASSERT(rootArea);

        if (frameSet()->textFrameSetType() != Words::MainTextFrameSet)
        {
            // No constraints except for the main frame set
            *isNewArea = false;
            return rootArea;
        }

        KWRootAreaPage *rootAreaPage = m_pageHash.value(rootArea);
        Q_ASSERT(rootAreaPage);

        if (constraints.visiblePageNumber >= 0)
            rootAreaPage->page.setVisiblePageNumber(constraints.visiblePageNumber);
        else
            rootAreaPage->page.setVisiblePageNumber(0);

        QString reallyNeededPageStyle = constraints.masterPageName;
        if (reallyNeededPageStyle.isNull())
        {
            // We must work using the previous pages...
            if (requestedPosition == 0)
            {
                reallyNeededPageStyle = pageManager->defaultPageStyle().name();
            }
            else
            {
                KWRootAreaPage *previousAreaPage = m_pageHash.value(m_rootAreaCache[requestedPosition - 1]);
                reallyNeededPageStyle = previousAreaPage->page.pageStyle().nextStyleName();
                if (reallyNeededPageStyle.isNull())
                    reallyNeededPageStyle = previousAreaPage->page.pageStyle().name();
            }
        }

        if (rootAreaPage->page.masterPageName() != reallyNeededPageStyle)
        {
            //TODO : recycle pages in order to save us a lot of effort and reduce risks of flickering, especially with very long documents
            releaseAllAfter(rootArea);
        }
        else
        {
            *isNewArea = false;
            return rootArea;
        }
    }

    // We are interested in the first KoTextLayoutRootArea that has a shape associated for display
    // purposes. This can mean that multiple KoTextLayoutRootArea are created but only selected
    // ones that should be layouted and displayed are passed on to the textlayout-library.
    // This is only done for headers and footers cause they are continuous whereas for example
    // Words::OtherFrameSet and Words::OtherTextFrameSet framesets may not have the correct position
    // or not shape assigned at this point but later.
    RootAreaConstraint realConstraints;
    realConstraints.masterPageName = reallyNeededPageStyle;
    realConstraints.visiblePageNumber = visiblePageNumber;
    realConstraints.newPageForced = newPageForced;
    KoTextLayoutRootArea *area = 0;
    do {
        area = provideNext(documentLayout, realConstraints);
        if (m_rootAreaCache.size() <= requestedPosition)
            m_rootAreaCache.append(area);
    } while(Words::isHeaderFooter(frameSet()) && area && !area->associatedShape());

    Q_ASSERT(m_rootAreaCache.size() > requestedPosition);

    if (area == 0 && (frameSet()->textFrameSetType() != Words::MainTextFrameSet) && requestedPosition == 0)
        m_rootAreaCache.clear();
    *isNewArea = true;

    return area;
}

// afterThis==nullptr means delete everything
void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    int afterIndex = -1;
    if (afterThis) {
        if (!m_pageHash.contains(afterThis))
            return;
        KWRootAreaPage *page = m_pageHash.value(afterThis);
        afterIndex = m_pages.indexOf(page);
        Q_ASSERT(afterIndex >= 0);

        int newSize = m_rootAreaCache.indexOf(afterThis) + 1;
        while (m_rootAreaCache.size() != newSize)
        {
            KoTextLayoutRootArea *oldArea = m_rootAreaCache.takeLast();
            delete(oldArea);
        }
    }

    debugWords << "afterPageNumber=" << afterIndex+1;

    bool atLeastOnePageRemoved = false;
    KWPageManager *pageManager = frameSet()->wordsDocument()->pageManager();
    if (afterIndex >= 0) {
        for(int i = m_pages.count() - 1; i > afterIndex; --i) {
            KWRootAreaPage *page = m_pages.takeLast();
            foreach(KoTextLayoutRootArea *area, page->rootAreas)
                m_pageHash.remove(area);
            delete page;

            if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
                pageManager->removePage(i+1);
                atLeastOnePageRemoved = true;
            }
        }

        // FIXME
        for(int i = m_dependentProviders.count() - 1; i >= 0; --i) {
            QPair<KWRootAreaProviderBase *, int> p = m_dependentProviders[i];
            if (p.second >= afterIndex)
                m_dependentProviders.removeAt(i);
        }
    } else {
        //atLeastOnePageRemoved = !m_pages.isEmpty();
        qDeleteAll(m_pages);
        qDeleteAll(m_rootAreaCache);
        m_pages.clear();
        m_pageHash.clear();
        m_rootAreaCache.clear();

        /*FIXME that would result in flickering :-/
        for(int i = pageManager->pageCount(); i >= 1; --i)
            pageManager->removePage(i);
        */

        /*FIXME
        m_dependentProviders.clear();
        */
    }
     if (atLeastOnePageRemoved)
         frameSet()->wordsDocument()->firePageSetupChanged();
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    KWDocument *kwdoc = const_cast<KWDocument*>(frameSet()->wordsDocument());
    KWPageManager *pageManager = kwdoc->pageManager();
    Q_ASSERT(pageManager);

    if (frameSet()->textFrameSetType() != Words::MainTextFrameSet) {
        if (m_pages.count() > pageManager->pageCount()) {
            // we need to wait for the mainFrameSet to finish till we are able to continue
            KWRootAreaProvider *provider = (KWRootAreaProvider *)kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider();
            provider->addDependentProvider(this, m_pages.count());
        }
    }

    KoShape *shape = rootArea->associatedShape();
    if (!shape)
        return;

    KWPage page = pageManager->page(shape);
    Q_ASSERT(page.isValid());
    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);
    bool isHeaderFooter = Words::isHeaderFooter(frameSet());

    debugWords << "pageNumber=" << page.pageNumber() << "frameSetType=" << Words::frameSetTypeName(frameSet()->textFrameSetType()) << "isNewRootArea=" << isNewRootArea << "rootArea=" << rootArea << "isDirty=" << rootArea->isDirty();

    QRectF updateRect = shape->outlineRect();

    QSizeF newSize = shape->size()
                    - QSizeF(data->leftPadding() + data->rightPadding(),
                             data->topPadding() + data->bottomPadding());

    KoBorder *border = shape->border();

    if (border) {
        newSize -= QSizeF(border->borderWidth(KoBorder::LeftBorder) + border->borderWidth(KoBorder::RightBorder), border->borderWidth(KoBorder::TopBorder) + border->borderWidth(KoBorder::BottomBorder));
    }

    if (isHeaderFooter
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {

        newSize.setHeight(rootArea->bottom() - rootArea->top());

        if (frameSet()->type() == Words::OtherFrameSet || frameSet()->textFrameSetType() == Words::OtherTextFrameSet) {
            // adjust size to have at least the defined minimum height
            Q_ASSERT(frameSet()->shapeCount() > 0);
            KoShape *firstShape = frameSet()->shapes().first();
            if (firstShape->minimumHeight() > newSize.height())
                newSize.setHeight(firstShape->minimumHeight());
        }
    }
    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidth) {
        newSize.setWidth(rootArea->right() - rootArea->left());
    }

    // To make sure footnotes always end up at the bottom of the main area we need to set this
    if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
        rootArea->setBottom(rootArea->top() + newSize.height());
    }

    newSize += QSizeF(data->leftPadding() + data->rightPadding(),
                      data->topPadding() + data->bottomPadding());
    if (border) {
        newSize += QSizeF(border->borderWidth(KoBorder::LeftBorder) + border->borderWidth(KoBorder::RightBorder), border->borderWidth(KoBorder::TopBorder) + border->borderWidth(KoBorder::BottomBorder));
    }

    if (newSize != rootArea->associatedShape()->size()) {
        rootArea->associatedShape()->setSize(newSize);

        // transfer the new size to the copy-shapes
        foreach(KWCopyShape *cs, frameSet()->copyShapes()) {
            cs->setSize(newSize);
        }

        if (isHeaderFooter) {
            // adjust the minimum shape height for headers and footer
            Q_ASSERT(frameSet()->shapeCount() > 0);
            KoShape *firstShape = frameSet()->shapes().first();
            if (firstShape->minimumHeight() != newSize.height()) {
                firstShape->setMinimumHeight(newSize.height());

                // transfer the new minimumFrameHeight to the copy-shapes too
                foreach(KWCopyShape *cs, frameSet()->copyShapes()) {
                    cs->setMinimumHeight(newSize.height());
                }
                // cause the header/footer's height changed we have to relayout the whole page
                frameSet()->wordsDocument()->frameLayout()->layoutFramesOnPage(page.pageNumber());
            }
        }
    }


    updateRect |= rootArea->associatedShape()->outlineRect();
    rootArea->associatedShape()->update(updateRect);

    if (frameSet()->textFrameSetType() == Words::MainTextFrameSet) {
        handleDependentProviders(page.pageNumber());
    }

}
