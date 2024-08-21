/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss \pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeTracker.h"
#include "TextShapeDebug.h"
#include "TextTool.h"

ChangeTracker::ChangeTracker(TextTool *parent)
    : QObject(parent)
    , m_document(nullptr)
    , m_tool(parent)
    , m_enableSignals(true)
    , m_reverseUndo(false)
{
}

void ChangeTracker::setDocument(QTextDocument *document)
{
    m_reverseUndo = false;
    if (m_document)
        disconnect(m_document, &QTextDocument::contentsChange, this, &ChangeTracker::contentsChange);
    m_document = document;
    if (m_document)
        connect(m_document, &QTextDocument::contentsChange, this, &ChangeTracker::contentsChange);
}

int ChangeTracker::getChangeId(QString title, int existingChangeId)
{
    Q_UNUSED(title);
    Q_UNUSED(existingChangeId);
    debugTextShape << "ChangeTracker::changeId :" << m_changeId;
    return m_changeId++;
}

void ChangeTracker::contentsChange(int from, int charsRemoves, int charsAdded)
{
    Q_UNUSED(from);
    Q_UNUSED(charsRemoves);
    Q_UNUSED(charsAdded);
    /*
        if (! m_enableSignals) return;
        m_enableSignals = false;
        debugTextShape << "ChangeTracker::contentsChange" << from << "," << charsRemoves << "," << charsAdded;

        if (charsRemoves == 0 && charsAdded == 0) {
            // I think we can quietly ignore this.
        } else if (charsRemoves == 0) { // easy
            QTextCursor cursor(m_document);
            cursor.setPosition(from);
            cursor.setPosition(from + charsAdded, QTextCursor::KeepAnchor);
            debugTextShape << "   added text:" << cursor.selectedText();
        } else {
            bool prev = m_tool->m_allowAddUndoCommand;
            m_tool->m_allowAddUndoCommand = false;
            m_reverseUndo ? m_document->redo() : m_document->undo();
            QTextCursor cursor(m_document);
            cursor.setPosition(from);
            cursor.setPosition(from + charsRemoves, QTextCursor::KeepAnchor);
            QString previousText = cursor.selectedText();
            m_reverseUndo ? m_document->undo() : m_document->redo();
            m_tool->m_allowAddUndoCommand = prev;

            cursor.setPosition(from);
            cursor.setPosition(from + charsAdded, QTextCursor::KeepAnchor);

            debugTextShape << "   - " << previousText;
            debugTextShape << "   + " << cursor.selectedText();
        }

        m_enableSignals = true;
        m_reverseUndo = false;
    */
}

void ChangeTracker::notifyForUndo()
{
    m_reverseUndo = true;
}
