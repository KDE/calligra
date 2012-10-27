/* This file is part of the KDE project
<<<<<<< HEAD
 * Copyright (C) 2006-2009 Thomas Zander <zander@kde.org>
=======
 * Copyright (c) 2012 C. Boemann <cbo@boemann.dk>
>>>>>>> master
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
#ifndef KOTEXTRANGEMANAGER_H
#define KOTEXTRANGEMANAGER_H

#include "KoTextRange.h"
#include "KoBookmarkManager.h"
<<<<<<< HEAD
#include "KoAnnotationManager.h"
=======
>>>>>>> master
#include "kotext_export.h"

// Qt + kde
#include <QHash>
<<<<<<< HEAD
=======
#include <QSet>
>>>>>>> master
#include <QTextBlock>


/**
 * A container to register all the text ranges with.
 */
class KOTEXT_EXPORT KoTextRangeManager : public QObject
{
    Q_OBJECT
public:
    /// Constructor
    explicit KoTextRangeManager(QObject *parent = 0);
    virtual ~KoTextRangeManager();

    QList<KoTextRange *> textRanges() const;

    /**
     * Insert a new text range into the manager.
<<<<<<< HEAD
     * @param object the inline object to insert.
=======
     * @param object the text range to be inserted.
>>>>>>> master
     */
    void insert(KoTextRange *object);

    /**
     * Remove a text range from this manager.
<<<<<<< HEAD
     * @param the text rage to be removed
     */
    void remove(KoTextRange *object);
=======
     * @param range the text range to be removed
     */
    void remove(KoTextRange *range);
>>>>>>> master

    /**
     * Return the bookmark manager.
     */
    const KoBookmarkManager *bookmarkManager() const;

    /**
<<<<<<< HEAD
     * Return the annotation manager.
     */
    const KoAnnotationManager *annotationManager() const;

    /**
=======
>>>>>>> master
     * Return a multi hash of KoTextRange that have start or end points between first and last
     * If the text range is a selection then the opposite end has to be within matchFirst and
     * matchLast.
     * Single position text ranges is only added once to the hash
     */
    QHash<int, KoTextRange *> textRangesChangingWithin(int first, int last, int matchFirst, int matchLast) const;

private:
<<<<<<< HEAD
    QHash<int, KoTextRange *> m_textRanges;
    QHash<int, KoTextRange *> m_deletedTextRanges;
    int m_lastObjectId;

    KoBookmarkManager m_bookmarkManager;
    KoAnnotationManager m_annotationManager;
=======
    QSet<KoTextRange *> m_textRanges;
    QSet<KoTextRange *> m_deletedTextRanges; // kept around for undo purposes

    KoBookmarkManager m_bookmarkManager;
>>>>>>> master
};

Q_DECLARE_METATYPE(KoTextRangeManager *)
#endif
