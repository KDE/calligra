/* This file is part of the KDE project
 * Copyright (C) 2006-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#include "KWFrameLayout.h"
#include "KWPageManager.h"
#include "KWTextFrameSet.h"
#include "KWTextFrame.h"
#include "KWPageStyle.h"
#include "KWPage.h"
#include "KWCopyShape.h"
#include "KWDocument.h"
#include "KWPageBackground.h"

#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>

#include <QTextLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <klocale.h>
#include <kdebug.h>
#include <limits.h>

KWFrameLayout::KWFrameLayout(const KWPageManager *pageManager, const QList<KWFrameSet*> &frameSets)
        : m_pageManager(pageManager),
        m_frameSets(frameSets),
        m_maintext(0),
        m_document(0),
        m_setup(false)
{
}

void KWFrameLayout::createNewFramesForPage(int pageNumber)
{
//kDebug() <<"createNewFramesForPage" << pageNumber;
    m_setup = false; // force reindexing of types
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    if (!page.isValid()) return; // page already deleted, probably.

    // Header footer handling.
    // first make a list of all types.
    QList<KWord::TextFrameSetType> allHFTypes;
    allHFTypes.append(KWord::OddPagesHeaderTextFrameSet);
    allHFTypes.append(KWord::EvenPagesHeaderTextFrameSet);
    allHFTypes.append(KWord::OddPagesFooterTextFrameSet);
    allHFTypes.append(KWord::EvenPagesFooterTextFrameSet);

    // create page background
    KWTextFrameSet *fs = getOrCreate(KWord::PageBackgroundFrameSet, page);
    if (!hasFrameOn(fs, pageNumber)) {
        KoShape *shape = new KWPageBackground();
        shape->setSize(QSizeF(page.width(), page.height()));
        new KWFrame(shape, fs);
    }

    // create headers & footers
    KWord::TextFrameSetType origin;
    if (shouldHaveHeaderOrFooter(pageNumber, true, &origin)) {
        allHFTypes.removeAll(origin);
        KWTextFrameSet *fs = getOrCreate(origin, page);
        if (!hasFrameOn(fs, pageNumber))
            createCopyFrame(fs, page);
    }
    if (shouldHaveHeaderOrFooter(pageNumber, false, &origin)) {
        allHFTypes.removeAll(origin);
        KWTextFrameSet *fs = getOrCreate(origin, page);
        if (!hasFrameOn(fs, pageNumber))
            createCopyFrame(fs, page);
    }

    // delete headers/footer frames that are not needed on this page
    foreach (KWFrame *frame, framesInPage(page.rect())) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
        if (tfs && allHFTypes.contains(tfs->textFrameSetType())) {
            tfs->removeFrame(frame);
            delete frame;
        }
    }

    // create main text frame. All columns of them.
    if (page.pageStyle().hasMainTextFrame()) {
        int columns = page.pageStyle().columns().columns;
        KWTextFrameSet *fs = getOrCreate(KWord::MainTextFrameSet, page);
        QRectF rect(QPointF(0, page.offsetInDocument()),
                    QSizeF(page.width(), page.height()));
        if (page.pageSide() == KWPage::PageSpread)
            rect.setWidth(rect.width() / 2);
        foreach (KWFrame *frame, framesInPage(rect)) {
            if (frame->frameSet() == fs) {
                columns--;
                if (columns < 0) {
                    fs->removeFrame(frame);
                    delete frame;
                }
            }
        }
        while (columns > 0) {
            new KWTextFrame(createTextShape(page), fs);
            columns--;
        }
    }

    if (page.pageSide() == KWPage::PageSpread) {
        // inline helper class
        class PageSpreadShapeFactory
        {
        public:
            PageSpreadShapeFactory(KWFrameLayout *parent) {
                m_parent = parent;
            }
            void create(const KWPage &page, KWTextFrameSet *fs) {
                KWFrame *frame;
                if (fs->textFrameSetType() == KWord::MainTextFrameSet)
                    frame = new KWTextFrame(m_parent->createTextShape(page), fs);
                else
                    frame = m_parent->createCopyFrame(fs, page);
                KoShape *shape = frame->shape();
                shape->setPosition(QPointF(page.width() / 2 + 1, shape->position().y()));
            }
            KWFrameLayout *m_parent;
        };
        PageSpreadShapeFactory factory(this);
        if (shouldHaveHeaderOrFooter(pageNumber + 1, true, &origin)) {
            KWTextFrameSet *fs = getOrCreate(origin, m_pageManager->page(pageNumber + 1));
            if (!hasFrameOn(fs, pageNumber + 1))
                factory.create(page, fs);
        }
        if (shouldHaveHeaderOrFooter(pageNumber + 1, false, &origin)) {
            KWTextFrameSet *fs = getOrCreate(origin, m_pageManager->page(pageNumber + 1));
            if (!hasFrameOn(fs, pageNumber + 1))
                factory.create(page, fs);
        }
        if (page.pageStyle().hasMainTextFrame()) {
            int columns = page.pageStyle().columns().columns;
            KWTextFrameSet *fs = getOrCreate(KWord::MainTextFrameSet, page);
            QRectF rect(QPointF(page.width(), page.offsetInDocument()),
                        QSizeF(page.width() / 2,  page.height()));
            foreach (KWFrame *frame, framesInPage(rect)) {
                if (frame->frameSet() == fs) {
                    columns--;
                    if (columns < 0) {
                        fs->removeFrame(frame);
                        delete frame;
                    }
                }
            }
            while (columns > 0) {
                factory.create(page, fs);
                columns--;
            }
        }
    }

    bool odd = false; // an odd number of pages back, so frameOnBothSheets matters
    for (int i = pageNumber - 2; i < pageNumber; i++) {
        if (i < m_pageManager->begin().pageNumber()) {
            odd = true;
            continue;
        }
        KWPage prevPage = m_pageManager->page(i);
        QRectF pageRect = prevPage.rect(pageNumber);
        foreach (KWFrame *frame, framesInPage(pageRect)) {
            if (odd && !frame->frameOnBothSheets())
                continue;
            if (!(frame->newFrameBehavior() == KWord::ReconnectNewFrame ||
                    frame->newFrameBehavior() == KWord::CopyNewFrame))
                continue;

            KWFrame *f;
            KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
            if (tfs) {
                if (tfs->textFrameSetType() != KWord::OtherTextFrameSet)
                    continue; // these are copied above already.
                f = new KWTextFrame(createTextShape(page), tfs);
            } else {
                Q_ASSERT(frame->newFrameBehavior() == KWord::CopyNewFrame);
                Q_ASSERT(frame->frameSet());
                Q_ASSERT(frame->frameSet()->frameCount() > 0);
                f = new KWFrame(new KWCopyShape(frame->frameSet()->frames().first()->shape(), m_pageManager), frame->frameSet());
            }
            const qreal y = frame->shape()->position().y();
            qreal offsetFromPage = y - pageRect.top();
            f->copySettings(frame);
            f->shape()->setPosition(QPointF(frame->shape()->position().x(),
                                            page.offsetInDocument() + offsetFromPage));
        }
        odd = true;
    }

    layoutFramesOnPage(pageNumber);
    if (page.pageSide() == KWPage::PageSpread)
        layoutFramesOnPage(pageNumber + 1);
}

void KWFrameLayout::layoutFramesOnPage(int pageNumber)
{
//kDebug() <<"KWFrameLayout::layoutFramesOnPage";
    /* assumes all frames are there and will do layouting of all the frames
        - headers/footers/main FS are positioned
        - normal frames are clipped to page */

    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());

    /* +-----------------+
       |  0              | <- pageStyle->pageLayout()->topMargin + layout->topPadding
       |  1  [ header ]  |
       |  2              | <- pageStyle->headerDistance()
       |  3  [ maintxt ] |
       |  4              | <- pageStyle->endNoteDistance()
       |  5  [ endnote ] |
       |  6              | <- pageStyle->footerDistance()
       |  7  [ footer ]  |
       |  8              | <- pageStyle->pageLayout()->bottomMargin + layout->bottomPadding
       +-----------------+ */

    // Create some data structures used for the layouting of the frames later
    int minZIndex = INT_MAX;
    qreal requestedHeight[9], minimumHeight[9], resultingPositions[9];
    for (int i = 0; i < 9; i++) { // zero fill.
        requestedHeight[i] = 0;
        minimumHeight[i] = 0;
        resultingPositions[i] = 0;
    }
    minimumHeight[0] = page.topMargin() + page.topPadding();
    minimumHeight[8] = page.bottomMargin() + page.bottomPadding();

    KoPageLayout layout = page.pageStyle().pageLayout();
    layout.leftMargin = page.leftMargin();
    layout.rightMargin = page.rightMargin();
    layout.leftPadding = page.leftPadding();
    layout.rightPadding = page.rightPadding();
    qreal left = 0, width = page.width();
    if (page.pageSide() == KWPage::PageSpread) {
        width /= 2;
        layout.leftMargin = page.pageEdgeMargin();
        layout.rightMargin = page.marginClosestBinding();
        if (page.pageNumber() != pageNumber) { // doing the 'right' part
            left = width;
            qSwap(layout.leftMargin, layout.rightMargin);
            qSwap(layout.leftPadding, layout.rightPadding);
        }
    }
    qreal textWidth = width - layout.leftMargin - layout.rightMargin
                            - layout.leftPadding - layout.rightPadding;

    KWPageStyle pageStyle = page.pageStyle();
    const int columns = pageStyle.hasMainTextFrame() ? pageStyle.columns().columns : 0;
    int columnsCount = columns;
    KWTextFrame **main;
    KWTextFrame *footer = 0, *endnote = 0, *header = 0;
    KWFrame *pageBackground = 0;
    main = new KWTextFrame*[columnsCount];
    if (columns > 0)
        main[0] = 0;
    QRectF pageRect(left, page.offsetInDocument(), width, page.height());
    foreach (KWFrame *frame, framesInPage(pageRect)) {
        KWTextFrameSet *textFrameSet = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
        if (textFrameSet == 0 || textFrameSet->textFrameSetType() == KWord::OtherTextFrameSet)
            minZIndex = qMin(minZIndex, frame->shape()->zIndex());
        if (textFrameSet == 0) continue;
        switch (textFrameSet->textFrameSetType()) {
        case KWord::OddPagesHeaderTextFrameSet:
        case KWord::EvenPagesHeaderTextFrameSet: {
            header = static_cast<KWTextFrame *>(frame);
            minimumHeight[1] = qMax((qreal)10, page.pageStyle().headerMinimumHeight());
            requestedHeight[1] = static_cast<KWTextFrame *>(textFrameSet->frames().first())->minimumFrameHeight();
            if (minimumHeight[1] < page.pageStyle().headerDistance())
                minimumHeight[2] = page.pageStyle().headerDistance() - minimumHeight[1];
            break;
        }
        case KWord::OddPagesFooterTextFrameSet:
        case KWord::EvenPagesFooterTextFrameSet: {
            footer = static_cast<KWTextFrame *>(frame);
            minimumHeight[7] = qMax((qreal)10, page.pageStyle().footerMinimumHeight());
            requestedHeight[7] = static_cast<KWTextFrame *>(textFrameSet->frames().first())->minimumFrameHeight();
            if(minimumHeight[7] < page.pageStyle().footerDistance())
                minimumHeight[6] = page.pageStyle().footerDistance() - minimumHeight[7];
            break;
        }
        case KWord::MainTextFrameSet: {
            if (columnsCount < 1) {
                kWarning(32001) << "Too many columns present on page, ignoring 1";
                break;
            }
            main[--columnsCount] = static_cast<KWTextFrame *>(frame);
            minimumHeight[3] = 10;
            // make at least one line fit lest we add endless pages.
            QTextLayout *layout = textFrameSet->document()->begin().layout();
            if (layout && layout->lineCount() > 0) {
                minimumHeight[3] = qMax((qreal) 10, layout->lineAt(0).height());
            }

            requestedHeight[3] = -1; // rest
            break;
        }
        case KWord::PageBackgroundFrameSet:
            pageBackground = static_cast<KWFrame *>(frame);
            break;
        // TODO endnote frameset
        default:;
        }
    }
    if (minZIndex < INT_MAX) {
        --minZIndex;
        if (endnote)
            endnote->shape()->setZIndex(minZIndex--);
        for (int i = 0; i < columns; ++i)
            main[i]->shape()->setZIndex(minZIndex);
        if (footer)
            footer->shape()->setZIndex(--minZIndex);
        if (header)
            header->shape()->setZIndex(--minZIndex);
        if (pageBackground)
            pageBackground->shape()->setZIndex(--minZIndex);
    }

    // spread space across items.
    qreal heightLeft = page.height();
    for (int i = 0; i < 9; i++)
        heightLeft -= qMax(minimumHeight[i], requestedHeight[i]);
    if (heightLeft >= 0) { // easy; plenty of space
        if (minimumHeight[5] > 0) // if we have an endnote
            minimumHeight[6] += heightLeft; // add space below endnote
        else
            minimumHeight[3] += heightLeft; // add space to main text frame
        qreal y = page.offsetInDocument();
        for (int i = 0; i < 9; i++) {
            resultingPositions[i] = y;
            y += qMax(minimumHeight[i], requestedHeight[i]);
        }
    } else {
        // for situations where the header + footer are too big to fit together with a
        // minimum sized main text frame.
        minimumHeight[5] = 0; // no end note
        heightLeft = page.height();
        for (int i = 0; i < 9; i++)
            heightLeft -= minimumHeight[i];
        qreal y = page.offsetInDocument();
        for (int i = 0; i < 9; i++) {
            resultingPositions[i] = y;
            qreal row = minimumHeight[i];
            if (requestedHeight[i] > row) {
                row += heightLeft / 3;
            }
            y += row;
        }
    }

    // actually move / size the frames.
    if (columns > 0 && main[0]) {
        const qreal columnWidth = textWidth / columns;
        QPointF *points = new QPointF[columns];
        for (int i = columns - 1; i >= 0; i--)
            points[i] = QPointF(left + layout.leftMargin + layout.leftPadding
                                + columnWidth * i, resultingPositions[3]);
        for (int i = 0; i < columns; i++) {
            for (int f = 0; f < columns; f++) {
                if (f == i) continue;
                if (qAbs(main[f]->shape()->position().x() - points[i].x()) < 10.0) {
                    qSwap(main[f], main[i]);
                    break;
                }
            }
        }

        bool first = true;
        for (int i = columns - 1; i >= 0; i--) {
            main[i]->setFrameBehavior(KWord::AutoCreateNewFrameBehavior);
            main[i]->setNewFrameBehavior(KWord::ReconnectNewFrame);
            KoShape *shape = main[i]->shape();
            shape->setPosition(points[i]);
            shape->setSize(QSizeF(columnWidth -
                                  (first ? 0 : page.pageStyle().columns().columnSpacing),
                                  resultingPositions[4] - resultingPositions[3]));
            first = false;
        }
        delete[] points;
    }
    if (endnote) {
        endnote->shape()->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[5]));
        endnote->shape()->setSize(QSizeF(textWidth, resultingPositions[6] - resultingPositions[5]));
    }
    if (header) {
        header->shape()->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[1]));
        header->shape()->setSize(QSizeF(textWidth, resultingPositions[2] - resultingPositions[1]));
    }
    if (footer) {
        footer->shape()->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[7]));
        footer->shape()->setSize(QSizeF(textWidth, resultingPositions[8] - resultingPositions[7]));
    }
    if (pageBackground) {
        pageBackground->shape()->setPosition(
            QPointF(left, page.offsetInDocument()));
        pageBackground->shape()->setSize(QSizeF(width, page.height()));
    }
    delete [] main;
