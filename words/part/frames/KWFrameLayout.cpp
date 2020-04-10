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
#include <KoDocumentResourceManager.h>
#include <KoShapeBackground.h>
#include <KoColumns.h>

#include <QPainterPath>
#include <QTextLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <WordsDebug.h>
#include <limits.h>

/**
 * This shape is a helper class for drawing the background of pages
 * and/or the separators between multiple columns.
 */
class KWPageBackground : public KoShape
{
public:
    KWPageBackground()
    {
        setSelectable(false);
        setTextRunAroundSide(KoShape::RunThrough, KoShape::Background);
    }
    ~KWPageBackground() override
    {
    }
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext) override
    {
        applyConversion(painter, converter);

        // paint background
        if (background()) {
            QPainterPath p;
            p.addRect(QRectF(QPointF(), size()));
            background()->paint(painter, converter, paintContext, p);
        }

        // paint separators
        if (! m_separatorPositions.isEmpty()) {
            QPen separatorPen(QBrush(m_separatorColor), m_separatorWidth,
                              Qt::PenStyle(m_separatorStyle), Qt::FlatCap);
            painter.setPen(separatorPen);
            foreach(qreal separatorPos, m_separatorPositions) {
                QLineF line(separatorPos, m_separatorY, separatorPos, m_separatorY+m_separatorHeight);
                painter.drawLine(line);
            }
        }
    }
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override
    {
        return true;
    }
    void saveOdf(KoShapeSavingContext &) const override
    {
    }

    void setSeparators(KoColumns::SeparatorStyle separatorStyle, const QColor &color,
                       const QList<qreal> &separatorPositions, qreal separatorY,
                       qreal separatorWidth, qreal separatorHeight)
    {
        m_separatorStyle = separatorStyle;
        m_separatorColor = color;
        m_separatorPositions = separatorPositions;
        m_separatorY = separatorY;
        m_separatorWidth = separatorWidth;
        m_separatorHeight = separatorHeight;
    }

    void clearSeparators()
    {
        m_separatorPositions.clear();
    }

