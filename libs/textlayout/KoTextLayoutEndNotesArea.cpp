/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextLayoutEndNotesArea.h"

#include "FrameIterator.h"
#include "KoCharAreaInfo.h"
#include "KoInlineNote.h"
#include "KoInlineTextObjectManager.h"
#include "KoPointedAt.h"
#include "KoTextLayoutNoteArea.h"

#include <KoTextDocument.h>

#include <QPainter>

#include <algorithm>

static bool beforeThan(KoInlineNote *note1, KoInlineNote *note2)
{
    return (note1->getPosInDocument() < note2->getPosInDocument());
}

class Q_DECL_HIDDEN KoTextLayoutEndNotesArea::Private
{
public:
    Private()
        : startOfArea(nullptr)
    {
    }
    QList<KoTextLayoutNoteArea *> endNoteAreas;
    QList<QTextFrame *> endNoteFrames;
    FrameIterator *startOfArea;
    FrameIterator *endOfArea;
    int endNoteAutoCount;
};

KoTextLayoutEndNotesArea::KoTextLayoutEndNotesArea(KoTextLayoutArea *parent, KoTextDocumentLayout *documentLayout)
    : KoTextLayoutArea(parent, documentLayout)
    , d(new Private)
{
    d->endNoteAutoCount = 0;
}

KoTextLayoutEndNotesArea::~KoTextLayoutEndNotesArea()
{
    qDeleteAll(d->endNoteAreas);
    delete d;
}

bool KoTextLayoutEndNotesArea::layout(FrameIterator *cursor)
{
    qDeleteAll(d->endNoteAreas);
    d->endNoteAreas.clear();
    d->endNoteFrames.clear();

    d->startOfArea = new FrameIterator(cursor);
    d->endOfArea = nullptr;
    int shiftDown = 15;
    qreal y = top() + shiftDown;
    setBottom(y);

    KoInlineTextObjectManager *manager = KoTextDocument(documentLayout()->document()).inlineTextObjectManager();
    QList<KoInlineNote *> list = QList<KoInlineNote *>(manager->endNotes());
    std::sort(list.begin(), list.end(), beforeThan); // making a list of endnotes in the order they appear
    while (cursor->endNoteIndex < list.length()) {
        KoInlineNote *note = list[cursor->endNoteIndex];
        if (note->autoNumbering()) {
            note->setAutoNumber(d->endNoteAutoCount++);
        }
        QTextFrame *subFrame = note->textFrame();
        KoTextLayoutNoteArea *noteArea = new KoTextLayoutNoteArea(note, this, documentLayout());
        d->endNoteAreas.append(noteArea);
        d->endNoteFrames.append(subFrame);
        noteArea->setReferenceRect(left(), right(), y, maximumAllowedBottom());
        if (noteArea->layout(cursor->subFrameIterator(subFrame)) == false) {
            d->endOfArea = new FrameIterator(cursor);
            setBottom(noteArea->bottom());
            return false;
        }
        y = noteArea->bottom();
        setBottom(y);
        delete cursor->currentSubFrameIterator;
        cursor->currentSubFrameIterator = nullptr;
        cursor->endNoteIndex++;
    }
    if (cursor->endNoteIndex == 0) {
        setBottom(top() + shiftDown);
    }
    d->endOfArea = new FrameIterator(cursor);
    return true;
}
KoPointedAt KoTextLayoutEndNotesArea::hitTest(const QPointF &p, Qt::HitTestAccuracy accuracy) const
{
    KoPointedAt pointedAt;
    int endNoteIndex = 0;
    while (endNoteIndex < d->endNoteAreas.length()) {
        // check if p is over end notes area
        if (p.y() > d->endNoteAreas[endNoteIndex]->top() && p.y() < d->endNoteAreas[endNoteIndex]->bottom()) {
            pointedAt = d->endNoteAreas[endNoteIndex]->hitTest(p, accuracy);
            return pointedAt;
        }
        ++endNoteIndex;
    }
    return KoPointedAt();
}

QVector<KoCharAreaInfo> KoTextLayoutEndNotesArea::generateCharAreaInfos() const
{
    QVector<KoCharAreaInfo> result;

    if (d->startOfArea == nullptr) { // We have not been layouted yet
        return result;
    }

    foreach (KoTextLayoutNoteArea *area, d->endNoteAreas) {
        result.append(area->generateCharAreaInfos());
    }

    return result;
}

QRectF KoTextLayoutEndNotesArea::selectionBoundingBox(QTextCursor &cursor) const
{
    QTextFrame *subFrame;
    int endNoteIndex = 0;
    while (endNoteIndex < d->endNoteFrames.length()) {
        subFrame = d->endNoteFrames[endNoteIndex];
        if (subFrame != nullptr) {
            if (cursor.selectionStart() >= subFrame->firstPosition() && cursor.selectionEnd() <= subFrame->lastPosition()) {
                return d->endNoteAreas[endNoteIndex]->selectionBoundingBox(cursor);
            }
            ++endNoteIndex;
        }
    }
    return QRectF();
}

void KoTextLayoutEndNotesArea::paint(QPainter *painter, const KoTextDocumentLayout::PaintContext &context)
{
    if (d->startOfArea == nullptr) // We have not been layouted yet
        return;

    if (!d->endNoteAreas.isEmpty()) {
        int left = 2;
        int right = 150;
        int shiftDown = 10;
        painter->drawLine(left, top() + shiftDown, right, top() + shiftDown);
    }
    foreach (KoTextLayoutNoteArea *area, d->endNoteAreas) {
        area->paint(painter, context);
    }
}
