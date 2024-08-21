/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextRange.h"

#include "KoTextInlineRdf.h"
#include "KoTextRangeManager.h"

#include "TextDebug.h"
#include <QTextCursor>

class KoTextRangePrivate
{
public:
    KoTextRangePrivate()
        : manager(nullptr)
        , id(-1)
        , rdf(nullptr)
        , positionOnlyMode(true)
        , initialStartPosition(-1)
        , initialEndPosition(-1)
    {
    }
    virtual ~KoTextRangePrivate();

    KoTextRangeManager *manager;
    int id;
    QTextCursor cursor;
    KoTextInlineRdf *rdf; //< A textrange might have RDF, we own it.
    bool positionOnlyMode;
    int snapAnchor;
    int snapPos;
    QTextDocument *document;
    int initialStartPosition;
    int initialEndPosition;
};

KoTextRange::KoTextRange(const QTextCursor &cursor)
    : d(new KoTextRangePrivate)
{
    d->document = cursor.document();
    d->cursor = cursor;
    d->cursor.setPosition(cursor.selectionStart());
    d->cursor.setKeepPositionOnInsert(true);
    if (cursor.hasSelection()) {
        setRangeEnd(cursor.selectionEnd());
    }
}

KoTextRange::KoTextRange(QTextDocument *document, int position)
    : d(new KoTextRangePrivate)
{
    d->document = document;
    d->initialStartPosition = position;
    d->initialEndPosition = position;
}

bool KoTextRange::isFinalized() const
{
    return !d->cursor.isNull();
}

void KoTextRange::finalizePosition()
{
    if (!d->cursor.isNull())
        return;

    d->cursor = QTextCursor(d->document);
    d->cursor.setPosition(d->initialStartPosition);
    d->cursor.setKeepPositionOnInsert(true);
    if (d->initialEndPosition != d->initialStartPosition) {
        setRangeEnd(d->initialEndPosition);
    }
}

KoTextRangePrivate::~KoTextRangePrivate()
{
    delete rdf;
}

KoTextRange::~KoTextRange()
{
    if (d->manager) {
        d->manager->remove(this);
    }
    delete d;
    d = nullptr;
}

void KoTextRange::setManager(KoTextRangeManager *manager)
{
    d->manager = manager;
}

KoTextRangeManager *KoTextRange::manager() const
{
    return d->manager;
}

QTextDocument *KoTextRange::document() const
{
    return d->document;
}

bool KoTextRange::positionOnlyMode() const
{
    return d->positionOnlyMode;
}

void KoTextRange::setPositionOnlyMode(bool b)
{
    d->positionOnlyMode = b;
}

bool KoTextRange::hasRange() const
{
    return (!d->positionOnlyMode) && d->cursor.hasSelection();
}

int KoTextRange::rangeStart() const
{
    return d->positionOnlyMode ? d->cursor.position() : d->cursor.selectionStart();
}

int KoTextRange::rangeEnd() const
{
    return d->positionOnlyMode ? d->cursor.position() : d->cursor.selectionEnd();
}

void KoTextRange::setRangeStart(int position)
{
    d->positionOnlyMode = true;
    d->cursor.setPosition(position);
}

void KoTextRange::setRangeEnd(int position)
{
    d->positionOnlyMode = false;
    if (d->cursor.isNull()) {
        d->initialEndPosition = position;
    } else {
        d->cursor.setPosition(d->cursor.selectionStart());
        d->cursor.setPosition(position, QTextCursor::KeepAnchor);
    }
}

QString KoTextRange::text() const
{
    return d->positionOnlyMode ? QString() : d->cursor.selectedText();
}

void KoTextRange::setInlineRdf(KoTextInlineRdf *rdf)
{
    d->rdf = rdf;
}

KoTextInlineRdf *KoTextRange::inlineRdf() const
{
    return d->rdf;
}

void KoTextRange::snapshot()
{
    d->snapAnchor = d->cursor.anchor();
    d->snapPos = d->cursor.position();
}

void KoTextRange::restore()
{
    d->cursor.setPosition(d->snapAnchor);
    d->cursor.setPosition(d->snapPos, QTextCursor::KeepAnchor);
}
