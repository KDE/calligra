/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTWRITER_H
#define KOTEXTWRITER_H

#include "kotext_export.h"

class KoShapeSavingContext;
class KoStyleManager;
class QTextDocument;
class QTextBlock;
class QTextBlockFormat;
class QTextCharFormat;
class QString;

class KoDocumentRdfBase;

/**
 * KoTextWriter saves the text ODF of a shape
 */
class KOTEXT_EXPORT KoTextWriter
{
public:
    /**
     * Constructor.
     *
     * @param context The context the KoTextWriter is called in
     * @param rdfData The RDF data of the document
     */
    explicit KoTextWriter(KoShapeSavingContext &context, KoDocumentRdfBase *rdfData = nullptr);

    /**
     * Destructor.
     */
    ~KoTextWriter();

    /// XXX: APIDOX!
    static void saveOdf(KoShapeSavingContext &context, KoDocumentRdfBase *rdfData, QTextDocument *document, int from, int to);

    /**
     * Save a paragraph style used in a text block
     *
     * This checks if the style is a document style or a automatic style
     * and saves it accordingly.
     *
     * @param block The block form which the style information are taken
     * @param styleManager The used style manager
     * @param context The saving context
     */
    static QString saveParagraphStyle(const QTextBlock &block, KoStyleManager *styleManager, KoShapeSavingContext &context);

    static QString
    saveParagraphStyle(const QTextBlockFormat &blockFormat, const QTextCharFormat &charFormat, KoStyleManager *styleManager, KoShapeSavingContext &context);

    /**
     * Writes the portion of document contained within 'from' and 'to'
     *
     * @param document The text document we are saving. There can be more than one
     * text document in the office document, but we don't care
     * @param from the start position in characters from which we save
     * @param to the end position in characters up to which we save. If -1, we save to the end
     */
    void write(const QTextDocument *document, int from, int to = -1);

private:
    class Private;
    Private *const d;
};

#endif
