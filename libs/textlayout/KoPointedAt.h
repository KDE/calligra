/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann KO GmbH <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOPOINTEDAT_H
#define KOPOINTEDAT_H

#include "kotextlayout_export.h"

#include <QPointF>
#include <QString>
#include <QTextCursor>

class KoBookmark;
class QTextTable;
class KoInlineTextObjectManager;
class KoTextRangeManager;
class KoInlineNote;

class KOTEXTLAYOUT_EXPORT KoPointedAt
{
public:
    KoPointedAt();
    explicit KoPointedAt(KoPointedAt *other);

    void fillInLinks(const QTextCursor &cursor, KoInlineTextObjectManager *inlineManager, KoTextRangeManager *rangeManager);

    enum TableHit { None, ColumnDivider, RowDivider };
    int position;
    KoBookmark *bookmark;
    QString externalHRef;
    KoInlineNote *note;
    int noteReference;
    QTextTable *table;
    TableHit tableHit;
    int tableRowDivider;
    int tableColumnDivider;
    qreal tableLeadSize;
    qreal tableTrailSize;
    QPointF tableDividerPos;
};

#endif
