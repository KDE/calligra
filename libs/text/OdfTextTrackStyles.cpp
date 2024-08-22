/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2012-2014 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "OdfTextTrackStyles.h"
#include "KoCharacterStyle.h"
#include "KoParagraphStyle.h"
#include "KoTextDocument.h"

#include "TextDebug.h"
#include <QTextDocument>

QHash<QObject *, OdfTextTrackStyles *> OdfTextTrackStyles::instances;

OdfTextTrackStyles *OdfTextTrackStyles::instance(KoStyleManager *manager)
{
    if (!instances.contains(manager)) {
        instances[manager] = new OdfTextTrackStyles(manager);
        connect(manager, &QObject::destroyed, instances[manager], &OdfTextTrackStyles::styleManagerDied);
    }

    return instances[manager];
}

void OdfTextTrackStyles::registerDocument(QTextDocument *qDoc)
{
    if (!m_documents.contains(qDoc)) {
        m_documents.append(qDoc);
        connect(qDoc, &QObject::destroyed, this, &OdfTextTrackStyles::documentDied);
    }
}

void OdfTextTrackStyles::unregisterDocument(QTextDocument *qDoc)
{
    if (m_documents.contains(qDoc)) {
        m_documents.removeOne(qDoc);
    }
}

OdfTextTrackStyles::OdfTextTrackStyles(KoStyleManager *manager)
    : QObject(manager)
    , m_styleManager(manager)
    , m_changeCommand(nullptr)
{
    connect(manager, &KoStyleManager::editHasBegun, this, &OdfTextTrackStyles::beginEdit);
    connect(manager, &KoStyleManager::editHasEnded, this, &OdfTextTrackStyles::endEdit);
    connect(manager,
            &KoStyleManager::characterStyleHasChanged,
            this,
            QOverload<int, const KoCharacterStyle *, const KoCharacterStyle *>::of(&OdfTextTrackStyles::recordStyleChange));
    connect(manager,
            &KoStyleManager::paragraphStyleHasChanged,
            this,
            QOverload<int, const KoParagraphStyle *, const KoParagraphStyle *>::of(&OdfTextTrackStyles::recordStyleChange));
}

OdfTextTrackStyles::~OdfTextTrackStyles() = default;

void OdfTextTrackStyles::beginEdit()
{
    Q_ASSERT(m_changeCommand == nullptr);
    m_changeCommand = new ChangeStylesMacroCommand(m_documents, m_styleManager.data());
}

void OdfTextTrackStyles::endEdit()
{
    if (m_documents.length() > 0) {
        KUndo2Stack *undoStack = KoTextDocument(m_documents.first()).undoStack();
        if (undoStack) {
            undoStack->push(m_changeCommand);
        }
    } else
        delete m_changeCommand;

    m_changeCommand = nullptr;
}

void OdfTextTrackStyles::recordStyleChange(int id, const KoParagraphStyle *origStyle, const KoParagraphStyle *newStyle)
{
    m_changeCommand->changedStyle(id);

    if (origStyle != newStyle) {
        m_changeCommand->origStyle(origStyle->clone());
        m_changeCommand->changedStyle(newStyle->clone());
    }
}

void OdfTextTrackStyles::recordStyleChange(int id, const KoCharacterStyle *origStyle, const KoCharacterStyle *newStyle)
{
    m_changeCommand->changedStyle(id);

    if (origStyle != newStyle) {
        m_changeCommand->origStyle(origStyle->clone());
        m_changeCommand->changedStyle(newStyle->clone());
    }
}

void OdfTextTrackStyles::styleManagerDied(QObject *manager)
{
    OdfTextTrackStyles::instances.remove(manager);
}

void OdfTextTrackStyles::documentDied(QObject *document)
{
    unregisterDocument(static_cast<QTextDocument *>(document));
}
