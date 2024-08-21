/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008, 2011 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * SPDX-FileCopyrightText: 2008 Roopesh Chander <roop@forwardbias.in>
 * SPDX-FileCopyrightText: 2007-2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * SPDX-FileCopyrightText: 2009-2011 KO GmbH <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2009-2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2010 Nandita Suri <suri.nandita@gmail.com>
 * SPDX-FileCopyrightText: 2010 Ajay Pundhir <ajay.pratap@iiitb.net>
 * SPDX-FileCopyrightText: 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * SPDX-FileCopyrightText: 2011 Stuart Dickson <stuart@furkinfantasic.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTLAYOUTAREA_P_H
#define KOTEXTLAYOUTAREA_P_H

#include "KoTextLayoutEndNotesArea.h"
#include "KoTextLayoutNoteArea.h"
#include "KoTextLayoutTableArea.h"

#include <KoTextBlockBorderData.h>

// local type for temporary use in restartLayout
struct LineKeeper {
    int columns;
    qreal lineWidth;
    QPointF position;
};
Q_DECLARE_TYPEINFO(LineKeeper, Q_MOVABLE_TYPE);

class Q_DECL_HIDDEN KoTextLayoutArea::Private
{
public:
    Private()
        : left(0.0)
        , right(0.0)
        , top(0.0)
        , bottom(0.0)
        , maximalAllowedBottom(0.0)
        , maximumAllowedWidth(0.0)
        , neededWidth(0.0)
        , isLayoutEnvironment(false)
        , actsHorizontally(false)
        , dropCapsWidth(0)
        , dropCapsDistance(0)
        , startOfArea(nullptr)
        , endOfArea(nullptr)
        , copyEndOfArea(nullptr)
        , footNoteCursorToNext(nullptr)
        , footNoteCursorFromPrevious(nullptr)
        , continuedNoteToNext(nullptr)
        , continuedNoteFromPrevious(nullptr)
        , footNoteCountInDoc(0)
        , acceptsPageBreak(false)
        , acceptsColumnBreak(false)
        , virginPage(true)
        , verticalAlignOffset(0)
        , preregisteredFootNotesHeight(0)
        , footNotesHeight(0)
        , footNoteAutoCount(0)
        , extraTextIndent(0)
        , endNotesArea(nullptr)
    {
    }

    KoTextLayoutArea *parent; //  A pointer to the parent

    KoTextDocumentLayout *documentLayout;

    qreal left; // reference area left
    qreal right; // reference area right
    qreal top; // reference area top
    qreal bottom; // reference area top
    qreal maximalAllowedBottom;
    qreal maximumAllowedWidth; // 0 indicates wrapping is allowed
    qreal neededWidth; // used in conjunction with grow-text-width
    QRectF boundingRect;
    bool isLayoutEnvironment;
    bool actsHorizontally;
    KoTextBlockBorderData *prevBorder;
    qreal prevBorderPadding;

    qreal x; // text area starts here as defined by margins (so not == left)
    qreal y;
    qreal width; // of text area as defined by margins (so not == right - left)
    qreal indent;
    qreal dropCapsWidth;
    qreal dropCapsDistance;
    int dropCapsNChars;
    bool isRtl;
    qreal bottomSpacing;
    QList<KoTextLayoutTableArea *> tableAreas;
    FrameIterator *startOfArea;
    FrameIterator *endOfArea;
    FrameIterator *copyEndOfArea;
    FrameIterator *footNoteCursorToNext;
    FrameIterator *footNoteCursorFromPrevious;
    KoInlineNote *continuedNoteToNext;
    KoInlineNote *continuedNoteFromPrevious;
    int footNoteCountInDoc;

    bool acceptsPageBreak;
    bool acceptsColumnBreak;
    bool virginPage;
    qreal verticalAlignOffset;
    QVector<QRectF> blockRects;
    qreal anchoringParagraphTop;
    qreal anchoringParagraphContentTop;

    qreal preregisteredFootNotesHeight;
    qreal footNotesHeight;
    int footNoteAutoCount;
    qreal extraTextIndent;
    QList<KoTextLayoutNoteArea *> preregisteredFootNoteAreas;
    QList<KoTextLayoutNoteArea *> footNoteAreas;
    QList<QTextFrame *> preregisteredFootNoteFrames;
    QList<QTextFrame *> footNoteFrames;
    KoTextLayoutEndNotesArea *endNotesArea;
    QList<KoTextLayoutArea *> generatedDocAreas;

    /// utility method to restart layout of a block
    QTextLine restartLayout(QTextBlock &block, int lineTextStartOfLastKeep);
    /// utility method to store remaining layout of a split block
    void stashRemainingLayout(QTextBlock &block, int lineTextStartOfFirstKeep, QVector<LineKeeper> &stashedLines, QPointF &stashedCounterPosition);
    /// utility method to recreate partial layout of a split block
    QTextLine recreatePartialLayout(QTextBlock &block, const QVector<LineKeeper> &stashedLines, QPointF &stashedCounterPosition, QTextLine &line);
};

#endif // KOTEXTLAYOUTAREA_P_H