// TODO endnotes
}

bool KWFrameLayout::shouldHaveHeaderOrFooter(int pageNumber, bool header, KWord::TextFrameSetType *origin)
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    switch (header ? page.pageStyle().headerPolicy() : page.pageStyle().footerPolicy()) {
    case KWord::HFTypeNone:
        return false;
    case KWord::HFTypeEvenOdd:
        if (header)
            *origin = pageNumber % 2 == 0 ? KWord::EvenPagesHeaderTextFrameSet :
                      KWord::OddPagesHeaderTextFrameSet;
        else
            *origin = pageNumber % 2 == 0 ? KWord::EvenPagesFooterTextFrameSet :
                      KWord::OddPagesFooterTextFrameSet;
        break;
    case KWord::HFTypeUniform:
        *origin = header ? KWord::OddPagesHeaderTextFrameSet : KWord::OddPagesFooterTextFrameSet;
        break;
    }
    return true;
}

QList<KWFrame *> KWFrameLayout::framesInPage(const QRectF &page)
{
    // hopefully replaced with a tree
    QList<KWFrame*> answer;
    foreach (KWFrameSet *fs, m_frameSets) {
        foreach (KWFrame *frame, fs->frames()) {
            if (page.contains(frame->shape()->absolutePosition()))
                answer.append(frame);
        }
    }
    return answer;
}

