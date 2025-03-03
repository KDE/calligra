/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOLIST_H
#define KOLIST_H

#include "styles/KoListStyle.h"

#include <QMetaType>
#include <QPointer>
#include <QTextList>
#include <QVector>

class KoListPrivate;

/**
 * This class represents an ODF list. An ODF list may have upto 10 levels
 * Each of the levels is represented as a QTextList (QTextList does not support
 * embedded lists). There is always an associated KoListStyle that holds the
 * styling information for various level of the ODF list.
 */
class KOTEXT_EXPORT KoList : public QObject
{
    Q_OBJECT
public:
    enum Type {
        TextList,
        NumberedParagraph
    };

    /// Constructor
    KoList(const QTextDocument *document, KoListStyle *style, Type type = TextList);

    /// Destructor
    ~KoList() override;

    /// Adds \a block to \a level of this list
    void add(const QTextBlock &block, int level);

    /// Removes \a block from any KoList the block is a part of
    static void remove(const QTextBlock &block);

    /**
     * Adds \a block to a list that follows \a style at \a level. If the block is
     * already a part of a list, it is removed from that list. If the block before
     * or after this block is part of a list that follows \a style, this block is
     * added to that list. If required a new KoList is created.
     * Returns the KoList that this block was added to.
     */
    static KoList *applyStyle(const QTextBlock &block, KoListStyle *style, int level);

    /// Sets the style of this list
    void setStyle(KoListStyle *style);

    /// Returns the style of this list
    KoListStyle *style() const;

    /// Return true if this list contains \a textlist
    bool contains(QTextList *textList) const;

    /// Returns the QTextLists that form this list
    QVector<QPointer<QTextList>> textLists() const;

    QVector<KoListStyle::ListIdType> textListIds() const;

    static int level(const QTextBlock &block);

    /// Update the stored QTextList pointer for the given block
    void updateStoredList(const QTextBlock &block);

    KoList *listContinuedFrom() const;
    void setListContinuedFrom(KoList *list);

private:
    KoListPrivate *const d;

    Q_PRIVATE_SLOT(d, void styleChanged(int))
};

Q_DECLARE_METATYPE(KoList *)
Q_DECLARE_METATYPE(QList<KoList *>)

#endif // KOLIST_H
