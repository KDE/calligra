/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2010 by Nokia, Matus Hanzes
 * Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
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
#include <KoDocumentResourceManager.h>

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
    virtual void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext)
    {
        if (background()) {
            applyConversion(painter, converter);
            QPainterPath p;
            p.addRect(QRectF(QPointF(), size()));
            background()->paint(painter, converter, paintContext, p);
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
    int columns = page.pageStyle().columns().count;
    Q_ASSERT(columns >= 1);
    KWTextFrameSet *fs = getOrCreate(Words::MainTextFrameSet, page);
    QRectF rect(QPointF(0, page.offsetInDocument()), QSizeF(page.width(), page.height()));

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

    layoutFramesOnPage(pageNumber);
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
       |  4              | <- pageStyle->footerDistance()
       |  5  [ footer ]  |
       |  6              | <- pageStyle->pageLayout()->bottomMargin + layout->bottomPadding
       +-----------------+ */

    // Create some data structures used for the layouting of the frames later
    int minZIndex = INT_MAX;
    qreal requestedHeight[7], minimumHeight[7], resultingPositions[7];
    for (int i = 0; i < 7; i++) { // zero fill.
        requestedHeight[i] = 0;
        minimumHeight[i] = 0;
        resultingPositions[i] = 0;
    }
    minimumHeight[0] = page.topMargin() + page.topPadding();
    minimumHeight[6] = page.bottomMargin() + page.bottomPadding();

    KoPageLayout layout = page.pageStyle().pageLayout();
    layout.leftMargin = page.leftMargin();
    layout.rightMargin = page.rightMargin();
    layout.leftPadding = page.leftPadding();
    layout.rightPadding = page.rightPadding();
    qreal left = 0, width = page.width();
    qreal textWidth = width - layout.leftMargin - layout.rightMargin
                            - layout.leftPadding - layout.rightPadding;

    KWPageStyle pageStyle = page.pageStyle();
    KoColumns columns = pageStyle.columns();
    int columnIndex = 0;
    KWFrame **main;
    KWFrame *footer = 0, *header = 0;
    KWFrame *pageBackground = 0;
    main = new KWFrame*[columns.count];
    if (columns.count > 0)
        main[0] = 0;
    QRectF pageRect(left, page.offsetInDocument(), width, page.height());
    QList<KWFrame *> frames = framesInPage(pageRect);

    kDebug(32001) << "pageNumber=" << pageNumber << "columns=" << columns.count << "frameCount=" << frames.count();
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
            minimumHeight[2] = pageStyle.headerDistance();
            minimumHeight[1] = qMax((qreal)10, pageStyle.headerMinimumHeight() - pageStyle.headerDistance());
            requestedHeight[1] = qMax(minimumHeight[1], textFrameSet->frames().first()->minimumFrameHeight());
            if (pageStyle.headerDynamicSpacing()) {
                minimumHeight[2] = qMax((qreal)0, minimumHeight[1] - requestedHeight[1]);
            }
            break;
        }
        case Words::OddPagesFooterTextFrameSet:
        case Words::EvenPagesFooterTextFrameSet: {
            footer = frame;
            minimumHeight[4] = pageStyle.footerDistance();
            minimumHeight[5] = qMax((qreal)10, pageStyle.footerMinimumHeight() - pageStyle.footerDistance());
            requestedHeight[5] = qMax(minimumHeight[5], textFrameSet->frames().first()->minimumFrameHeight());
            if (pageStyle.headerDynamicSpacing()) {
                minimumHeight[4] = qMax((qreal)0, minimumHeight[5] - requestedHeight[5]);
            }
            break;
        }
        case Words::MainTextFrameSet: {
            if (columnIndex == columns.count) {
                kWarning(32001) << "Too many columns present on page, ignoring 1, columns.count=" << columns.count;
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
    for (int i = 0; i < columns.count; ++i) {
        Q_ASSERT_X(main[i], __FUNCTION__, QString("No KWFrame for column=%1 columnCount=%2").arg(i).arg(columns.count).toLocal8Bit());
        Q_ASSERT_X(main[i]->shape(), __FUNCTION__, QString("No TextShape in KWFrame for column=%1 columnCount=%2").arg(i).arg(columns.count).toLocal8Bit());
        if (main[i] && main[i]->shape())
            main[i]->shape()->setZIndex(minZIndex);
    }
    if (footer && footer->shape()) {
        footer->shape()->setZIndex(minZIndex);
        // Make us compatible with ms word (seems saner too). Compatible with LO would be 0
        footer->shape()->setRunThrough(-3); //so children will be <= -2 and thus below main text
    }
    if (header && header->shape()) {
        header->shape()->setZIndex(minZIndex);
        // Make us compatible with ms word (seems saner too). Compatible with LO would be 0
        header->shape()->setRunThrough(-3); //so children will be <= -2 and thus below main text
    }

    if (pageBackground) {
        KoShape *bs = pageBackground->shape();
        bs->setRunThrough(-10); //so it will be below everything
        bs->setZIndex(--minZIndex);
        bs->setSize(pageRect.size());
        bs->setPosition(pageRect.topLeft());
    }

    // spread space across items.
    qreal heightLeft = page.height();
    for (int i = 0; i < 7; i++)
        heightLeft -= qMax(minimumHeight[i], requestedHeight[i]);
    if (heightLeft >= 0) { // easy; plenty of space
        minimumHeight[3] += heightLeft; // add space to main text frame
        qreal y = page.offsetInDocument();
        for (int i = 0; i < 7; i++) {
            resultingPositions[i] = y;
            y += qMax(minimumHeight[i], requestedHeight[i]);
        }
    } else {
        // for situations where the header + footer are too big to fit together with a
        // minimum sized main text frame.
        heightLeft = page.height();
        for (int i = 0; i < 7; i++)
            heightLeft -= minimumHeight[i];
        qreal y = page.offsetInDocument();
        for (int i = 0; i < 7; i++) {
            resultingPositions[i] = y;
            qreal row = minimumHeight[i];
            if (requestedHeight[i] > row) {
                row += heightLeft / 3;
            }
            y += row;
        }
    }

    // actually move / size the frames.
    if (columns.count > 0 && main[0]) {
        const qreal fullColumnHeight = resultingPositions[4] - resultingPositions[3];
        const qreal columnsXOffset = left + layout.leftMargin + layout.leftPadding;

        QRectF *columnRects = new QRectF[columns.count];
        // uniform columns?
        if (columns.columnData.isEmpty()) {
            const qreal columnWidth = (textWidth - columns.gapWidth * (columns.count - 1)) / columns.count;
            const qreal columnStep = columnWidth + columns.gapWidth;

            for (int i = 0; i < columns.count; i++) {
                columnRects[i] = QRectF(
                    columnsXOffset + columnStep * i,
                    resultingPositions[3],
                    columnWidth,
                    fullColumnHeight);
            }
        } else {
            qreal totalRelativeWidth = 0.0;
            foreach(const KoColumns::ColumnDatum &cd, columns.columnData) {
                totalRelativeWidth += cd.relativeWidth;
            }
            int relativeColumnXOffset = 0;
            for (int i = 0; i < columns.count; i++) {
                const KoColumns::ColumnDatum &columnDatum = columns.columnData.at(i);
                const qreal columnWidth = textWidth * columnDatum.relativeWidth / totalRelativeWidth;
                const qreal columnXOffset = textWidth * relativeColumnXOffset / totalRelativeWidth;

                columnRects[i] = QRectF(
                    columnsXOffset + columnXOffset + columnDatum.leftMargin,
                    resultingPositions[3] + columnDatum.topMargin,
                    columnWidth - columnDatum.leftMargin - columnDatum.rightMargin,
                    fullColumnHeight - columnDatum.topMargin - columnDatum.bottomMargin);

                relativeColumnXOffset += columnDatum.relativeWidth;
            }
        }
        // what is this doing? Friedrich
        for (int i = 0; i < columns.count; i++) {
            for (int f = 0; f < columns.count; f++) {
                if (f == i) continue;
                if (qAbs(main[f]->shape()->position().x() - columnRects[i].x()) < 10.0) {
                    qSwap(main[f], main[i]);
                    break;
                }
            }
        }

        for (int i = columns.count - 1; i >= 0; i--) {
            main[i]->setFrameBehavior(Words::AutoCreateNewFrameBehavior);
            main[i]->setNewFrameBehavior(Words::ReconnectNewFrame);
            KoShape *shape = main[i]->shape();
            shape->setPosition(columnRects[i].topLeft());
            shape->setSize(columnRects[i].size());
        }
        delete[] columnRects;

        // We need to store the content rect so layout can place it's anchored shapes
        // correctly
        page.setContentRect(QRectF(QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[3]), QSizeF(textWidth ,resultingPositions[4] - resultingPositions[3])));
    }
    if (header) {
        header->shape()->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[1]));
        header->shape()->setSize(QSizeF(textWidth, resultingPositions[2] - resultingPositions[1]));
    }
    if (footer) {
        footer->shape()->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[5]));
        footer->shape()->setSize(QSizeF(textWidth, resultingPositions[6] - resultingPositions[5]));
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
            *origin = header ? Words::EvenPagesHeaderTextFrameSet : Words::EvenPagesFooterTextFrameSet;
            break;
        case KWPageStyle::RightPages:
            *origin = header ? Words::OddPagesHeaderTextFrameSet : Words::OddPagesFooterTextFrameSet;
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
    KoDocumentResourceManager *rm = 0;
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
