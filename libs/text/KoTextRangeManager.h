/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOTEXTRANGEMANAGER_H
#define KOTEXTRANGEMANAGER_H

#include "KoAnnotationManager.h"
#include "KoBookmarkManager.h"
#include "KoTextRange.h"
#include "kotext_export.h"

// Qt
#include <QHash>
#include <QMetaType>
#include <QSet>
#include <QTextCursor>

/**
 * A container to register all the text ranges with.
 */
class KOTEXT_EXPORT KoTextRangeManager : public QObject
{
    Q_OBJECT
public:
    /// Constructor
    explicit KoTextRangeManager(QObject *parent = nullptr);
    ~KoTextRangeManager() override;

    QList<KoTextRange *> textRanges() const;

    QList<KoTextRange *> textRanges(const QTextDocument *doc) const;

    void finalizeTextRanges();

    /**
     * Insert a new text range into the manager.
     * @param object the text range to be inserted.
     */
    void insert(KoTextRange *object);

    /**
     * Remove a text range from this manager.
     * @param range the text range to be removed
     */
    void remove(KoTextRange *range);

    /**
     * Return the bookmark manager.
     */
    const KoBookmarkManager *bookmarkManager() const;

    /**
     * Return the annotation manager.
     */
    const KoAnnotationManager *annotationManager() const;

    /**
     * Return a multi hash of KoTextRange that have start or end points between first and last
     * If the text range is a selection then the opposite end has to be within matchFirst and
     * matchLast.
     * Single position text ranges is only added once to the hash
     */
    QMultiHash<int, KoTextRange *> textRangesChangingWithin(const QTextDocument *, int first, int last, int matchFirst, int matchLast) const;

    QMultiHash<int, KoTextRange *>
    textRangesChangingWithin(const QTextDocument *, QList<const QMetaObject *> types, int first, int last, int matchFirst, int matchLast) const;

    template<class T>
    T *createAndLoadOdf(const QTextCursor &cursor, const KoXmlElement &element, KoShapeLoadingContext &context)
    {
        T *object = new T(cursor.document(), cursor.position());
        object->setManager(this);
        if (!object->loadOdf(element, context)) {
            delete (object);
            return nullptr;
        }
        insert(object);
        return object;
    }

private:
    class KoTextRangeManagerPrivate;

    KoTextRangeManagerPrivate *d;

    KoBookmarkManager m_bookmarkManager;
    KoAnnotationManager m_annotationManager;
};

Q_DECLARE_METATYPE(KoTextRangeManager *)
#endif
