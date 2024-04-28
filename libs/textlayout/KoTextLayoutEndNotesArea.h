/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTLAYOUTENDNOTESAREA_H
#define KOTEXTLAYOUTENDNOTESAREA_H

#include "kotextlayout_export.h"

#include "KoTextLayoutArea.h"

class QRectF;

/**
 * When laying out text it happens in areas that can occupy space of various size.
 */
class KOTEXTLAYOUT_EXPORT KoTextLayoutEndNotesArea : public KoTextLayoutArea
{
public:
    /// constructor
    explicit KoTextLayoutEndNotesArea(KoTextLayoutArea *parent, KoTextDocumentLayout *documentLayout);
    ~KoTextLayoutEndNotesArea() override;

    /// Layouts as much as it can
    /// Returns true if it has reached the end of the frame
    bool layout(FrameIterator *cursor);

    KoPointedAt hitTest(const QPointF &p, Qt::HitTestAccuracy accuracy) const;

    QRectF selectionBoundingBox(QTextCursor &cursor) const;

    QVector<KoCharAreaInfo> generateCharAreaInfos() const;

    void paint(QPainter *painter, const KoTextDocumentLayout::PaintContext &context);

private:
    class Private;
    Private *const d;
};

#endif
