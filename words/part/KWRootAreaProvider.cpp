/* This file is part of the KDE project
 * Copyright (C) 2010-2012 C. Boemann <cbo@boemann.dk>
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
#include <KoParagraphStyle.h>
#include <KoTableStyle.h>
#include <KoTextAnchor.h>

#include <QTimer>
#include <kdebug.h>

class KWTextLayoutRootArea : public KoTextLayoutRootArea
{
    public:
        KWTextLayoutRootArea(KoTextDocumentLayout *documentLayout, KWTextFrameSet *frameSet, KWFrame *frame, int pageNumber) : KoTextLayoutRootArea(documentLayout), m_frameSet(frameSet), m_frame(frame), m_pageNumber(pageNumber) {
            //kDebug(32001);
        }
        virtual ~KWTextLayoutRootArea() {
            //kDebug(32001);
        }
        virtual bool layout(FrameIterator *cursor) {
            //kDebug(32001) << "pageNumber=" << m_pageNumber << "frameSetType=" << Words::frameSetTypeName(m_frameSet->textFrameSetType()) << "isDirty=" << isDirty();
            bool ok = KoTextLayoutRootArea::layout(cursor);
            return ok;
        }
        KWTextFrameSet *m_frameSet;
        KWFrame *m_frame;
        int m_pageNumber;
};

KWRootAreaProvider::KWRootAreaProvider(KWTextFrameSet *textFrameSet)
    : KoTextLayoutRootAreaProvider()
    , m_textFrameSet(textFrameSet)
{
}

KWRootAreaProvider::~KWRootAreaProvider()
{
    qDeleteAll(m_pages);
}

void KWRootAreaProvider::clearPages(int pageNumber)
{
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(m_textFrameSet->document()->documentLayout());
    Q_ASSERT(lay);
    int prevPageIndex = pageNumber - 2;
    do {
        KWRootAreaPage *prevPage = prevPageIndex >= 0 && prevPageIndex < m_textFrameSet->rootAreaProvider()->pages().count() ? m_textFrameSet->rootAreaProvider()->pages()[prevPageIndex] : 0;
        if (prevPage) {
            if (prevPage->rootAreas.isEmpty()) {
                --prevPageIndex;
                continue; // this page doesn't have any root-areas so try the next previous page
            }
            QList<KoTextLayoutRootArea *> rootAreas = prevPage->rootAreas;
            foreach(KoTextLayoutRootArea *area, rootAreas) {
                m_textFrameSet->rootAreaProvider()->releaseAllAfter(area);
                lay->removeRootArea(area);
            }
        } else {
            m_textFrameSet->rootAreaProvider()->releaseAllAfter(0);
            lay->removeRootArea(0);
        }
    } while(false);
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
        Q_ASSERT(pageNumber - 1 <= p.first->m_pages.count());
        m_dependentProviders.removeAt(i); // this one is handled now
        if (pageNumber - 1 < p.first->m_pages.count()) {
            KWRootAreaPage *page = p.first->m_pages[pageNumber - 1];
            foreach(KoTextLayoutRootArea *rootArea, page->rootAreas) {
                rootArea->setDirty(); // be sure the root-areas from the page are relayouted
            }
        }
        KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(p.first->m_textFrameSet->document()->documentLayout());
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

KoTextLayoutRootArea* KWRootAreaProvider::provideNext(KoTextDocumentLayout *documentLayout)
{
    KWPageManager *pageManager = m_textFrameSet->wordsDocument()->pageManager();
    Q_ASSERT(pageManager);

    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->wordsDocument());
    Q_ASSERT(kwdoc);

    int pageNumber = 1;
    KWRootAreaPage *rootAreaPage = m_pages.isEmpty() ? 0 : m_pages.last();

    int requiredRootAreaCount = 1;
    if (rootAreaPage && m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
        Q_ASSERT(rootAreaPage->page.isValid());
        Q_ASSERT(rootAreaPage->page.pageStyle().isValid());
        requiredRootAreaCount = rootAreaPage->page.pageStyle().columns().count;
    }
    if (rootAreaPage && rootAreaPage->rootAreas.count() < requiredRootAreaCount) {
        pageNumber = m_pages.count(); // the root-area is still on the same page
    } else {
        pageNumber = m_pages.count() + 1; // the root-area is the first on a new page

        if (m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
            // Create missing KWPage's (they will also create a KWFrame and TextShape per page)
            for(int i = pageManager->pageCount(); i < pageNumber; ++i) {
                QString masterPageName;
                int visiblePageNumber = -1;
                QList<KoTextLayoutRootArea *> rootAreasBefore = m_pages[i - 1]->rootAreas;
                if (!rootAreasBefore.isEmpty()) {
                    //FIXME this assumes that a) endTextFrameIterator() will return the starting-it of the
                    //next QTextBlock which wasn't layouted yet and b) that it's a good idea to deal with
                    //the API on that level. We probably like to hide such logic direct within the
                    //textlayout-library and provide a more abstract way to deal with content.
                    QTextFrame::iterator it = rootAreasBefore.last()->endTextFrameIterator();
                    QTextBlock firstBlock = it.currentBlock();
                    QTextTable *firstTable = qobject_cast<QTextTable*>(it.currentFrame());
                    if (firstBlock.isValid()) {
                        masterPageName = firstBlock.blockFormat().property(KoParagraphStyle::MasterPageName).toString();
                        bool ok;
                        int num = firstBlock.blockFormat().property(KoParagraphStyle::PageNumber).toInt(&ok);
                        if (ok)
                            visiblePageNumber = num;
                    }
                    if (firstTable) {
                        masterPageName = firstTable->frameFormat().property(KoTableStyle::MasterPageName).toString();
                        bool ok;
                        int num = firstTable->frameFormat().property(KoTableStyle::PageNumber).toInt(&ok);
                        if (ok)
                            visiblePageNumber = num;
                    }
                }
                KWPage page = kwdoc->appendPage(masterPageName, false);
                Q_ASSERT(page.isValid());
                if (visiblePageNumber >= 0)
                    page.setVisiblePageNumber(visiblePageNumber);
            }
        } else if (pageNumber > pageManager->pageCount()) {
            if (Words::isHeaderFooter(m_textFrameSet)) {
                // Headers and footers are special in that they continue with every page what is why they depend on the mainframe.
                kwdoc->frameLayout()->mainFrameSet()->rootAreaProvider()->addDependentProvider(this, pageNumber);
            }
            return 0; // not ready to layout this yet
        }

        KWPage page = pageManager->page(pageNumber);
        Q_ASSERT(page.isValid());
        rootAreaPage = new KWRootAreaPage(page);
        m_pages.append(rootAreaPage);
    }

    kDebug(32001) << "pageNumber=" << pageNumber <<  "frameSet=" << Words::frameSetTypeName(m_textFrameSet->textFrameSetType());

    handleDependentProviders(pageNumber);

    // Determinate the frames that are on the page. Note that the KWFrameLayout only knows
    // about header, footer and the mainframes but not about all other framesets.
    QList<KWFrame *> frames;
    if (m_textFrameSet->type() == Words::OtherFrameSet || m_textFrameSet->textFrameSetType() == Words::OtherTextFrameSet) {
        if (KWFrame *f = m_textFrameSet->frames().value(pageNumber - 1))
            frames = QList<KWFrame *>() << f;
    } else {
        frames = kwdoc->frameLayout()->framesOn(m_textFrameSet, pageNumber);
    }

    // position OtherFrameSet's which are anchored to this page
    if (m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
        foreach(KWFrameSet* fs, kwdoc->frameSets()) {
            KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
            if (fs->type() != Words::OtherFrameSet && (!tfs || tfs->textFrameSetType() != Words::OtherTextFrameSet))
                continue;
            foreach (KWFrame *frame, fs->frames()) {
                if (frame->anchoredPageNumber() == pageNumber) {
                    qreal oldOffset = frame->anchoredFrameOffset();
                    qreal newOffset = rootAreaPage->page.offsetInDocument();
                    if (!qFuzzyCompare(1 + oldOffset, 1 + newOffset)) {
                        frame->setAnchoredFrameOffset(newOffset);
                        QPointF pos(frame->shape()->position().x(), newOffset - oldOffset + frame->shape()->position().y());
                        frame->shape()->setPosition(pos);
                    }
                }
            }
        }
    } else {
        if (!documentLayout->referencedLayout()) {
            KoTextDocumentLayout *reflay = dynamic_cast<KoTextDocumentLayout*>(kwdoc->frameLayout()->mainFrameSet()->document()->documentLayout());
            documentLayout->setReferencedLayout(reflay);
        }
    }

    KWFrame *frame = rootAreaPage->rootAreas.count() < frames.count() ? frames[rootAreaPage->rootAreas.count()] : 0;

    KWTextLayoutRootArea *area = new KWTextLayoutRootArea(documentLayout, m_textFrameSet, frame, pageNumber);
    if (m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
        area->setAcceptsPageBreak(true);
    }

    if (frame) { // Not every KoTextLayoutRootArea has a frame that contains a KoShape for display purposes.
        KoShape *shape = frame->shape();
        Q_ASSERT(shape);
        //Q_ASSERT_X(pageNumber == pageManager->page(shape).pageNumber(), __FUNCTION__, QString("KWPageManager is out-of-sync, pageNumber=%1 vs pageNumber=%2 with offset=%3 vs offset=%4 on frameSetType=%5").arg(pageNumber).arg(pageManager->page(shape).pageNumber()).arg(shape->absolutePosition().y()).arg(pageManager->page(shape).offsetInDocument()).arg(Words::frameSetTypeName(m_textFrameSet->textFrameSetType())).toLocal8Bit());
        KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
        if (data) {
            data->setRootArea(area);
            area->setAssociatedShape(shape);
        } else {
            kWarning(32001) << "shape has no KoTextShapeData";
        }
    }

    if (m_textFrameSet->type() != Words::OtherFrameSet && m_textFrameSet->textFrameSetType() != Words::OtherTextFrameSet) {
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

KoTextLayoutRootArea *KWRootAreaProvider::provide(KoTextDocumentLayout *documentLayout)
{
    KWPageManager *pageManager = m_textFrameSet->wordsDocument()->pageManager();
    Q_ASSERT(pageManager);
    if (pageManager->pageCount() == 0) // not ready yet (may happen e.g. on loading a document)
        return 0;

    // We are interested in the first KoTextLayoutRootArea that has a shape associated for display
    // purposes. This can mean that multiple KoTextLayoutRootArea are created but only selected
    // ones that should be layouted and displayed are passed on to the textlayout-library.
    // This is only done for headers and footers cause they are continuous whereas for example
    // Words::OtherFrameSet and Words::OtherTextFrameSet framesets may not have the correct position
    // or not shape assigned at this point but later.
    KoTextLayoutRootArea *area = 0;
    do {
        area = provideNext(documentLayout);
    } while(Words::isHeaderFooter(m_textFrameSet) && area && !area->associatedShape());

    return area;
}

// afterThis==NULL means delete everything
void KWRootAreaProvider::releaseAllAfter(KoTextLayoutRootArea *afterThis)
{
    int afterIndex = -1;
    if (afterThis) {
        if (!m_pageHash.contains(afterThis))
            return;
        KWRootAreaPage *page = m_pageHash[afterThis];
        afterIndex = m_pages.indexOf(page);
        Q_ASSERT(afterIndex >= 0);
    }

    kDebug(32001) << "afterPageNumber=" << afterIndex+1;

    bool atLeastOnePageRemoved = false;
    KWPageManager *pageManager = m_textFrameSet->wordsDocument()->pageManager();
    if (afterIndex >= 0) {
        for(int i = m_pages.count() - 1; i > afterIndex; --i) {
            KWRootAreaPage *page = m_pages.takeLast();
            foreach(KoTextLayoutRootArea *area, page->rootAreas)
                m_pageHash.remove(area);
            delete page;

            if (m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
                pageManager->removePage(i+1);
                atLeastOnePageRemoved = true;
            }
        }

        // FIXME
        for(int i = m_dependentProviders.count() - 1; i >= 0; --i) {
            QPair<KWRootAreaProvider *, int> p = m_dependentProviders[i];
            if (p.second >= afterIndex)
                m_dependentProviders.removeAt(i);
        }
    } else {
        //atLeastOnePageRemoved = !m_pages.isEmpty();
        qDeleteAll(m_pages);
        m_pages.clear();
        m_pageHash.clear();

        /*FIXME that would result in flickering :-/
        for(int i = pageManager->pageCount(); i >= 1; --i)
            pageManager->removePage(i);
        */

        /*FIXME
        m_dependentProviders.clear();
        */
    }
     if (atLeastOnePageRemoved)
         m_textFrameSet->wordsDocument()->firePageSetupChanged();
}