private:
    KoColumns::SeparatorStyle m_separatorStyle;
    QColor m_separatorColor;

    QList<qreal> m_separatorPositions;
    qreal m_separatorY;
    /** Width in pt */
    qreal m_separatorWidth;
    qreal m_separatorHeight;
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
    debugWords << "pageNumber=" << pageNumber;

    m_setup = false; // force reindexing of types
    const KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    const KWPageStyle pageStyle = page.pageStyle();

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
        debugWords << "HeaderTextFrame" << fs << "sequencedShapeOn=" << sequencedShapeOn(fs, pageNumber) << "pageStyle=" << pageStyle.name();
        if (!sequencedShapeOn(fs, pageNumber)) {
            createCopyFrame(fs, page);
        }
    }

    if (shouldHaveHeaderOrFooter(pageNumber, false, &origin)) {
        allHFTypes.removeAll(origin);
        KWTextFrameSet *fs = getOrCreate(origin, page);
        debugWords << "FooterTextFrame" << fs << "sequencedShapeOn=" << sequencedShapeOn(fs, pageNumber) << "pageStyle=" << pageStyle.name();
        if (!sequencedShapeOn(fs, pageNumber)) {
            createCopyFrame(fs, page);
        }
    }

    //debugWords <<"createNewFramesForPage" << pageNumber << "TextFrameSetType=" << Words::frameSetTypeName(origin);

    // delete headers/footer frames that are not needed on this page
    foreach (KoShape *shape, sequencedShapesOnPage(page.rect())) {
        if (KWFrameSet::from(shape)->type() != Words::TextFrameSet)
            continue;
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(KWFrameSet::from(shape));
        if (tfs && (allHFTypes.contains(tfs->textFrameSetType())
                || (tfs->pageStyle() != pageStyle && Words::isHeaderFooter(tfs)))) {
            Q_ASSERT(shape);
            KWPage p = m_pageManager->page(shape);
            Q_ASSERT(p.isValid());
            debugWords<<"Delete disabled header/footer shape=" << shape << "pageRect=" << page.rect() << "pageNumber=" << p.pageNumber();
            tfs->removeShape(shape);
            delete shape;
        }
    }

    // create main text shape. All columns of them.
    const int columnsCount = pageStyle.columns().count;
    Q_ASSERT(columnsCount >= 1);
    KWTextFrameSet *fs = getOrCreate(Words::MainTextFrameSet, page);
    QRectF rect(QPointF(0, page.offsetInDocument()), QSizeF(page.width(), page.height()));

    debugWords << "MainTextFrame" << fs << "pageRect=" << rect << "columnsCount=" << columnsCount;
    int neededColumnsCount = columnsCount;
    // Check existing column shapes
    foreach (KoShape *shape, sequencedShapesOnPage(rect)) {
        if (KWFrameSet::from(shape) == fs) {
            --neededColumnsCount;
            if (neededColumnsCount < 0) {
                debugWords << "Deleting KWFrame from MainTextFrame";
                fs->removeShape(shape);
                delete shape;
            }
        }
    }
    const qreal colwidth = pageStyle.pageLayout().width / columnsCount;
    const qreal colheight = pageStyle.pageLayout().height;
    // Create missing column shapes
    for (int c = 0; c < neededColumnsCount; ++c) {
        debugWords << "Creating KWFrame for MainTextFrame";
        KoShape * shape = createTextShape(page);
        shape->setPosition(QPoint(c * colwidth+10.0, page.offsetInDocument()+10.0));
        shape->setSize(QSizeF(colwidth, colheight));
        new KWFrame(shape, fs);
    }

    if (pageStyle.background() || (columnsCount > 1)) {
        // create page background
        if (!m_backgroundFrameSet) {
            m_backgroundFrameSet = new KWFrameSet(Words::BackgroundFrameSet);
            m_backgroundFrameSet->setName("backgroundFrames");
            emit newFrameSet(m_backgroundFrameSet);
            Q_ASSERT(m_frameSets.contains(m_backgroundFrameSet)); // the emit should have made that so :)
        }
        KoShape *background = sequencedShapeOn(m_backgroundFrameSet, pageNumber);
        if (background == 0) {
            background = new KWPageBackground();
            background->setPosition(QPointF(0, page.offsetInDocument()));
            new KWFrame(background, m_backgroundFrameSet);
            background->setTextRunAroundSide(KoShape::RunThrough);
        }
        background->setBackground(pageStyle.background());
    } else {
        // check if there is a frame, if so, delete it, we don't need it.
        KoShape *background = sequencedShapeOn(m_backgroundFrameSet, pageNumber);
        if (background)
            delete background;
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
    const qreal left = 0, width = page.width();
    const qreal textWidth = width - layout.leftMargin - layout.rightMargin
                                  - layout.leftPadding - layout.rightPadding;

    KWPageStyle pageStyle = page.pageStyle();
    KoColumns columns = pageStyle.columns();
    int columnIndex = 0;
    KoShape **main;
    KoShape *footer = 0, *header = 0;
    KoShape *pageBackground = 0;
    main = new KoShape*[columns.count];
    if (columns.count > 0)
        main[0] = 0;
    QRectF pageRect(left, page.offsetInDocument(), width, page.height());
    QList<KoShape *> shapes = sequencedShapesOnPage(pageRect);

    debugWords << "pageNumber=" << pageNumber << "columns=" << columns.count << "shapeCount=" << shapes.count();
    foreach (KoShape *shape, shapes) {
        KWTextFrameSet *textFrameSet = 0;
        switch (KWFrameSet::from(shape)->type()) {
        case Words::BackgroundFrameSet:
            pageBackground = shape;
            continue;
        case Words::TextFrameSet:
            textFrameSet = static_cast<KWTextFrameSet*>(KWFrameSet::from(shape));
            if (textFrameSet->textFrameSetType() == Words::OtherTextFrameSet) {
                minZIndex = qMin(minZIndex, shape->zIndex());
                continue;
            }
            break;
        case Words::OtherFrameSet:
            minZIndex = qMin(minZIndex, shape->zIndex());
            continue;
        }
        Q_ASSERT(textFrameSet);

        /*
        KWPage page = m_pageManager->page(frame->shape());
        Q_ASSERT(page.isValid());
        debugWords << "textFrameSetType=" << Words::frameSetTypeName(textFrameSet->textFrameSetType())
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
            header = shape;
            minimumHeight[2] = pageStyle.headerDistance();
            minimumHeight[1] = qMax((qreal)10, pageStyle.headerMinimumHeight() - pageStyle.headerDistance());
            requestedHeight[1] = qMax(minimumHeight[1], textFrameSet->shapes().first()->minimumHeight());
            if (pageStyle.headerDynamicSpacing()) {
                minimumHeight[2] = qMax((qreal)0, minimumHeight[1] - requestedHeight[1]);
            }
            break;
        }
        case Words::OddPagesFooterTextFrameSet:
        case Words::EvenPagesFooterTextFrameSet: {
            footer = shape;
            minimumHeight[4] = pageStyle.footerDistance();
            minimumHeight[5] = qMax((qreal)10, pageStyle.footerMinimumHeight() - pageStyle.footerDistance());
            requestedHeight[5] = qMax(minimumHeight[5], textFrameSet->shapes().first()->minimumHeight());
            if (pageStyle.footerDynamicSpacing()) {
                minimumHeight[4] = qMax((qreal)0, minimumHeight[5] - requestedHeight[5]);
            }
            break;
        }
        case Words::MainTextFrameSet: {
            if (columnIndex == columns.count) {
                warnWords << "Too many columns present on page, ignoring 1, columns.count=" << columns.count;
                break;
            }
            main[columnIndex] = shape;
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

    pageBackground = sequencedShapeOn(m_backgroundFrameSet, pageNumber);

    --minZIndex;
    for (int i = 0; i < columns.count; ++i) {
        Q_ASSERT_X(main[i], __FUNCTION__, QString("No TextShape for column=%1 columnsCount=%2").arg(i).arg(columns.count).toLocal8Bit());
        if (main[i])
            main[i]->setZIndex(minZIndex);
    }
    if (footer) {
        footer->setZIndex(minZIndex);
        // Make us compatible with ms word (seems saner too). Compatible with LO would be 0
        footer->setRunThrough(-3); //so children will be <= -2 and thus below main text
    }
    if (header) {
        header->setZIndex(minZIndex);
        // Make us compatible with ms word (seems saner too). Compatible with LO would be 0
        header->setRunThrough(-3); //so children will be <= -2 and thus below main text
    }

    if (pageBackground) {
        pageBackground->setRunThrough(-10); //so it will be below everything
        pageBackground->setZIndex(--minZIndex);
    }

    // spread space across items.
    qreal heightLeft = page.height();
    for (int i = 0; i < 7; i++) {
        heightLeft -= qMax(minimumHeight[i], requestedHeight[i]);
    }
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
            const qreal totalRelativeWidth = columns.totalRelativeWidth();

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

        //make sure the order of shapes geometrically follows the textflow order
        for (int i = 0; i < columns.count; i++) {
            for (int f = 0; f < columns.count; f++) {
                if (f == i) continue;
                if (qAbs(main[f]->position().x() - columnRects[i].x()) < 10.0) {
                    qSwap(main[f], main[i]);
                    break;
                }
            }
        }

        // finally set size and position of the shapes
        for (int i = columns.count - 1; i >= 0; i--) {
            main[i]->setPosition(columnRects[i].topLeft());
            main[i]->setSize(columnRects[i].size());
        }

        delete[] columnRects;

        // We need to store the content rect so layout can place it's anchored shapes
        // correctly
        page.setContentRect(QRectF(QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[3]), QSizeF(textWidth ,resultingPositions[4] - resultingPositions[3])));
    }
    if (header) {
        header->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[1]));
        header->setSize(QSizeF(textWidth, resultingPositions[2] - resultingPositions[1]));
    }
    if (footer) {
        footer->setPosition(
            QPointF(left + layout.leftMargin + layout.leftPadding, resultingPositions[5]));
        footer->setSize(QSizeF(textWidth, resultingPositions[6] - resultingPositions[5]));
    }
    if (pageBackground) {
        pageBackground->setPosition(QPointF(left + layout.leftMargin, page.offsetInDocument() + layout.topMargin));
        pageBackground->setSize(QSizeF(width - layout.leftMargin - layout.rightMargin,
                                                page.height() - layout.topMargin - layout.bottomMargin));

        // set separator data
        KWPageBackground *bs = dynamic_cast<KWPageBackground *>(pageBackground);
        if (columns.count > 1) {
            const qreal fullColumnHeight = resultingPositions[4] - resultingPositions[3];

            QList<qreal> separatorXPositions;
            // uniform columns?
            if (columns.columnData.isEmpty()) {
                const qreal separatorXBaseOffset = layout.leftPadding - (columns.gapWidth * 0.5);
                const qreal columnWidth = (textWidth - columns.gapWidth * (columns.count - 1)) / columns.count;
                const qreal columnStep = columnWidth + columns.gapWidth;
                for (int i = 1; i < columns.count; ++i) {
                    separatorXPositions << separatorXBaseOffset + columnStep * i;
                }
            } else {
                const qreal totalRelativeWidth = columns.totalRelativeWidth();

                int relativeColumnXOffset = 0;
                for (int i = 0; i < columns.count-1; i++) {
                    const KoColumns::ColumnDatum &columnDatum = columns.columnData.at(i);
                    relativeColumnXOffset += columnDatum.relativeWidth;
                    const qreal columnXOffset = textWidth * relativeColumnXOffset / totalRelativeWidth;

                    separatorXPositions << layout.leftPadding + columnXOffset;
                }
            }
            const qreal separatorHeight = fullColumnHeight * columns.separatorHeight / 100.0;
            const qreal separatorY = layout.topPadding +
                ((columns.separatorVerticalAlignment == KoColumns::AlignBottom) ?
                    fullColumnHeight * (100 - columns.separatorHeight) / 100.0 :
                 (columns.separatorVerticalAlignment == KoColumns::AlignVCenter) ?
                    fullColumnHeight * (100 - columns.separatorHeight) / 200.0 :
                /* default: KoColumns::AlignTop */
                    0);
            bs->setSeparators(columns.separatorStyle, columns.separatorColor,
                              separatorXPositions, separatorY,
                              columns.separatorWidth, separatorHeight);
        } else {
            bs->clearSeparators();
        }
    }
    delete [] main;
}

