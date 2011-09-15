/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2010 by Nokia, Matus Hanzes
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
#include "KWPageStyle.h"
#include "KWPage.h"
#include "KWCopyShape.h"
#include "KWDocument.h"
#include "Words.h"

#include <KoShapeRegistry.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeBackground.h>

#include <QTextLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <kdebug.h>
#include <limits.h>

class KWPageBackground : public KoShape
{
public:
    KWPageBackground()
    {
        setSelectable(false);
        setTextRunAroundSide(KoShape::RunThrough, KoShape::Background);
    }
    virtual ~KWPageBackground()
    {
    }
    virtual void paint(QPainter &painter, const KoViewConverter &converter)
    {
        if (background()) {
            applyConversion(painter, converter);
            QPainterPath p;
            p.addRect(QRectF(QPointF(), size()));
            background()->paint(painter, p);
        }
    }
    virtual bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &)
    {
        return true;
    }
    virtual void saveOdf(KoShapeSavingContext &) const
    {
    }
};

KWFrameLayout::KWFrameLayout(const KWPageManager *pageManager, const QList<KWFrameSet*> &frameSets)
        : m_pageManager(pageManager),
        m_frameSets(frameSets),
        m_maintext(0),
        m_backgroundFrameSet(0),
        m_document(0),
        m_setup(false)
{
}

void KWFrameLayout::createNewFramesForPage(int pageNumber)
{
    kDebug(32001) << "pageNumber=" << pageNumber;

    m_setup = false; // force reindexing of types
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());

    // Header footer handling.
    // first make a list of all types.
    QList<Words::TextFrameSetType> allHFTypes;
    allHFTypes.append(Words::OddPagesHeaderTextFrameSet);
    allHFTypes.append(Words::EvenPagesHeaderTextFrameSet);
    allHFTypes.append(Words::OddPagesFooterTextFrameSet);
    allHFTypes.append(Words::EvenPagesFooterTextFrameSet);

    // create headers & footers
    Words::TextFrameSetType origin;
    if (shouldHaveHeaderOrFooter(pageNumber, true, &origin)) {
        allHFTypes.removeAll(origin);
        KWTextFrameSet *fs = getOrCreate(origin, page);
        kDebug(32001) << "HeaderTextFrame" << fs << "frameOn=" << frameOn(fs, pageNumber) << "pageStyle=" << page.pageStyle().name();
        if (!frameOn(fs, pageNumber)) {
            createCopyFrame(fs, page);
        }
    }

    if (shouldHaveHeaderOrFooter(pageNumber, false, &origin)) {
        allHFTypes.removeAll(origin);
        KWTextFrameSet *fs = getOrCreate(origin, page);
        kDebug(32001) << "FooterTextFrame" << fs << "frameOn=" << frameOn(fs, pageNumber) << "pageStyle=" << page.pageStyle().name();
        if (!frameOn(fs, pageNumber)) {
            createCopyFrame(fs, page);
        }
    }

    //kDebug(32001) <<"createNewFramesForPage" << pageNumber << "TextFrameSetType=" << Words::frameSetTypeName(origin);

    if (page.pageStyle().background()) {
        // create page background
        if (!m_backgroundFrameSet) {
            m_backgroundFrameSet = new KWFrameSet(Words::BackgroundFrameSet);
            m_backgroundFrameSet->setName("backgroundFrames");
            emit newFrameSet(m_backgroundFrameSet);
            Q_ASSERT(m_frameSets.contains(m_backgroundFrameSet)); // the emit should have made that so :)
        }
        KWFrame *background = frameOn(m_backgroundFrameSet, pageNumber);
        if (background == 0) {
            KoShape *shape = new KWPageBackground();
            shape->setPosition(QPointF(0, page.offsetInDocument()));
            background = new KWFrame(shape, m_backgroundFrameSet);
            shape->setTextRunAroundSide(KoShape::RunThrough);
            // TODO pagespread
        }
        background->shape()->setBackground(page.pageStyle().background());
    } else {
        // check if there is a frame, if so, delete it, we don't need it.
        KWFrame *background = frameOn(m_backgroundFrameSet, pageNumber);
        if (background)
            delete background->shape();
    }

    // delete headers/footer frames that are not needed on this page
    foreach (KWFrame *frame, framesInPage(page.rect())) {
        if (frame->frameSet()->type() != Words::TextFrameSet)
            continue;
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
        if (tfs && (allHFTypes.contains(tfs->textFrameSetType())
                || (tfs->pageStyle() != page.pageStyle() && Words::isHeaderFooter(tfs)))) {
            Q_ASSERT(frame->shape());
            KWPage p = m_pageManager->page(frame->shape());
            Q_ASSERT(p.isValid());
            kDebug(32001)<<"Delete disabled header/footer frame=" << frame << "pageRect=" << page.rect() << "pageNumber=" << p.pageNumber();
            tfs->removeFrame(frame);
            delete frame->shape();
        }
    }

    // create main text frame. All columns of them.
    if (page.pageStyle().hasMainTextFrame()) {
        int columns = page.pageStyle().columns().columns;
        Q_ASSERT(columns >= 1);
        KWTextFrameSet *fs = getOrCreate(Words::MainTextFrameSet, page);
        QRectF rect(QPointF(0, page.offsetInDocument()),
                    QSizeF(page.width(), page.height()));

#if 0
        if (page.pageSide() == KWPage::PageSpread)
            rect.setWidth(rect.width() / 2);
#endif
        kDebug(32001) << "MainTextFrame" << fs << "pageRect=" << rect << "columns=" << columns;
        foreach (KWFrame *frame, framesInPage(rect)) {
            if (frame->frameSet() == fs) {
                columns--;
                if (columns < 0) {
                    kDebug(32001) << "Deleting KWFrame from MainTextFrame";
                    fs->removeFrame(frame);
                    delete frame->shape();
                }
            }
        }

        qreal colwidth = page.pageStyle().pageLayout().width / columns;
        qreal colheight = page.pageStyle().pageLayout().height;
        //for (--columns; columns >= 0; --columns) {
        for (int c = 0; c < columns; ++c) {
            kDebug(32001) << "Creating KWFrame for MainTextFrame";
            KoShape * shape = createTextShape(page);
            shape->setPosition(QPoint(c * colwidth, page.offsetInDocument()));
            shape->setSize(QSizeF(colwidth, colheight));
            new KWFrame(shape, fs);
        }
    }