void KWRootAreaProvider::doPostLayout(KoTextLayoutRootArea *rootArea, bool isNewRootArea)
{
    KWDocument *kwdoc = const_cast<KWDocument*>(m_textFrameSet->wordsDocument());
    KWPageManager *pageManager = kwdoc->pageManager();
    Q_ASSERT(pageManager);

    if (m_textFrameSet->textFrameSetType() != Words::MainTextFrameSet) {
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
    bool isHeaderFooter = Words::isHeaderFooter(m_textFrameSet);

    kDebug(32001) << "pageNumber=" << page.pageNumber() << "frameSetType=" << Words::frameSetTypeName(m_textFrameSet->textFrameSetType()) << "isNewRootArea=" << isNewRootArea << "rootArea=" << rootArea << "isDirty=" << rootArea->isDirty();

    QRectF updateRect = rootArea->associatedShape()->outlineRect();
    //rootArea->associatedShape()->update(updateRect);

    QSizeF newSize = rootArea->associatedShape()->size()
                    - QSizeF(data->leftPadding() + data->rightPadding(),
                             data->topPadding() + data->bottomPadding());
    if (isHeaderFooter
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowHeight) {

        newSize.setHeight(rootArea->bottom() - rootArea->top());

        if (m_textFrameSet->type() == Words::OtherFrameSet || m_textFrameSet->textFrameSetType() == Words::OtherTextFrameSet) {
            // adjust size to have at least the defined minimum height
            Q_ASSERT(m_textFrameSet->frameCount() > 0);
            KWFrame *frame = static_cast<KWFrame*>(m_textFrameSet->frames().first());
            if (frame->minimumFrameHeight() > newSize.height())
                newSize.setHeight(frame->minimumFrameHeight());
        }
    }
    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight
        ||data->resizeMethod() == KoTextShapeData::AutoGrowWidth) {
        newSize.setWidth(rootArea->right() - rootArea->left());
    }

    // To make sure footnotes always end up at the bottom of the main area we need to set this
    if (m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
        rootArea->setBottom(rootArea->top() + newSize.height());
    }

    newSize += QSizeF(data->leftPadding() + data->rightPadding(),
                      data->topPadding() + data->bottomPadding());
    if (newSize != rootArea->associatedShape()->size()) {
        //QPointF centerpos = rootArea->associatedShape()->absolutePosition();
        rootArea->associatedShape()->setSize(newSize);
        //rootArea->associatedShape()->setAbsolutePosition(centerpos);

//TODO we would need to do something like the following to relayout all affected
//pages again but that is so terrible slow that it's unusable. We need to find
//a better solution for that.
#if 0
        // the list of pages that need 
        QList<int> relayoutPages;
        
        // transfer the new size to the copy-shapes
        if (KWFrame *frame = dynamic_cast<KWFrame*>(rootArea->associatedShape()->applicationData())) {
            foreach(KWFrame* f, frame->copies()) {
                if (f->shape()) {
                    f->shape()->setSize(newSize);
                    KWPage p = pageManager->page(f->shape());
                    if (p.isValid() && !relayoutPages.contains(p.pageNumber()))
                        relayoutPages.append(p.pageNumber());
                }
            }
        }
            
        if (isHeaderFooter) {
            // adjust the minimum frame height for headers and footer
            Q_ASSERT(m_textFrameSet->frameCount() > 0);
            KWFrame *frame = static_cast<KWFrame*>(m_textFrameSet->frames().first());
            if (frame->minimumFrameHeight() != newSize.height()) {
                frame->setMinimumFrameHeight(newSize.height());
                if (!relayoutPages.contains(page.pageNumber()))
                    relayoutPages.append(page.pageNumber());
            }
        }

        qSort(relayoutPages);
        foreach(int pageNumber, relayoutPages)
            m_textFrameSet->wordsDocument()->frameLayout()->layoutFramesOnPage(pageNumber);
#else

        // transfer the new size to the copy-shapes
        if (KWFrame *frame = dynamic_cast<KWFrame*>(rootArea->associatedShape()->applicationData())) {
            foreach(KWFrame* f, frame->copies()) {
                if (f->shape()) {
                    f->shape()->setSize(newSize);
                }
            }
        }

        if (isHeaderFooter) {
            // adjust the minimum frame height for headers and footer
            Q_ASSERT(m_textFrameSet->frameCount() > 0);
            KWFrame *frame = static_cast<KWFrame*>(m_textFrameSet->frames().first());
            if (frame->minimumFrameHeight() != newSize.height()) {
                frame->setMinimumFrameHeight(newSize.height());

                // cause the header/footer's height changed we have to relayout the whole page
                m_textFrameSet->wordsDocument()->frameLayout()->layoutFramesOnPage(page.pageNumber());
            }
        }
#endif
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

    updateRect |= rootArea->associatedShape()->outlineRect();
    rootArea->associatedShape()->update(updateRect);

#if 0 
    // enabling this makes loading documents > 100 pages much much slower
    // for documents > 400 it gets so slow that it is barly useable.
    // only enable if you know what you are doing and never commit it enabled
    // temporary sanity-check
    for(int i = 1; i <= pageManager->pageCount(); ++i) {
        KWPage page = pageManager->page(i);
        Q_ASSERT(i == page.pageNumber());

        KWTextFrameSet *mainFrameSet       = kwdoc->frameLayout()->mainFrameSet();
        KWTextFrameSet *oddHeaderFrameSet  = kwdoc->frameLayout()->getFrameSet(Words::OddPagesHeaderTextFrameSet, page.pageStyle());
        KWTextFrameSet *evenHeaderFrameSet = kwdoc->frameLayout()->getFrameSet(Words::EvenPagesHeaderTextFrameSet, page.pageStyle());
        KWTextFrameSet *oddFooterFrameSet  = kwdoc->frameLayout()->getFrameSet(Words::OddPagesFooterTextFrameSet, page.pageStyle());
        KWTextFrameSet *evenFooterFrameSet = kwdoc->frameLayout()->getFrameSet(Words::EvenPagesFooterTextFrameSet, page.pageStyle());

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
#endif

    if (m_textFrameSet->textFrameSetType() == Words::MainTextFrameSet) {
        handleDependentProviders(page.pageNumber());
    }

}

QSizeF KWRootAreaProvider::suggestSize(KoTextLayoutRootArea *rootArea)
{
    KoShape *shape = rootArea->associatedShape();
    if (!shape) { // no shape => nothing to draw => no space needed
        return QSizeF(0.,0.);
    }

    KoTextShapeData *data = qobject_cast<KoTextShapeData*>(shape->userData());
    Q_ASSERT(data);

    QSizeF size = shape->size() - QSizeF(data->leftPadding() + data->rightPadding(), data->topPadding() + data->bottomPadding());
    size.setWidth(qMax(size.width(), qreal(1.0)));
    size.setHeight(qMax(size.height(), qreal(1.0)));
    if (data->resizeMethod() == KoTextShapeData::AutoGrowWidthAndHeight || data->resizeMethod() == KoTextShapeData::AutoGrowHeight
        || m_textFrameSet->textFrameSetType() == Words::OtherTextFrameSet) {
        size.setHeight(1E6);
    }

    return size;
}

QList<KoTextLayoutObstruction *> KWRootAreaProvider::relevantObstructions(KoTextLayoutRootArea *rootArea)
{
    QList<KoTextLayoutObstruction*> obstructions;
    Q_ASSERT(rootArea);

    KoShape *currentShape = rootArea->associatedShape();

    if(!currentShape)
        return obstructions;

    // let's convert into canvas/KWDocument coords
    QRectF rect = currentShape->boundingRect();

    //TODO would probably be faster if we could use the RTree of the shape manager
    foreach (KWFrameSet *fs, m_textFrameSet->wordsDocument()->frameSets()) {
        if (fs  == m_textFrameSet) {
            continue; // we don't collide with ourselves
        }

        if (KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs)) {
            if (tfs->textFrameSetType() != Words::OtherTextFrameSet) {
                continue; // we don't collide with headers, footers and main-text.
            }
        }

        foreach (KWFrame *frame, fs->frames()) {
            KoShape *shape = frame->shape();
            if (shape == currentShape)
                continue;
            if (! shape->isVisible(true))
                continue;
            if (frame->anchorType() != KoTextAnchor::AnchorPage)
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

