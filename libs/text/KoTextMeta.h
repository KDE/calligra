/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTMETA_H
#define KOTEXTMETA_H

#include "KoInlineObject.h"
#include "kotext_export.h"

/**
 * Used to indicate an ODF text:meta container. This is very similar to a KoBookmark
 * in that a specific start-end is marked.
 */
class KOTEXT_EXPORT KoTextMeta : public KoInlineObject
{
    Q_OBJECT
public:
    enum BookmarkType {
        StartBookmark, ///< start position
        EndBookmark ///< end position
    };

    /**
     * Constructor
     * @param name the name for this bookmark
     * @param document the text document where this bookmark is located
     */
    explicit KoTextMeta(const QTextDocument *document);

    ~KoTextMeta() override;

    /// reimplemented from super
    void saveOdf(KoShapeSavingContext &context) override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// reimplemented from super
    void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format) override;
    /// reimplemented from super
    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override;
    /// reimplemented from super
    void paint(QPainter &painter,
               QPaintDevice *pd,
               const QTextDocument *document,
               const QRectF &rect,
               const QTextInlineObject &object,
               int posInDocument,
               const QTextCharFormat &format) override;

    void setType(BookmarkType type);

    /// @return the current type of this bookmark
    BookmarkType type() const;

    void setEndBookmark(KoTextMeta *bookmark);

    /// @return the end bookmark if the type is StartBookmark
    KoTextMeta *endBookmark() const;

    /// @return the exact cursor position of this bookmark in document
    int position() const;

private:
    class Private; // TODO share the private with super
    Private *const d;
};

#endif
