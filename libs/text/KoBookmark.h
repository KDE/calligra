/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOBOOKMARK_H
#define KOBOOKMARK_H

#include "KoTextRange.h"
#include "kotext_export.h"

class KoBookmarkManager;

/**
 * A document can store a set of cursor positions/selected cursor locations which can be
 * retrieved again later to go to those locations from any location in the document.
 * The bookmark location will be automatically updated if user alters the text in the document.
 * A bookmark is identified by it's name, and all bookmarks are managed by KoBookmarkManager. A
 * bookmark can be retrieved from the bookmark manager by using name as identifier.
 * @see KoBookmarkManager
 */
class KOTEXT_EXPORT KoBookmark : public KoTextRange
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * By default a bookmark has the SinglePosition type and an empty name.
     * The name is set when the book is inserted into the bookmark manager.
     *
     * @param document the text document where this bookmark is located
     */
    explicit KoBookmark(const QTextCursor &);
    explicit KoBookmark(QTextDocument *document, int position);

    ~KoBookmark() override;

    /// reimplemented from super
    void saveOdf(KoShapeSavingContext &context, int position, TagType tagType) const override;

    /**
     * Set the new name for this bookmark
     * @param name the new name of the bookmark
     */
    void setName(const QString &name);

    /// @return the name of this bookmark
    QString name() const;

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /**
     * This is called to allow Cut and Paste of bookmarks. This
     * method gives a correct, unique, name
     */
    static QString createUniqueBookmarkName(const KoBookmarkManager *bmm, const QString &bookmarkName, bool isEndMarker);

private:
    class Private;
    Private *const d;
};

#endif