KWTextFrameSet *KWFrameLayout::getOrCreate(KWord::TextFrameSetType type, const KWPage &page)
{
    Q_ASSERT(page.isValid());
    setup();
    FrameSets frameSets = m_pageStyles.value(page.pageStyle());
    KWTextFrameSet **answer = 0;
    switch (type) {
    case KWord::OddPagesHeaderTextFrameSet:
        answer = &frameSets.oddHeaders;
        break;
    case KWord::EvenPagesHeaderTextFrameSet:
        answer = &frameSets.evenHeaders;
        break;
    case KWord::OddPagesFooterTextFrameSet:
        answer = &frameSets.oddFooters;
        break;
    case KWord::EvenPagesFooterTextFrameSet:
        answer = &frameSets.evenFooters;
        break;
    case KWord::PageBackgroundFrameSet:
        answer = &frameSets.pageBackground;
        break;
    case KWord::MainTextFrameSet:
        answer = &m_maintext;
        break;
    default:
        KWTextFrameSet *newFS = new KWTextFrameSet(m_document);
        emit newFrameSet(newFS);
        return newFS;
    }
    Q_ASSERT(answer);
    if (*answer == 0) { // it wasn't created yet, lets do so now.
        KWTextFrameSet *newFS = new KWTextFrameSet(m_document, type);
        *answer = newFS;
        if (type != KWord::MainTextFrameSet) {
            newFS->setPageStyle(page.pageStyle());
            m_pageStyles.insert(page.pageStyle(), frameSets);
        }
        emit newFrameSet(newFS);
        Q_ASSERT(m_frameSets.contains(newFS)); // the emit should have made that so :)
    }
    return *answer;
}

