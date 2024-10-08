/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ParagraphFormattingCommand.h"

#include "KoTextEditor_p.h"
#include <KoTextDocument.h>
#include <KoTextEditor.h>

#include <KLocalizedString>

class MergeAutoParagraphStyleVisitor : public KoTextVisitor
{
public:
    MergeAutoParagraphStyleVisitor(KoTextEditor *editor, const QTextCharFormat &deltaCharFormat, const QTextBlockFormat &deltaBlockFormat)
        : KoTextVisitor(editor)
        , m_deltaCharFormat(deltaCharFormat)
        , m_deltaBlockFormat(deltaBlockFormat)
    {
    }

    void visitBlock(QTextBlock &block, const QTextCursor &caret) override
    {
        m_formats.clear();
        m_cursors.clear();

        for (QTextBlock::iterator it = block.begin(); it != block.end(); ++it) {
            QTextCursor fragmentSelection(caret);
            fragmentSelection.setPosition(it.fragment().position());
            fragmentSelection.setPosition(it.fragment().position() + it.fragment().length(), QTextCursor::KeepAnchor);

            if (fragmentSelection.anchor() >= fragmentSelection.position()) {
                continue;
            }

            visitFragmentSelection(fragmentSelection);
        }

        QTextCursor cursor(caret);
        cursor.mergeBlockFormat(m_deltaBlockFormat);
        cursor.mergeBlockCharFormat(m_deltaCharFormat);

        QList<QTextCharFormat>::ConstIterator it = m_formats.constBegin();
        foreach (QTextCursor cursor, m_cursors) {
            cursor.setCharFormat(*it);
            ++it;
        }
    }

    void visitFragmentSelection(QTextCursor &fragmentSelection) override
    {
        QTextCharFormat format = fragmentSelection.charFormat();
        format.merge(m_deltaCharFormat);

        m_formats.append(format);
        m_cursors.append(fragmentSelection);
    }

    QTextCharFormat m_deltaCharFormat;
    QTextBlockFormat m_deltaBlockFormat;
    QList<QTextCharFormat> m_formats;
    QList<QTextCursor> m_cursors;
};

ParagraphFormattingCommand::ParagraphFormattingCommand(KoTextEditor *editor,
                                                       const QTextCharFormat &characterFormat,
                                                       const QTextBlockFormat &blockFormat,
                                                       const KoListLevelProperties &llp,
                                                       KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_first(true)
    , m_editor(editor)
    , m_charFormat(characterFormat)
    , m_blockFormat(blockFormat)
    , m_levelProperties(llp)
{
    Q_ASSERT(editor);

    setText(kundo2_i18n("Direct Paragraph Formatting"));
}

ParagraphFormattingCommand::~ParagraphFormattingCommand() = default;

void ParagraphFormattingCommand::undo()
{
    KUndo2Command::undo();
}

void ParagraphFormattingCommand::redo()
{
    if (!m_first) {
        KUndo2Command::redo();
    } else {
        MergeAutoParagraphStyleVisitor visitor(m_editor, m_charFormat, m_blockFormat);

        m_editor->recursivelyVisitSelection(m_editor->document()->rootFrame()->begin(), visitor);

        KoTextEditor::ChangeListFlags flags(KoTextEditor::AutoListStyle | KoTextEditor::DontUnsetIfSame);

        m_editor->setListProperties(m_levelProperties, flags, this);

        m_first = false;
    }
}