#if 0
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
                if (fs->textFrameSetType() == Words::MainTextFrameSet) {
                    KoShape * shape = m_parent->createTextShape(page);
                    shape->setPosition(QPointF(page.width() / 2 + 1, shape->position().y()));
                    shape->setSize(QSizeF(page.pageStyle().pageLayout().width, page.pageStyle().pageLayout().height));
                    frame = new KWTextFrame(shape, fs);
                } else {
                    frame = m_parent->createCopyFrame(fs, page);
                    frame->shape()->setPosition(QPointF(page.width() / 2 + 1, frame->shape()->position().y()));
                    frame->shape()->setSize(QSizeF(page.pageStyle().pageLayout().width, page.pageStyle().pageLayout().height));
                }
            }
            KWFrameLayout *m_parent;
        };
        PageSpreadShapeFactory factory(this);
#if 0
        if (shouldHaveHeaderOrFooter(pageNumber + 1, true, &origin)) {
            KWTextFrameSet *fs = getOrCreate(origin, m_pageManager->page(pageNumber + 1));
            if (!frameOn(fs, pageNumber + 1))
                factory.create(page, fs);
        }
        if (shouldHaveHeaderOrFooter(pageNumber + 1, false, &origin)) {
            KWTextFrameSet *fs = getOrCreate(origin, m_pageManager->page(pageNumber + 1));
            if (!frameOn(fs, pageNumber + 1))
                factory.create(page, fs);
        }