void KWFrameLayout::setup()
{
    if (m_setup)
        return;

    m_maintext = 0;
    m_pageStyles.clear();
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0)
            continue;
        FrameSets frameSets = m_pageStyles.value(tfs->pageStyle());
        switch (tfs->textFrameSetType()) {
        case KWord::OddPagesHeaderTextFrameSet:
            frameSets.oddHeaders = tfs;
            break;
        case KWord::EvenPagesHeaderTextFrameSet:
            frameSets.evenHeaders = tfs;
            break;
        case KWord::OddPagesFooterTextFrameSet:
            frameSets.oddFooters = tfs;
            break;
        case KWord::EvenPagesFooterTextFrameSet:
            frameSets.evenFooters = tfs;
            break;
        case KWord::PageBackgroundFrameSet:
            frameSets.pageBackground = tfs;
            break;
        case KWord::MainTextFrameSet:
            m_maintext = tfs;
            disconnect(tfs, SIGNAL(frameRemoved(KWFrame*)), this, SLOT(mainframeRemoved(KWFrame*)));
            connect(tfs, SIGNAL(frameRemoved(KWFrame*)), this, SLOT(mainframeRemoved(KWFrame*)));
        default: ;// ignore
        }
        if (tfs->pageStyle().isValid())
            m_pageStyles.insert(tfs->pageStyle(), frameSets);
    }
    m_setup = true;
}

