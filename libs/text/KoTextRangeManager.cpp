/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextRangeManager.h"

#include "KoAnnotation.h"
#include "KoBookmark.h"
#include <algorithm>

#include "TextDebug.h"

struct KoTextRangeManagerIndex {
    QList<KoTextRange *> singlePoints;
    QList<KoTextRange *> nonOverlapping;
    QList<KoTextRange *> overlapping;

    void addToIndex(KoTextRange *r)
    {
        if (!r->hasRange()) {
            if (addInList(r, singlePoints))
                return;
        }
        if (r->rangeEnd() != r->rangeStart()) {
            if (addInList(r, nonOverlapping))
                return;
        }
        addInList(r, overlapping, true);
    }

    void removeFromIndex(KoTextRange *r)
    {
        if (!r->hasRange())
            singlePoints.removeOne(r);
        else
            nonOverlapping.removeOne(r);
        overlapping.removeOne(r);
    }

private:
    bool addInList(KoTextRange *r, QList<KoTextRange *> &list, bool force = false)
    {
        auto it = list.begin();
        while (it != list.end() && (*it)->rangeStart() < r->rangeStart())
            ++it;
        if (force || it == list.end() || ((*it)->rangeStart() > r->rangeStart())) {
            list.insert(it, r);
            return true;
        }
        return false;
    }
};

class KoTextRangeManager::KoTextRangeManagerPrivate
{
public:
    QHash<const QTextDocument *, QSet<KoTextRange *>> m_textRanges;
    QHash<const QTextDocument *, QSet<KoTextRange *>> m_deletedTextRanges; // kept around for undo purposes

    QHash<const QTextDocument *, QHash<const QMetaObject *, KoTextRangeManagerIndex>> indexes;

    QHash<const QTextDocument *, QList<KoTextRange *>> unfinalizedRanges;

    void addDocument(const QTextDocument *doc)
    {
        // Should we connect to the QTextDocument::cursorPositionChanged signal to make sure
        // the indexes remain sorted? I still don't see how we could end up with a cursor being
        // moved before another with the current object interfaces, but any mistake in this perception
        // could end up with completely broken layout, crashes...
        if (!m_textRanges.contains(doc)) {
            m_textRanges.insert(doc, {});
            m_deletedTextRanges.insert(doc, {});
            indexes.insert(doc, {});
            unfinalizedRanges.insert(doc, {});
        }
    }
};

KoTextRangeManager::KoTextRangeManager(QObject *parent)
    : QObject(parent)
    , d(new KoTextRangeManagerPrivate())
{
}

KoTextRangeManager::~KoTextRangeManager()
{
    delete d;
}

QList<KoTextRange *> KoTextRangeManager::textRanges(const QTextDocument *doc) const
{
    return d->m_textRanges.value(doc).values();
}

void KoTextRangeManager::finalizeTextRanges()
{
    for (auto &ranges : d->unfinalizedRanges.values()) {
        for (KoTextRange *range : ranges) {
            range->finalizePosition();
            insert(range);
        }
    }
    d->unfinalizedRanges.clear();
}

void KoTextRangeManager::insert(KoTextRange *textRange)
{
    if (!textRange) {
        return;
    }

    auto doc = textRange->document();
    d->addDocument(doc);

    if (!textRange->isFinalized()) {
        d->unfinalizedRanges[doc].append(textRange);
        return;
    }

    QSet<KoTextRange *> &docTextRanges = d->m_textRanges[doc];
    QSet<KoTextRange *> &docDeletedTextRanges = d->m_deletedTextRanges[doc];

    if (docTextRanges.contains(textRange)) {
        return;
    }

    if (docDeletedTextRanges.contains(textRange)) {
        docDeletedTextRanges.remove(textRange);
        textRange->restore();
    } else {
        textRange->setManager(this);
    }

    KoBookmark *bookmark = qobject_cast<KoBookmark *>(textRange);
    if (bookmark) {
        m_bookmarkManager.insert(bookmark->name(), bookmark);
    } else {
        KoAnnotation *annotation = qobject_cast<KoAnnotation *>(textRange);
        if (annotation) {
            m_annotationManager.insert(annotation->name(), annotation);
        }
    }
    docTextRanges.insert(textRange);
    if (!d->indexes[doc].contains(textRange->metaObject()))
        d->indexes[doc].insert(textRange->metaObject(), {});
    d->indexes[doc][textRange->metaObject()].addToIndex(textRange);
}