#else
    #ifdef __GNUC__
        #warning FIXME: port to textlayout-rework
    #endif
#endif
        if (page.pageStyle().hasMainTextFrame()) {
            int columns = page.pageStyle().columns().columns;
            KWTextFrameSet *fs = getOrCreate(Words::MainTextFrameSet, page);
            QRectF rect(QPointF(page.width(), page.offsetInDocument()),
                        QSizeF(page.width() / 2,  page.height()));
#if 0
            foreach (KWFrame *frame, framesInPage(rect)) {
                if (frame->frameSet() == fs) {
                    columns--;
                    if (columns < 0) {
                        fs->removeFrame(frame);
                        delete frame;
                    }
                }
            }
#endif
            while (columns > 0) {
                factory.create(page, fs);
                columns--;
            }
        }
    }
#endif

#if 0
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
            if (!(frame->newFrameBehavior() == Words::ReconnectNewFrame ||
                    frame->newFrameBehavior() == Words::CopyNewFrame))
                continue;
            if (Words::isAutoGenerated(frame->frameSet()))
                continue; // these are copied above already.
            KWFrame *f = 0;
            KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(frame->frameSet());
            if (tfs && frame->newFrameBehavior() != Words::CopyNewFrame) {
                f = new KWTextFrame(createTextShape(page), tfs);
            } else {
                Q_ASSERT(frame->newFrameBehavior() == Words::CopyNewFrame);
                Q_ASSERT(frame->frameSet());
                Q_ASSERT(frame->frameSet()->frameCount() > 0);
                f = new KWFrame(new KWCopyShape(frame->frameSet()->frames().first()->shape(),
                            m_pageManager), frame->frameSet());
            }
            const qreal y = frame->shape()->position().y();
            qreal offsetFromPage = y - pageRect.top();
            f->copySettings(frame);
            f->shape()->setPosition(QPointF(frame->shape()->position().x(),
                                            page.offsetInDocument() + offsetFromPage));
        }
        odd = true;
    }
#endif

    layoutFramesOnPage(pageNumber);
#if 0
    if (page.pageSide() == KWPage::PageSpread)
        layoutFramesOnPage(pageNumber + 1);
#endif
}