KoShape *KWFrameLayout::createTextShape(const KWPage &page)
{
    Q_ASSERT(page.isValid());
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoResourceManager *rm = 0;
    if (m_document)
        rm = m_document->resourceManager();
    KoShape *shape = factory->createDefaultShape(rm);
    shape->setPosition(QPointF(0, page.offsetInDocument()));
    return shape;
}

bool KWFrameLayout::hasFrameOn(KWTextFrameSet *fs, int pageNumber)
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    foreach (KWFrame *frame, framesInPage(page.rect())) {
        if (frame->frameSet() == fs)
            return true;
    }
    return false;
}

void KWFrameLayout::cleanupHeadersFooters()
{
    QHash<KWPageStyle, FrameSets> pageStyles;
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0)
            continue;
        FrameSets frameSets = pageStyles.value(tfs->pageStyle());
        switch (tfs->textFrameSetType()) {
        case KWord::OddPagesHeaderTextFrameSet:
            if (frameSets.oddHeaders) {
                emit removedFrameSet(frameSets.oddHeaders);
                delete frameSets.oddHeaders;
            }
            frameSets.oddHeaders = tfs;
            break;
        case KWord::EvenPagesHeaderTextFrameSet:
            if (frameSets.evenHeaders) {
                emit removedFrameSet(frameSets.evenHeaders);
                delete frameSets.evenHeaders;
            }
            frameSets.evenHeaders = tfs;
            break;
        case KWord::OddPagesFooterTextFrameSet:
            if (frameSets.oddFooters) {
                emit removedFrameSet(frameSets.oddFooters);
                delete frameSets.oddFooters;
            }
            frameSets.oddFooters = tfs;
            break;
        case KWord::EvenPagesFooterTextFrameSet:
            if (frameSets.evenFooters) {
                emit removedFrameSet(frameSets.evenFooters);
                delete frameSets.evenFooters;
            }
            frameSets.evenFooters = tfs;
            break;
        default: ;// ignore
        }
        if (tfs->pageStyle().isValid())
            pageStyles.insert(tfs->pageStyle(), frameSets);
    }

    foreach (const KWPageStyle &style, pageStyles.keys()) {
        FrameSets frameSets = pageStyles[style];
        switch (style.headerPolicy()) {
        case KWord::HFTypeEvenOdd:
            break;
        case KWord::HFTypeNone:
            cleanFrameSet(frameSets.oddHeaders);
            // fall through
        case KWord::HFTypeUniform:
            cleanFrameSet(frameSets.evenHeaders);
        }
        switch (style.footerPolicy()) {
        case KWord::HFTypeEvenOdd:
            break;
        case KWord::HFTypeNone:
            cleanFrameSet(frameSets.oddFooters);
            // fall through
        case KWord::HFTypeUniform:
            cleanFrameSet(frameSets.evenFooters);
        }
    }
    m_setup = false;
}