void KWFrameLayout::proposeShapeMove(const KoShape *shape, QPointF &delta, const KWPage &page)
{
    KoShapeAnchor *anchor = shape->anchor();
    if (!anchor) {
        return; // nothing we can do
    }

    QRectF refRect;
    const qreal textWidth = page.width() - page.leftMargin() - page.rightMargin()
                                - page.leftPadding() - page.rightPadding();
    switch (anchor->horizontalRel()) {
        case KoShapeAnchor::HParagraph: // LO mistakenly saves it like this sometimes - stupid LO
        anchor->setHorizontalRel(KoShapeAnchor::HPage); // let's fix it
        // fall through
    case KoShapeAnchor::HPage:
        refRect.setX(0);
        refRect.setWidth(page.width());
        break;
    case KoShapeAnchor::HPageContent:
        refRect.setX(page.leftMargin() + page.leftPadding());
        refRect.setWidth(textWidth);
        break;
    case KoShapeAnchor::HPageStartMargin:
        refRect.setX(0);
        refRect.setRight(page.leftMargin() + page.leftPadding());
        break;
    case KoShapeAnchor::HPageEndMargin:
        refRect.setX(page.width() - page.rightMargin() - page.rightPadding());
        refRect.setRight(page.width());
        break;
    default:
        break;
    }
    switch (anchor->verticalRel()) {
    case KoShapeAnchor::VPage:
        refRect.setY(page.offsetInDocument());
        refRect.setHeight(page.height());
        break;
    case KoShapeAnchor::VPageContent:
        refRect.setY(page.contentRect().y());
        refRect.setHeight(page.contentRect().height());
        break;
    default:
        break;
    }
    QPointF newPos = shape->position() + delta;
    switch (anchor->horizontalPos()) {
    case KoShapeAnchor::HLeft:
        newPos.setX(refRect.x());
        break;
    case KoShapeAnchor::HCenter:
        newPos.setX(refRect.x() + (refRect.width() - shape->size().width()) / 2);
        break;
    case KoShapeAnchor::HRight:
        newPos.setX(refRect.right() - shape->size().width());
        break;
    default:
        break;
    }

    switch (anchor->verticalPos()) {
    case KoShapeAnchor::VTop:
        newPos.setY(refRect.y());
        break;
    case KoShapeAnchor::VMiddle:
        newPos.setY(refRect.y() + (refRect.height() - shape->size().height()) / 2);
        break;
    case KoShapeAnchor::VBottom:
        newPos.setY(refRect.bottom() - shape->size().height());
        break;
    default:
        break;
    }
    delta = newPos - shape->position();
}

