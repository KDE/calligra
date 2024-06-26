/*
 *  SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef CHANGESTYLESCOMMAND_H
#define CHANGESTYLESCOMMAND_H

#include <kundo2command.h>

#include <QList>
#include <QSet>
#include <QTextBlockFormat>
#include <QTextCharFormat>

class KoCharacterStyle;
class KoParagraphStyle;
class QTextDocument;

class ChangeStylesCommand : public KUndo2Command
{
public:
    ChangeStylesCommand(QTextDocument *qDoc,
                        const QList<KoCharacterStyle *> &origCharacterStyles,
                        const QList<KoParagraphStyle *> &origParagraphStyles,
                        const QSet<int> &changedStyles,
                        KUndo2Command *parent);
    ~ChangeStylesCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    /**
     * Helper function for clearing common properties.
     *
     * Clears properties in @a firstFormat that have the same value in @a secondFormat.
     */
    void clearCommonProperties(QTextFormat *firstFormat, const QTextFormat &secondFormat);

private:
    struct Memento // documents all change to the textdocument by a single style change
    {
        QTextDocument *document;
        int blockPosition;
        int paragraphStyleId;
        QTextBlockFormat blockDirectFormat;
        QTextBlockFormat blockParentFormat;
        QTextCharFormat blockDirectCharFormat;
        QTextCharFormat blockParentCharFormat;
        QList<QTextCharFormat> fragmentDirectFormats;
        QList<QTextCursor> fragmentCursors;
        QList<int> fragmentStyleId;
    };
    QList<Memento *> m_mementos;

private:
    QList<KoCharacterStyle *> m_origCharacterStyles;
    QList<KoParagraphStyle *> m_origParagraphStyles;
    QSet<int> m_changedStyles;
    QTextDocument *m_document;
    bool m_first;
};

#endif // CHANGESTYLESCOMMAND_H