void KWFrameLayout::cleanFrameSet(KWTextFrameSet *fs)
{
    if (fs == 0)
        return;
    if (fs->frameCount() == 0)
        return;
    foreach (KWFrame *frame, fs->frames()) {
        fs->removeFrame(frame);
        delete(frame->shape());
    }
}

void KWFrameLayout::createNewFrameForPage(KWTextFrameSet *fs, int pageNumber)
{
    if (fs->frameCount() == 0)
        return;
    if (pageNumber == m_pageManager->begin().pageNumber())
        return;
    qreal prevPage, prevPage2;
    prevPage = m_pageManager->topOfPage(pageNumber - 1);
    if (pageNumber - 2 >= m_pageManager->begin().pageNumber())
        prevPage2 = m_pageManager->topOfPage(pageNumber - 2);
    else
        prevPage2 = -1;


    QList<KWTextFrame*> framesToDuplicate;
    QList<KWFrame*> frames = fs->frames();
    QList<KWFrame*>::Iterator iter = frames.end();
    while (iter != frames.begin()) {
        iter--;
        KWTextFrame *frame = static_cast<KWTextFrame*>(*iter);
        qreal y = frame->shape()->position().y();
        if (y > prevPage) {
            if (frame->frameOnBothSheets())
                framesToDuplicate.prepend(frame);
        } else if (y > prevPage2) {
            if (!frame->frameOnBothSheets())
                framesToDuplicate.prepend(frame);
        } else // more then 2 pages back is not interresting
            break;
    }

    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    const qreal offsetInDocument = page.offsetInDocument();
    // now add them in the proper order.
    foreach (KWTextFrame *f, framesToDuplicate) {
        KWTextFrame *frame = new KWTextFrame(createTextShape(page), fs);
        const qreal y = f->shape()->position().y();
        qreal offsetFromPage = y - prevPage2;
        if (y > prevPage)
            offsetFromPage = y - prevPage;
        frame->copySettings(f);
        frame->shape()->setPosition(QPointF(frame->shape()->position().x(),
                                            offsetInDocument + offsetFromPage));
    }
}

