/* This file is part of the KDE project
 * Copyright (c) 2012 Boudewijn Rempt <boud@kogmbh.com>
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
#include "KoTextRangeManager.h"
#include "KoTextDocument.h"
#include "KoBookmark.h"
#include "KoBookmarkManager.h"
#include "KoAnnotation.h"
#include "KoAnnotationManager.h"
#include "KoAnnotation.h"
#include "KoAnnotationManager.h"

#include <QTextCursor>

KoTextRangeManager::KoTextRangeManager(QObject *parent)
        : QObject(parent),
        m_lastObjectId(0)
{
}

KoTextRangeManager::~KoTextRangeManager()
{
}

void KoTextRangeManager::insert(KoTextRange *textRange)
{
    if (!textRange) {
        return;
    }

    int id = textRange->id();
    if (id == -1) {
        textRange->setId(++m_lastObjectId);
        textRange->setManager(this);
    }
    else {
        m_deletedTextRanges.remove(id);
    }

    KoBookmark *bookmark = dynamic_cast<KoBookmark *>(textRange);
    if (bookmark) {
        m_bookmarkManager.insert(bookmark->name(), bookmark);
    }
    else {
        KoAnnotation *annotation = dynamic_cast<KoAnnotation *>(textRange);
        if (annotation) {
            m_annotationManager.insert(annotation->name(), annotation);
        }
    }
    m_textRanges.insert(textRange->id(), textRange);
}

void KoTextRangeManager::remove(KoTextRange *textRange)
{
    if (!textRange) {
        return;
    }

    KoBookmark *bookmark = dynamic_cast<KoBookmark *>(textRange);
    if (bookmark) {
        m_bookmarkManager.remove(bookmark->name());
    }
    else {
        KoAnnotation *annotation = dynamic_cast<KoAnnotation *>(textRange);
        if (annotation) {
            m_annotationManager.remove(annotation->name());
        }
    }

    int id = textRange->id();
    m_textRanges.remove(id);
    m_deletedTextRanges[id] = textRange;
}

const KoBookmarkManager *KoTextRangeManager::bookmarkManager() const
{
    return &m_bookmarkManager;
}

const KoAnnotationManager *KoTextRangeManager::annotationManager() const
{
    return &m_annotationManager;
}

QList<KoTextRange *> KoTextRangeManager::textRanges() const
{
    return m_textRanges.values();
}


QHash<int, KoTextRange *> KoTextRangeManager::textRangesChangingWithin(int first, int last, int matchFirst, int matchLast) const
{
    QHash<int, KoTextRange *> ranges;
    foreach (KoTextRange *range, m_textRanges) {
        if (!range->hasSelection()) {
            if (range->cursor().position() >= first && range->cursor().position() <= last) {
                ranges.insertMulti(range->cursor().position(), range);
            }
        } else {
            if (range->cursor().selectionStart() >= first && range->cursor().selectionStart() <= last) {
                if (matchLast == -1 || range->cursor().selectionEnd() <= matchLast) {
                    ranges.insertMulti(range->cursor().selectionStart(), range);
                }
            }
            if (range->cursor().selectionEnd() >= first && range->cursor().selectionEnd() <= last) {
                if (matchFirst == -1 || range->cursor().selectionStart() >= matchFirst) {
                    ranges.insertMulti(range->cursor().selectionEnd(), range);
                }
            }
        }
    }
    return ranges;
}

#include <KoTextRangeManager.moc>
