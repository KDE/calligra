/*
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "InsertInlineObjectCommand.h"

#include "KoInlineObject.h"
#include "KoInlineTextObjectManager.h"
#include "KoTextDocument.h"
#include "KoTextEditor.h"

InsertInlineObjectCommand::InsertInlineObjectCommand(KoInlineObject *inlineObject, QTextDocument *document, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_inlineObject(inlineObject)
    , m_document(document)
    , m_first(true)
    , m_position(-1)
{
}

InsertInlineObjectCommand::~InsertInlineObjectCommand()
{
    if (m_deleteInlineObject) {
        delete m_inlineObject;
    }
}

void InsertInlineObjectCommand::redo()
{
    KUndo2Command::redo();

    KoTextDocument doc(m_document);
    KoTextEditor *editor = doc.textEditor();
    if (m_first) {
        doc.inlineTextObjectManager()->insertInlineObject(*editor->cursor(), m_inlineObject);
        m_position = editor->cursor()->position();
        m_first = false;
    } else {
        doc.inlineTextObjectManager()->addInlineObject(m_inlineObject);
    }
    editor->setPosition(m_position);
    QTextCharFormat format = editor->charFormat();
    m_inlineObject->updatePosition(m_document, m_position, format);

    m_deleteInlineObject = false;
}

void InsertInlineObjectCommand::undo()
{
    KUndo2Command::undo();
    KoTextDocument(m_document).inlineTextObjectManager()->removeInlineObject(m_inlineObject);
    m_deleteInlineObject = true;
}