void KWFrameLayout::layoutFramesOnPage(int pageNumber)
{
    /* assumes all frames are there and will do layouting of all the frames
        - headers/footers/main FS are positioned
        - normal frames are clipped to page */

    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    if (!page.isValid())
        return;

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
    int columnIndex = 0;
    KWFrame **main;
    KWFrame *footer = 0, *endnote = 0, *header = 0;
    KWFrame *pageBackground = 0;
    main = new KWFrame*[columnsCount];
    if (columns > 0)
        main[0] = 0;
    QRectF pageRect(left, page.offsetInDocument(), width, page.height());
    QList<KWFrame *> frames = framesInPage(pageRect);

    kDebug(32001) << "pageNumber=" << pageNumber << "hasMainTextFrame=" << pageStyle.hasMainTextFrame() << "columns=" << pageStyle.columns().columns << "frameCount=" << frames.count();
    foreach (KWFrame *frame, frames) {
        KWTextFrameSet *textFrameSet = 0;
        switch (frame->frameSet()->type()) {
        case Words::BackgroundFrameSet:
            pageBackground = frame;
            continue;
        case Words::TextFrameSet:
            textFrameSet = static_cast<KWTextFrameSet*>(frame->frameSet());
            if (textFrameSet->textFrameSetType() == Words::OtherTextFrameSet) {
                minZIndex = qMin(minZIndex, frame->shape()->zIndex());
                continue;
            }
            break;
        case Words::OtherFrameSet:
            minZIndex = qMin(minZIndex, frame->shape()->zIndex());
            continue;
        }
        Q_ASSERT(textFrameSet);

        /*
        KWPage page = m_pageManager->page(frame->shape());
        Q_ASSERT(page.isValid());
        kDebug(32001) << "textFrameSetType=" << Words::frameSetTypeName(textFrameSet->textFrameSetType())
                 << "page=" << page.pageNumber()
                 << "offset=" << page.offsetInDocument()
                 << "position=" << frame->shape()->position()
                 << "size=" << frame->shape()->size()
                 << "outlineRect=" << frame->shape()->outlineRect()
                 << "boundingRect=" << frame->shape()->boundingRect();
        */

        switch (textFrameSet->textFrameSetType()) {
        case Words::OddPagesHeaderTextFrameSet:
        case Words::EvenPagesHeaderTextFrameSet: {
            header = frame;
            minimumHeight[1] = qMax((qreal)10, pageStyle.headerMinimumHeight());
            minimumHeight[2] = pageStyle.headerDistance();
            requestedHeight[1] = qMax(pageStyle.headerMinimumHeight(), textFrameSet->frames().first()->minimumFrameHeight());
            break;
        }
        case Words::OddPagesFooterTextFrameSet:
        case Words::EvenPagesFooterTextFrameSet: {
            footer = frame;
            minimumHeight[7] = qMax((qreal)10, pageStyle.footerMinimumHeight());
            minimumHeight[6] = pageStyle.footerDistance();
            requestedHeight[7] = qMax(pageStyle.footerMinimumHeight(), textFrameSet->frames().first()->minimumFrameHeight());
            break;
        }
        case Words::MainTextFrameSet: {
            if (columnIndex == columnsCount) {
                kWarning(32001) << "Too many columns present on page, ignoring 1, columnsCount=" << columnsCount;
                break;
            }
            main[columnIndex] = frame;
            ++columnIndex;
            minimumHeight[3] = 10;
            // make at least one line fit lest we add endless pages.
            QTextLayout *layout = textFrameSet->document()->begin().layout();
            if (layout && layout->lineCount() > 0) {
                minimumHeight[3] = qMax((qreal) 10, layout->lineAt(0).height());
            }
            requestedHeight[3] = -1; // rest
            break;
        }
        default:;
        }
    }

    pageBackground = frameOn(m_backgroundFrameSet, pageNumber);

    --minZIndex;
    if (endnote) {
        endnote->shape()->setZIndex(minZIndex--);
    }
    for (int i = 0; i < columns; ++i) {
        Q_ASSERT_X(main[i], __FUNCTION__, QString("No KWFrame for column=%1 columnCount=%2").arg(i).arg(columns).toLocal8Bit());
        Q_ASSERT_X(main[i]->shape(), __FUNCTION__, QString("No TextShape in KWFrame for column=%1 columnCount=%2").arg(i).arg(columns).toLocal8Bit());
        if (main[i] && main[i]->shape())
            main[i]->shape()->setZIndex(minZIndex);
    }
    if (footer && footer->shape()) {
        footer->shape()->setZIndex(--minZIndex);
    }
    if (header && header->shape()) {
        header->shape()->setZIndex(--minZIndex);
    }

    if (pageBackground) {
        KoShape *bs = pageBackground->shape();
        bs->setZIndex(--minZIndex);
        bs->setSize(pageRect.size());
        bs->setPosition(pageRect.topLeft());
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
            main[i]->setFrameBehavior(Words::AutoCreateNewFrameBehavior);
            main[i]->setNewFrameBehavior(Words::ReconnectNewFrame);
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
}

bool KWFrameLayout::shouldHaveHeaderOrFooter(int pageNumber, bool header, Words::TextFrameSetType *origin)
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    KWPageStyle pagestyle = page.pageStyle();
    Words::HeaderFooterType type = header ? pagestyle.headerPolicy() : pagestyle.footerPolicy();
    switch (pagestyle.pageUsage()) {
        case KWPageStyle::MirroredPages:
        case KWPageStyle::AllPages: {
            switch (type) {
                case Words::HFTypeNone:
                    return false;
                case Words::HFTypeEvenOdd:
                    if (header)
                        *origin = pageNumber % 2 == 0 ? Words::EvenPagesHeaderTextFrameSet : Words::OddPagesHeaderTextFrameSet;
                    else
                        *origin = pageNumber % 2 == 0 ? Words::EvenPagesFooterTextFrameSet : Words::OddPagesFooterTextFrameSet;
                    break;
                case Words::HFTypeUniform:
                    *origin = header ? Words::OddPagesHeaderTextFrameSet : Words::OddPagesFooterTextFrameSet;
                    break;
            }
        } break;
        case KWPageStyle::LeftPages:
            *origin = Words::EvenPagesHeaderTextFrameSet;
            break;
        case KWPageStyle::RightPages:
            *origin = Words::OddPagesHeaderTextFrameSet;
            break;
    }
    return true;
}