void KoTextRangeManager::remove(KoTextRange *textRange)
{
    if (!textRange) {
        return;
    }

    KoBookmark *bookmark = dynamic_cast<KoBookmark *>(textRange);
    if (bookmark) {
        m_bookmarkManager.remove(bookmark->name());
    } else {
        KoAnnotation *annotation = dynamic_cast<KoAnnotation *>(textRange);
        if (annotation) {
            m_annotationManager.remove(annotation->name());
        }
    }

    d->m_textRanges[textRange->document()].remove(textRange);
    d->m_deletedTextRanges[textRange->document()].insert(textRange);
    d->indexes[textRange->document()][textRange->metaObject()].removeFromIndex(textRange);
    textRange->snapshot();
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
    QList<KoTextRange *> allRanges;
    for (const QSet<KoTextRange *> &docRanges : d->m_textRanges.values()) {
        allRanges.append(docRanges.values());
    }
    return allRanges;
}

QMultiHash<int, KoTextRange *> KoTextRangeManager::textRangesChangingWithin(const QTextDocument *doc, int first, int last, int matchFirst, int matchLast) const
{
    QMultiHash<int, KoTextRange *> ranges;
    if (!d->m_textRanges.contains(doc))
        return ranges;

        // TODO: this constant is kind of magic. It's the trade-of between comparing each item, and the cost of going through several lists, with varying
        // ordering rules
#define INDEX_USAGE_THRESHOLD 10
    if (d->m_textRanges[doc].size() < INDEX_USAGE_THRESHOLD) {
        const QSet<KoTextRange *> &docRanges = d->m_textRanges.value(doc);
        // qDebug() << "KoTextRangeManager => going to skip the index";
        // On a small set of items, going through the set is going to be faster
        for (KoTextRange *range : docRanges) {
            if (!range->hasRange()) {
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    ranges.insert(range->rangeStart(), range);
                }
            } else {
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    if (matchLast == -1 || range->rangeEnd() <= matchLast) {
                        if (range->rangeEnd() >= matchFirst) {
                            ranges.insert(range->rangeStart(), range);
                        }
                    }
                }
                if (range->rangeEnd() >= first && range->rangeEnd() <= last) {
                    if (matchLast == -1 || range->rangeStart() <= matchLast) {
                        if (range->rangeStart() >= matchFirst) {
                            ranges.insert(range->rangeEnd(), range);
                        }
                    }
                }
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    if (matchLast == -1 || range->rangeEnd() >= matchLast) {
                        if (range->rangeEnd() >= matchFirst) {
                            ranges.replace(range->rangeStart(), range);
                        }
                    }
                }
            }
        }
    } else {
        for (const auto &docIndex : d->indexes.value(doc).values()) {
            auto comp = [](KoTextRange *r, int value) {
                return r->rangeStart() <= value;
            };

            // First, singlePoints
            auto it = std::lower_bound(docIndex.singlePoints.constBegin(), docIndex.singlePoints.constEnd(), first, comp);
            for (; it != docIndex.singlePoints.constEnd(); ++it) {
                if ((*it)->rangeStart() < first)
                    continue;
                if ((*it)->rangeStart() > last)
                    break;
                ranges.insert((*it)->rangeStart(), (*it));
            }

            // Second, non-overlapping
            it = std::lower_bound(docIndex.nonOverlapping.constBegin(), docIndex.nonOverlapping.constEnd(), first, comp);
            for (; it != docIndex.nonOverlapping.constEnd(); ++it) {
                KoTextRange *range = *it;
                if (range->rangeEnd() < first)
                    continue;
                if (range->rangeStart() > last)
                    break;
                // We have excluded what is completely out of scope.
                // Now for the few remaining ranges…
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    if (matchLast == -1 || range->rangeEnd() <= matchLast) {
                        if (range->rangeEnd() >= matchFirst) {
                            ranges.insert(range->rangeStart(), range);
                        }
                    }
                }
                if (range->rangeEnd() >= first && range->rangeEnd() <= last) {
                    if (matchLast == -1 || range->rangeStart() <= matchLast) {
                        if (range->rangeStart() >= matchFirst) {
                            ranges.insert(range->rangeEnd(), range);
                        }
                    }
                }
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    if (matchLast == -1 || range->rangeEnd() >= matchLast) {
                        if (range->rangeEnd() >= matchFirst) {
                            ranges.replace(range->rangeStart(), range);
                        }
                    }
                }
            }

            // Last, overlapppings
            for (KoTextRange *range : docIndex.overlapping) {
                if (range->rangeStart() < first)
                    continue;
                if (range->rangeStart() > last)
                    break;

                if (!range->hasRange()) {
                    if (range->rangeStart() >= first && range->rangeStart() <= last) {
                        ranges.insert(range->rangeStart(), range);
                    }
                } else {
                    if (range->rangeStart() >= first && range->rangeStart() <= last) {
                        if (matchLast == -1 || range->rangeEnd() <= matchLast) {
                            if (range->rangeEnd() >= matchFirst) {
                                ranges.insert(range->rangeStart(), range);
                            }
                        }
                    }
                    if (range->rangeEnd() >= first && range->rangeEnd() <= last) {
                        if (matchLast == -1 || range->rangeStart() <= matchLast) {
                            if (range->rangeStart() >= matchFirst) {
                                ranges.insert(range->rangeEnd(), range);
                            }
                        }
                    }
                    if (range->rangeStart() >= first && range->rangeStart() <= last) {
                        if (matchLast == -1 || range->rangeEnd() >= matchLast) {
                            if (range->rangeEnd() >= matchFirst) {
                                ranges.replace(range->rangeStart(), range);
                            }
                        }
                    }
                }
            }
        }
    }

    return ranges;
}

