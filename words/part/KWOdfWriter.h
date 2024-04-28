/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007-2008 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2007-2008 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWODFWRITER_H
#define KWODFWRITER_H

#include "KWPageStyle.h"

#include <KoRTree.h>

class KWDocument;
class KoOdfWriteStore;
class KoShapeSavingContext;
class KoEmbeddedDocumentSaver;
class KWTextFrameSet;
class KWPage;

class KoStore;

/**
 * Class that has a lot of the OpenDocument (ODF) saving code for Words.
 */
class KWOdfWriter : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param document the document this writer will work for.
     */
    explicit KWOdfWriter(KWDocument *document);

    /**
     * Destructor
     */
    ~KWOdfWriter() override;

    /**
     *  @brief Writes an OASIS OpenDocument to a store.
     *  This implements the KoDocument::saveOdf method.
     */
    bool save(KoOdfWriteStore &odfStore, KoEmbeddedDocumentSaver &embeddedSaver);

private:
    void saveHeaderFooter(KoShapeSavingContext &context);
    QByteArray serializeHeaderFooter(KoShapeSavingContext &context, KWTextFrameSet *fs);

    void calculateZindexOffsets();
    void addShapeToTree(KoShape *shape);

    bool saveOdfSettings(KoStore *store);

    /// The Words document.
    KWDocument *m_document;
    QHash<KWPageStyle, QString> m_masterPages;
    /// Since ODF requires zindexes >= 0 and we can have negative ones we will calculate an offset per
    /// page and store that here.
    QHash<KWPage, int> m_zIndexOffsets;
    KoRTree<KoShape *> m_shapeTree;
};

#endif