QList<KWFrame *> KWFrameLayout::framesInPage(int pageNumber) const
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    return framesInPage(page.rect());
}

QList<KWFrame *> KWFrameLayout::framesInPage(const QRectF &page) const
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

KWTextFrameSet *KWFrameLayout::getOrCreate(Words::TextFrameSetType type, const KWPage &page)
{
    Q_ASSERT(page.isValid());
    setup();
    FrameSets frameSets = m_pageStyles.value(page.pageStyle());
    KWTextFrameSet **answer = 0;
    switch (type) {
    case Words::OddPagesHeaderTextFrameSet:
        answer = &frameSets.oddHeaders;
        break;
    case Words::EvenPagesHeaderTextFrameSet:
        answer = &frameSets.evenHeaders;
        break;
    case Words::OddPagesFooterTextFrameSet:
        answer = &frameSets.oddFooters;
        break;
    case Words::EvenPagesFooterTextFrameSet:
        answer = &frameSets.evenFooters;
        break;
    case Words::MainTextFrameSet:
        answer = &m_maintext;
        break;
    default:
        Q_ASSERT(false); // we should never get asked for 'other'
    }
    Q_ASSERT(answer);

    // The frameset wasn't created yet what can happen if for example a file is
    // loaded that does not exist or just does not create the required framesets.
    if (*answer == 0) {
        KWTextFrameSet *newFS = new KWTextFrameSet(m_document, type);
        *answer = newFS;
        if (type != Words::MainTextFrameSet) {
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

    KWTextFrameSet *oldMainText = m_maintext;
    m_maintext = 0;
    m_backgroundFrameSet = 0;
    m_pageStyles.clear();
    foreach (KWFrameSet *fs, m_frameSets) {
        if (fs->type() == Words::BackgroundFrameSet) {
            m_backgroundFrameSet = fs;
        } else if (fs->type() == Words::TextFrameSet) {
            KWTextFrameSet *tfs = static_cast<KWTextFrameSet*>(fs);
            FrameSets frameSets = m_pageStyles.value(tfs->pageStyle());
            switch (tfs->textFrameSetType()) {
            case Words::OddPagesHeaderTextFrameSet:
                frameSets.oddHeaders = tfs;
                break;
            case Words::EvenPagesHeaderTextFrameSet:
                frameSets.evenHeaders = tfs;
                break;
            case Words::OddPagesFooterTextFrameSet:
                frameSets.oddFooters = tfs;
                break;
            case Words::EvenPagesFooterTextFrameSet:
                frameSets.evenFooters = tfs;
                break;
            case Words::MainTextFrameSet:
                Q_ASSERT(m_maintext == 0); // there can be only one!
                if (tfs != oldMainText) {
                    oldMainText = 0;
                    disconnect(tfs, SIGNAL(frameRemoved(KWFrame*)),
                            this, SLOT(mainframeRemoved(KWFrame*)));
                    connect(tfs, SIGNAL(frameRemoved(KWFrame*)),
                            this, SLOT(mainframeRemoved(KWFrame*)));
                }
                m_maintext = tfs;
            default: ;// ignore
            }
            if (tfs->pageStyle().isValid())
                m_pageStyles.insert(tfs->pageStyle(), frameSets);
        }
    }
    m_setup = true;
}

KoShape *KWFrameLayout::createTextShape(const KWPage &page)
{
    kDebug(32001) << "pageNumber=" << page.pageNumber();
    Q_ASSERT(page.isValid());
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
    if (!factory)
        return 0;
    KoResourceManager *rm = 0;
    if (m_document)
        rm = m_document->resourceManager();
    KoShape *shape = factory->createDefaultShape(rm);
    //Q_ASSERT(shape);
    return shape;
}

KWFrame *KWFrameLayout::frameOn(KWFrameSet *fs, int pageNumber) const
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    foreach (KWFrame *frame, framesInPage(page.rect())) {
        if (frame->frameSet() == fs)
            return frame;
    }
    return 0;
}

QList<KWFrame *> KWFrameLayout::framesOn(KWFrameSet *fs, int pageNumber) const
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    QList<KWFrame *> frames;
    foreach (KWFrame *frame, framesInPage(page.rect())) {
        if (frame->frameSet() == fs)
            frames.append(frame);
    }
    return frames;
}