QMultiHash<int, KoTextRange *> KoTextRangeManager::textRangesChangingWithin(const QTextDocument *doc,
                                                                            QList<const QMetaObject *> types,
                                                                            int first,
                                                                            int last,
                                                                            int matchFirst,
                                                                            int matchLast) const
{
    QMultiHash<int, KoTextRange *> ranges;
    if (!d->m_textRanges.contains(doc))
        return ranges;

    for (const QMetaObject *type : d->indexes.value(doc).keys()) {
        bool found = false;
        for (const QMetaObject *requestedType : types) {
            if (type->inherits(requestedType)) {
                found = true;
                break;
            }
        }
        if (!found)
            continue;
        const auto &docIndex = d->indexes.value(doc).value(type);

        auto comp = [](KoTextRange *r, int value) {
            return r->rangeStart() <= value;
        };

        // First, singlePoints
        auto it = std::lower_bound(docIndex.singlePoints.constBegin(), docIndex.singlePoints.constEnd(), first, comp);
        for (; it != docIndex.singlePoints.constEnd(); ++it) {
            if ((*it)->rangeStart() < first)
                continue;
            if ((*it)->rangeStart() > last)
                break;
            ranges.insert((*it)->rangeStart(), (*it));
        }

        // Second, non-overlapping
        it = std::lower_bound(docIndex.nonOverlapping.constBegin(), docIndex.nonOverlapping.constEnd(), first, comp);
        for (; it != docIndex.nonOverlapping.constEnd(); ++it) {
            KoTextRange *range = *it;
            if (range->rangeEnd() < first)
                continue;
            if (range->rangeStart() > last)
                break;
            // We have excluded what is completely out of scope.
            // Now for the few remaining ranges…
            if (range->rangeStart() >= first && range->rangeStart() <= last) {
                if (matchLast == -1 || range->rangeEnd() <= matchLast) {
                    if (range->rangeEnd() >= matchFirst) {
                        ranges.insert(range->rangeStart(), range);
                    }
                }
            }
            if (range->rangeEnd() >= first && range->rangeEnd() <= last) {
                if (matchLast == -1 || range->rangeStart() <= matchLast) {
                    if (range->rangeStart() >= matchFirst) {
                        ranges.insert(range->rangeEnd(), range);
                    }
                }
            }
            if (range->rangeStart() >= first && range->rangeStart() <= last) {
                if (matchLast == -1 || range->rangeEnd() >= matchLast) {
                    if (range->rangeEnd() >= matchFirst) {
                        ranges.replace(range->rangeStart(), range);
                    }
                }
            }
        }

        // Last, overlapppings
        for (KoTextRange *range : docIndex.overlapping) {
            if (range->rangeStart() < first)
                continue;
            if (range->rangeStart() > last)
                break;

            if (!range->hasRange()) {
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    ranges.insert(range->rangeStart(), range);
                }
            } else {
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    if (matchLast == -1 || range->rangeEnd() <= matchLast) {
                        if (range->rangeEnd() >= matchFirst) {
                            ranges.insert(range->rangeStart(), range);
                        }
                    }
                }
                if (range->rangeEnd() >= first && range->rangeEnd() <= last) {
                    if (matchLast == -1 || range->rangeStart() <= matchLast) {
                        if (range->rangeStart() >= matchFirst) {
                            ranges.insert(range->rangeEnd(), range);
                        }
                    }
                }
                if (range->rangeStart() >= first && range->rangeStart() <= last) {
                    if (matchLast == -1 || range->rangeEnd() >= matchLast) {
                        if (range->rangeEnd() >= matchFirst) {
                            ranges.replace(range->rangeStart(), range);
                        }
                    }
                }
            }
        }
    }

    return ranges;
}