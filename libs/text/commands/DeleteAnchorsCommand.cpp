// This file is part of the KDE project
// SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "DeleteAnchorsCommand.h"

#include "KoAnchorInlineObject.h"
#include "KoAnchorTextRange.h"
#include "KoInlineTextObjectManager.h"
#include "KoTextDocument.h"
#include "KoTextRangeManager.h"
#include <QTextCursor>

#include "TextDebug.h"

#include <algorithm>

bool sortAnchor(KoAnchorInlineObject *a1, KoAnchorInlineObject *a2)
{
    return a1->position() > a2->position();
}

DeleteAnchorsCommand::DeleteAnchorsCommand(const QList<KoShapeAnchor *> &anchorObjects, QTextDocument *document, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_first(true)
    , m_deleteAnchors(false)
{
    foreach (KoShapeAnchor *anchor, anchorObjects) {
        KoAnchorInlineObject *anchorObject = dynamic_cast<KoAnchorInlineObject *>(anchor->textLocation());
        KoAnchorTextRange *anchorRange = dynamic_cast<KoAnchorTextRange *>(anchor->textLocation());
        if (anchorObject && anchorObject->document() == document) {
            m_anchorObjects.append(anchorObject);
        } else if (anchorRange && anchorRange->document() == document) {
            m_anchorRanges.append(anchorRange);
        }
    }
    std::sort(m_anchorObjects.begin(), m_anchorObjects.end(), sortAnchor);
}

DeleteAnchorsCommand::~DeleteAnchorsCommand()
{
    if (m_deleteAnchors) {
        qDeleteAll(m_anchorRanges);
    }
}

void DeleteAnchorsCommand::redo()
{
    KUndo2Command::redo();
    m_deleteAnchors = true;
    if (m_first) {
        m_first = false;
        foreach (KoAnchorInlineObject *anchorObject, m_anchorObjects) {
            QTextCursor cursor(m_document);
            cursor.setPosition(anchorObject->position());
            cursor.deleteChar();
        }
    }
    KoInlineTextObjectManager *manager = KoTextDocument(m_document).inlineTextObjectManager();
    Q_ASSERT(manager);
    if (manager) {
        foreach (KoAnchorInlineObject *anchorObject, m_anchorObjects) {
            manager->removeInlineObject(anchorObject);
        }
    }
    KoTextRangeManager *rangeManager = KoTextDocument(m_document).textRangeManager();
    if (rangeManager) {
        foreach (KoAnchorTextRange *anchorRange, m_anchorRanges) {
            rangeManager->remove(anchorRange);
            m_document->markContentsDirty(anchorRange->position(), 0);
        }
    }
}

void DeleteAnchorsCommand::undo()
{
    KoInlineTextObjectManager *manager = KoTextDocument(m_document).inlineTextObjectManager();
    Q_ASSERT(manager);
    if (manager) {
        foreach (KoAnchorInlineObject *anchorObject, m_anchorObjects) {
            manager->addInlineObject(anchorObject);
        }
    }
    KUndo2Command::undo();
    KoTextRangeManager *rangeManager = KoTextDocument(m_document).textRangeManager();
    if (rangeManager) {
        foreach (KoAnchorTextRange *anchorRange, m_anchorRanges) {
            rangeManager->insert(anchorRange);
            m_document->markContentsDirty(anchorRange->position(), 0);
        }
    }
    m_deleteAnchors = false;
}