#if 0
void KWFrameLayout::cleanupHeadersFooters()
{
    kDebug(32001);
    QHash<KWPageStyle, FrameSets> pageStyles;
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0)
            continue;
        FrameSets frameSets = pageStyles.value(tfs->pageStyle());
        switch (tfs->textFrameSetType()) {
        case Words::OddPagesHeaderTextFrameSet:
            if (frameSets.oddHeaders) {
kDebug()<<"remove oddHeaders frameSets";
                emit removedFrameSet(frameSets.oddHeaders);
                delete frameSets.oddHeaders;
            }
            frameSets.oddHeaders = tfs;
            break;
        case Words::EvenPagesHeaderTextFrameSet:
            if (frameSets.evenHeaders) {
kDebug()<<"remove evenHeaders frameSets";
                emit removedFrameSet(frameSets.evenHeaders);
                delete frameSets.evenHeaders;
            }
            frameSets.evenHeaders = tfs;
            break;
        case Words::OddPagesFooterTextFrameSet:
            if (frameSets.oddFooters) {
kDebug()<<"remove oddFooters frameSets";
                emit removedFrameSet(frameSets.oddFooters);
                delete frameSets.oddFooters;
            }
            frameSets.oddFooters = tfs;
            break;
        case Words::EvenPagesFooterTextFrameSet:
            if (frameSets.evenFooters) {
kDebug()<<"remove evenFooters frameSets";
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
        case Words::HFTypeEvenOdd:
            break;
        case Words::HFTypeNone:
            cleanFrameSet(frameSets.oddHeaders);
            cleanFrameSet(frameSets.evenHeaders);
            break;
        case Words::HFTypeUniform:
            cleanFrameSet(frameSets.evenHeaders);
            break;
        }
        switch (style.footerPolicy()) {
        case Words::HFTypeEvenOdd:
            break;
        case Words::HFTypeNone:
            cleanFrameSet(frameSets.oddFooters);
            cleanFrameSet(frameSets.evenFooters);
            break;
        case Words::HFTypeUniform:
            cleanFrameSet(frameSets.evenFooters);
            break;
        }
    }
    m_setup = false;
}
#endif

void KWFrameLayout::cleanFrameSet(KWTextFrameSet *fs)
{
    kDebug(32001) << "frameSet=" << fs << "frameCount=" << (fs ? fs->frameCount() : 0);
    if (fs == 0)
        return;
    if (fs->frameCount() == 0)
        return;
    foreach (KWFrame *frame, fs->frames()) {
        fs->removeFrame(frame);
        delete frame->shape();
    }
}