KWFrame *KWFrameLayout::createCopyFrame(KWFrameSet *fs, const KWPage &page)
{
    Q_ASSERT(page.isValid());
    if (fs->frameCount() == 0) { // special case for the headers. Just return a new textframe.
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        Q_ASSERT(tfs); // an empty, non-text frameset asking for a copy? Thats a bug.
        KoShape *shape = createTextShape(page);
        shape->setSize(QSize(20, 10));
        KWTextFrame *frame = new KWTextFrame(shape, tfs);

        return frame;
    }

    KoShape *orig = 0;
    foreach (KWFrame *candidate, fs->frames()) {
        if (candidate->isCopy())
            break;
        orig = candidate->shape();
    }
    Q_ASSERT(orig); // can't have a frameset with only copy frames.

    KWCopyShape *shape = new KWCopyShape(orig, m_pageManager);
    shape->setPosition(QPointF(0, page.offsetInDocument()));
    KWFrame *frame = new KWFrame(shape, fs);
    return frame;
}

KWTextFrameSet *KWFrameLayout::mainFrameSet() const
{
    const_cast<KWFrameLayout*>(this)->setup();
    return m_maintext;
}

void KWFrameLayout::mainframeRemoved(KWFrame *frame)
{
    // if a main-frame is removed we should remove all other auto-generated frames on that page to allow
    // the page to be removed totally.  Besides; we don't want to have a header when there is no main-frame on a page :)

    if (frame->shape() == 0) return;
    KWPage page = m_pageManager->page(frame->shape());
    if (!page.isValid()) return;

    QList<KWFrame*> framesToDelete;
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        if (!KWord::isAutoGenerated(tfs))
            continue;
        const bool isMainFs = fs == m_maintext;
        foreach (KWFrame *f, fs->frames()) {
            if (f == frame)
                continue;
            if (page == m_pageManager->page(f->shape())) {
                if (isMainFs) // there is another frame of the main text frameset on this page.
                    return;
                framesToDelete << f;
            }
        }
    }

    // delete them!
    foreach (KWFrame *frame, framesToDelete) {
        // first remove if from the frameset to make sure the doc gets a signal and removes the page if needed
        frame->frameSet()->removeFrame(frame);
        // then actually delete the frame itself.
        delete frame->shape();
    }
}

