/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTLAYOUTTABLEAREA_H
#define KOTEXTLAYOUTTABLEAREA_H

#include "kotextlayout_export.h"

#include "KoTextLayoutArea.h"

#include <QVector>

class KoPointedAt;
class TableIterator;
class QTextTableCell;
class QLineF;

/**
 * This class represent a (full width) piece of a table
 */
class KOTEXTLAYOUT_EXPORT KoTextLayoutTableArea : public KoTextLayoutArea
{
public:
    /// constructor
    explicit KoTextLayoutTableArea(QTextTable *table, KoTextLayoutArea *parent, KoTextDocumentLayout *documentLayout);
    ~KoTextLayoutTableArea() override;

    /// Layouts as much as it can
    /// Returns true if it has reached the end of the table
    bool layoutTable(TableIterator *cursor);

    void paint(QPainter *painter, const KoTextDocumentLayout::PaintContext &context);

    KoPointedAt hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const;

    /// Calc a bounding box rect of the selection
    QRectF selectionBoundingBox(QTextCursor &cursor) const;

    QVector<KoCharAreaInfo> generateCharAreaInfos() const;

private:
    void layoutColumns();
    void collectBorderThicknesss(int row, qreal &topBorderWidth, qreal &bottomBorderWidth);
    void nukeRow(TableIterator *cursor);
    bool layoutRow(TableIterator *cursor, qreal topBorderWidth, qreal bottomBorderWidth);
    bool layoutMergedCellsNotEnding(TableIterator *cursor, qreal topBorderWidth, qreal bottomBorderWidth, qreal rowBottom);
    QRectF cellBoundingRect(const QTextTableCell &cell) const;
    void paintCell(QPainter *painter, const KoTextDocumentLayout::PaintContext &context, const QTextTableCell &tableCell, KoTextLayoutArea *frameArea);
    void paintCellBorders(QPainter *painter,
                          const KoTextDocumentLayout::PaintContext &context,
                          const QTextTableCell &tableCell,
                          bool topRow,
                          int maxRow,
                          QVector<QLineF> *accuBlankBorders);

    class Private;
    Private *const d;
};

#endif