#if 0
void KWFrameLayout::createNewFrameForPage(KWTextFrameSet *fs, int pageNumber)
{
    kDebug(32001);
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

    QList<KWFrame*> framesToDuplicate;
    QList<KWFrame*> frames = fs->frames();
    QList<KWFrame*>::Iterator iter = frames.end();
    while (iter != frames.begin()) {
        iter--;
        KWFrame *frame = static_cast<KWFrame*>(*iter);
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
    foreach (KWFrame *f, framesToDuplicate) {
        KWFrame *frame = new KWFrame(createTextShape(page), fs);
        const qreal y = f->shape()->position().y();
        qreal offsetFromPage = y - prevPage2;
        if (y > prevPage)
            offsetFromPage = y - prevPage;
        frame->copySettings(f);
        frame->shape()->setPosition(QPointF(frame->shape()->position().x(),
                                            offsetInDocument + offsetFromPage));
    }
    Q_ASSERT(false);
}
#endif

KWFrame *KWFrameLayout::createCopyFrame(KWFrameSet *fs, const KWPage &page)
{
    Q_ASSERT(page.isValid());
    kDebug(32001) << "frameSet=" << fs << "pageNumber=" << page.pageNumber() << "frameCount=" << fs->frameCount();
    if (fs->frameCount() == 0) { // special case for the headers. Just return a new textframe.
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        Q_ASSERT(tfs); // an empty, non-text frameset asking for a copy? Thats a bug.
        KoShape *shape = createTextShape(page);
        shape->setPosition(QPointF(0, page.offsetInDocument()));
        shape->setSize(QSize(20, 10));
        KWFrame *frame = new KWFrame(shape, tfs);
        return frame;
    }

    KoShape *orig = 0;
    //Lets find the last non-copy frame in the frameset
    for(int i = fs->frames().count() - 1; i >= 0; --i) {
        KWFrame *candidate = fs->frames()[i];
        if (!candidate->isCopy()) {
            orig = candidate->shape();
            break;
        }
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

QList<KWTextFrameSet*> KWFrameLayout::getFrameSets(const KWPageStyle &pageStyle) const
{
    FrameSets frameSets = m_pageStyles.value(pageStyle);
    QList<KWTextFrameSet*> result;
    result.append(m_maintext);
    result.append(frameSets.oddHeaders);
    result.append(frameSets.evenHeaders);
    result.append(frameSets.oddFooters);
    result.append(frameSets.evenFooters);
    result.append(frameSets.pageBackground);
    return result;
}

KWTextFrameSet* KWFrameLayout::getFrameSet(Words::TextFrameSetType type, const KWPageStyle &pageStyle) const
{
    FrameSets frameSets = m_pageStyles.value(pageStyle);
    switch (type) {
    case Words::OddPagesHeaderTextFrameSet:
        return frameSets.oddHeaders;
    case Words::EvenPagesHeaderTextFrameSet:
        return frameSets.evenHeaders;
    case Words::OddPagesFooterTextFrameSet:
        return frameSets.oddFooters;
    case Words::EvenPagesFooterTextFrameSet:
        return frameSets.evenFooters;
    case Words::MainTextFrameSet:
        return m_maintext;
    default:
        break;
    }
    return 0;
}

void KWFrameLayout::mainframeRemoved(KWFrame *frame)
{
    // if a main-frame is removed we should remove all other auto-generated frames on that page to allow
    // the page to be removed totally.  Besides; we don't want to have a header when there is no main-frame on a page :)

    if (frame->shape() == 0) return;
    KWPage page = m_pageManager->page(frame->shape());
    if (!page.isValid()) return;
    kDebug(32001) << "frame=" << frame << "frameSet=" << frame->frameSet() << "pageNumber=" << page.pageNumber();

    QList<KWFrame*> framesToDelete;
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        if (!tfs || !Words::isAutoGenerated(tfs))
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
        //frame->frameSet()->removeFrame(frame);
        // then actually delete the frame itself.
        delete frame->shape();
    }
}
