/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Pavol Korinek <pavol.korinek@ixonos.com>
 * SPDX-FileCopyrightText: 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>
 * SPDX-FileCopyrightText: 2011 Ko GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TOCGENERATOR_H
#define TOCGENERATOR_H

#include <QObject>
#include <QTextBlock>

class KoTextRangeManager;
class KoTextDocumentLayout;
class KoTableOfContentsGeneratorInfo;

class QTextDocument;

class ToCGenerator : public QObject
{
    Q_OBJECT
public:
    explicit ToCGenerator(QTextDocument *tocDocument, KoTableOfContentsGeneratorInfo *tocInfo);
    ~ToCGenerator() override;

    virtual void setBlock(const QTextBlock &block);

    bool generate();

private:
    QString resolvePageNumber(const QTextBlock &headingBlock);
    void generateEntry(int outlineLevel, QTextCursor &cursor, QTextBlock &block, int &blockId);

    QTextDocument *m_ToCDocument;
    KoTableOfContentsGeneratorInfo *m_ToCInfo;
    QTextBlock m_block;
    QTextDocument *m_document;
    KoTextDocumentLayout *m_documentLayout;
    bool m_success;
    bool m_preservePagebreak;

    // Return the ref (name) of the first KoBookmark in the block, if KoBookmark not found, null QString is returned
    QString fetchBookmarkRef(const QTextBlock &block, KoTextRangeManager *textRangeManager);
};

#endif
