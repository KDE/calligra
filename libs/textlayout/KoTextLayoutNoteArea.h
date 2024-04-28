/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Brijesh Patel <brijesh3105@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTLAYOUTNOTEAREA_H
#define KOTEXTLAYOUTNOTEAREA_H

#include "KoTextLayoutArea.h"

#include <KoTextDocumentLayout.h>

class KoInlineNote;

class KOTEXTLAYOUT_EXPORT KoTextLayoutNoteArea : public KoTextLayoutArea
{
public:
    explicit KoTextLayoutNoteArea(KoInlineNote *note, KoTextLayoutArea *parent, KoTextDocumentLayout *documentLayout);
    ~KoTextLayoutNoteArea() override;

    void paint(QPainter *painter, const KoTextDocumentLayout::PaintContext &context);

    bool layout(FrameIterator *cursor);

    void setAsContinuedArea(bool isContinuedArea);

    KoPointedAt hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const;

    QRectF selectionBoundingBox(QTextCursor &cursor) const;

private:
    class Private;
    Private *const d;
};

#endif // KOTEXTLAYOUTNOTEAREA_H
