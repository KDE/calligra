// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "InsertNoteCommand.h"

#include <KLocalizedString>

#include <KoInlineNote.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

InsertNoteCommand::InsertNoteCommand(KoInlineNote::Type type, QTextDocument *document)
    : KUndo2Command()
    , m_document(document)
    , m_first(true)
{
    if (type == KoInlineNote::Footnote) {
        setText(kundo2_i18n("Insert Footnote"));
    } else if (type == KoInlineNote::Endnote) {
        setText(kundo2_i18n("Insert Endnote"));
    }
    m_inlineNote = new KoInlineNote(type);
}

InsertNoteCommand::~InsertNoteCommand() = default;

void InsertNoteCommand::undo()
{
    KUndo2Command::undo();
}

void InsertNoteCommand::redo()
{
    if (!m_first) {
        KUndo2Command::redo();
        QTextCursor cursor(m_document.data());
        cursor.setPosition(m_framePosition);
        m_inlineNote->setTextFrame(cursor.currentFrame());
        m_inlineNote->setMotherFrame(KoTextDocument(m_document).auxillaryFrame());
    } else {
        m_first = false;
        if (m_document) {
            KoTextEditor *textEditor = KoTextDocument(m_document).textEditor();
            if (textEditor) {
                textEditor->beginEditBlock();
                QTextCursor *caret = textEditor->cursor();
                if (textEditor->hasSelection()) {
                    textEditor->deleteChar(false, this);
                }
                KoInlineTextObjectManager *manager = KoTextDocument(m_document).inlineTextObjectManager();
                manager->insertInlineObject(*caret, m_inlineNote);
                m_inlineNote->setMotherFrame(KoTextDocument(m_document).auxillaryFrame());
                m_framePosition = m_inlineNote->textFrame()->lastPosition();
                textEditor->setPosition(m_framePosition);

                textEditor->endEditBlock();
            }
        }
    }
}
