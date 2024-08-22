// This file is part of the KDE project
// SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "SplitSectionsCommand.h"
#include <KoParagraphStyle.h>
#include <KoSection.h>
#include <KoSectionEnd.h>
#include <KoSectionUtils.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

#include <KLocalizedString>
#include <kundo2command.h>

SplitSectionsCommand::SplitSectionsCommand(QTextDocument *document, SplitType type, int splitPosition)
    : KUndo2Command()
    , m_first(true)
    , m_document(document)
    , m_type(type)
    , m_splitPosition(splitPosition)
{
    if (m_type == Startings) {
        setText(kundo2_i18n("Split sections startings"));
    } else { // Endings
        setText(kundo2_i18n("Split sections endings"));
    }
}

SplitSectionsCommand::~SplitSectionsCommand() = default;

void SplitSectionsCommand::undo()
{
    KUndo2Command::undo();
    // FIXME: if it will go to KoTextCommandBase, place UndoRedoFinalizer here

    // All formatting changes will be undone automatically.
    // Model Level is untouched.
}

void SplitSectionsCommand::redo()
{
    KoTextDocument koDocument(m_document);

    if (!m_first) {
        KUndo2Command::redo();
        // FIXME: if it will go to KoTextCommandBase, place UndoRedoFinalizer here

        // All formatting changes will be redone automatically.
        // Model level is untouched.
    } else {
        m_first = false;

        KoTextEditor *editor = koDocument.textEditor();

        if (m_type == Startings) {
            editor->movePosition(QTextCursor::StartOfBlock);
            editor->newLine();
            editor->movePosition(QTextCursor::PreviousBlock);

            QTextBlockFormat fmt = editor->blockFormat();
            KoSectionUtils::setSectionEndings(fmt, QList<KoSectionEnd *>());
            QList<KoSection *> firstBlockStartings = KoSectionUtils::sectionStartings(fmt).mid(0, m_splitPosition);
            QList<KoSection *> moveForward = KoSectionUtils::sectionStartings(fmt).mid(m_splitPosition);
            KoSectionUtils::setSectionStartings(fmt, firstBlockStartings);
            editor->setBlockFormat(fmt);
            editor->movePosition(QTextCursor::NextBlock);
            fmt = editor->blockFormat();
            KoSectionUtils::setSectionStartings(fmt, moveForward);
            editor->setBlockFormat(fmt);
            editor->movePosition(QTextCursor::PreviousBlock);
        } else { // Endings
            editor->movePosition(QTextCursor::EndOfBlock);
            editor->newLine();

            QTextBlockFormat fmt = editor->blockFormat();
            QList<KoSectionEnd *> secondBlockEndings = KoSectionUtils::sectionEndings(fmt).mid(m_splitPosition + 1);
            QList<KoSectionEnd *> moveBackward = KoSectionUtils::sectionEndings(fmt).mid(0, m_splitPosition + 1);
            KoSectionUtils::setSectionEndings(fmt, secondBlockEndings);
            editor->setBlockFormat(fmt);
            editor->movePosition(QTextCursor::PreviousBlock);
            fmt = editor->blockFormat();
            KoSectionUtils::setSectionEndings(fmt, moveBackward);
            editor->setBlockFormat(fmt);
            editor->movePosition(QTextCursor::NextBlock);
        }
    }
}
