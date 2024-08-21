/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 C. Boemann KO GmbH <cbo@kogmbh.com>
 * SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPointedAt.h"

#include <KoInlineNote.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextRangeManager.h>

#include <TextLayoutDebug.h>

#include <QTextCursor>

KoPointedAt::KoPointedAt()
    : position(-1)
    , bookmark(nullptr)
    , note(nullptr)
    , noteReference(-1)
    , table(nullptr)
    , tableHit(None)
{
}

KoPointedAt::KoPointedAt(KoPointedAt *other)
{
    position = other->position;
    bookmark = other->bookmark;
    note = other->note;
    noteReference = other->noteReference;
    externalHRef = other->externalHRef;
    tableHit = other->tableHit;
    tableRowDivider = other->tableRowDivider;
    tableColumnDivider = other->tableColumnDivider;
    tableLeadSize = other->tableLeadSize;
    tableTrailSize = other->tableTrailSize;
    table = other->table;
}

void KoPointedAt::fillInLinks(const QTextCursor &cursor, KoInlineTextObjectManager *inlineManager, KoTextRangeManager *rangeManager)
{
    bookmark = nullptr;
    externalHRef.clear();
    note = nullptr;

    if (!inlineManager)
        return;

    // Is there an href here ?
    if (cursor.charFormat().isAnchor()) {
        QString href = cursor.charFormat().anchorHref();
        // local href starts with #
        if (href.startsWith('#')) {
            // however bookmark does not contain it, so strip it
            href = href.right(href.size() - 1);

            if (!href.isEmpty()) {
                bookmark = rangeManager->bookmarkManager()->bookmark(href);
            }
            return;
        } else {
            // Nope, then it must be external;
            externalHRef = href;
        }
    } else {
        note = dynamic_cast<KoInlineNote *>(inlineManager->inlineTextObject(cursor));
    }
}