bool KWFrameLayout::shouldHaveHeaderOrFooter(int pageNumber, bool header, Words::TextFrameSetType *origin)
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    KWPageStyle pagestyle = page.pageStyle();
    Words::HeaderFooterType type = header ? pagestyle.headerPolicy() : pagestyle.footerPolicy();
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
    return true;
}

QList<KoShape *> KWFrameLayout::sequencedShapesOnPage(int pageNumber) const
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    return sequencedShapesOnPage(page.rect());
}

QList<KoShape *> KWFrameLayout::sequencedShapesOnPage(const QRectF &page) const
{
    // hopefully replaced with a tree
    QList<KoShape *> answer;
    foreach (KWFrameSet *fs, m_frameSets) {
        foreach (KoShape *shape, fs->shapes()) {
            // use TopLeftCorner as main,header,footer are not rotated, also see bug 275288
            if (page.contains(shape->absolutePosition(KoFlake::TopLeftCorner)))
                answer.append(shape);
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
    // When framesets have been removed things needs to be updated
    if (m_setup && ((m_maintext && !m_frameSets.contains(m_maintext)) || (m_backgroundFrameSet && !m_frameSets.contains(m_backgroundFrameSet)))) {
        m_setup = false;
    }
    if (m_setup) {
        return;
    }
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
                    disconnect(tfs, SIGNAL(shapeRemoved(KoShape*)),
                            this, SLOT(mainShapeRemoved(KoShape*)));
                    connect(tfs, SIGNAL(shapeRemoved(KoShape*)),
                            this, SLOT(mainShapeRemoved(KoShape*)));
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
    debugWords << "pageNumber=" << page.pageNumber();
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

KoShape *KWFrameLayout::sequencedShapeOn(KWFrameSet *fs, int pageNumber) const
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    foreach (KoShape *shape, sequencedShapesOnPage(page.rect())) {
        if (KWFrameSet::from(shape) == fs)
            return shape;
    }
    return 0;
}

QList<KoShape *> KWFrameLayout::sequencedShapesOn(KWFrameSet *fs, int pageNumber) const
{
    KWPage page = m_pageManager->page(pageNumber);
    Q_ASSERT(page.isValid());
    QList<KoShape *> shapes;
    foreach (KoShape *shape, sequencedShapesOnPage(page.rect())) {
        if (KWFrameSet::from(shape) == fs)
            shapes.append(shape);
    }
    return shapes;
}

void KWFrameLayout::cleanFrameSet(KWTextFrameSet *fs)
{
    debugWords << "frameSet=" << fs << "shapeCount=" << (fs ? fs->shapeCount() : 0);
    if (fs == 0)
        return;
    if (fs->shapeCount() == 0)
        return;
    foreach (KWFrame *frame, fs->frames()) {
        fs->removeShape(frame->shape());
        delete frame->shape();
    }
}

KWFrame *KWFrameLayout::createCopyFrame(KWFrameSet *fs, const KWPage &page)
{
    Q_ASSERT(page.isValid());
    debugWords << "frameSet=" << fs << "pageNumber=" << page.pageNumber() << "shapeCount=" << fs->shapeCount();
    if (fs->shapeCount() == 0) { // special case for the headers. Just return a new textframe.
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        Q_ASSERT(tfs); // an empty, non-text frameset asking for a copy? Thats a bug.
        KoShape *shape = createTextShape(page);
        shape->setPosition(QPointF(10.0, page.offsetInDocument()+10.0));
        shape->setSize(QSize(20, 10));
        KWFrame *frame = new KWFrame(shape, tfs);
        return frame;
    }

    KoShape *orig = 0;
    //Lets find the last non-copy frame in the frameset
    for(int i = fs->shapeCount() - 1; i >= 0; --i) {
        KoShape *candidate = fs->shapes()[i];
        if (!dynamic_cast<KWCopyShape*>(candidate)) {
            orig = candidate;
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

void KWFrameLayout::mainShapeRemoved(KoShape *shape)
{
    // if a main-shape is removed we should remove all other auto-generated shapes on that page to allow
    // the page to be removed totally.  Besides; we don't want to have a header when there is no main-shape on a page :)

    KWPage page = m_pageManager->page(shape);
    if (!page.isValid()) return;
    debugWords << "shape=" << shape << "pageNumber=" << page.pageNumber();

    QList<KoShape *> shapesToDelete;
    foreach (KWFrameSet *fs, m_frameSets) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        if (!tfs || !Words::isAutoGenerated(tfs))
            continue;
        const bool isMainFs = fs == m_maintext;
        foreach (KoShape *s, fs->shapes()) {
            if (s == shape)
                continue;
            if (page == m_pageManager->page(s)) {
                if (isMainFs) // there is another shape of the main text frameset on this page.
                    return;
                shapesToDelete << s;
            }
        }
    }

    // delete them!
    foreach (KoShape *s, shapesToDelete) {
        // first remove if from the frameset to make sure the doc gets a signal and removes the page if needed
        //frame->frameSet()->removeFrame(frame);
        // then actually delete the frame itself.
        delete s;
    }
}
