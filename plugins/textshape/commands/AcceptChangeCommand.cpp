// This file is part of the KDE project
// SPDX-FileCopyrightText: 2010 Pierre Stirnweiss \pstirnweiss@googlemail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "AcceptChangeCommand.h"

#include <KoGenChange.h>
#include <KoTextDocument.h>

#include <changetracker/KoChangeTracker.h>
#include <changetracker/KoChangeTrackerElement.h>
#include <styles/KoCharacterStyle.h>

#include <KLocalizedString>

#include <QPair>
#include <QStack>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFragment>

AcceptChangeCommand::AcceptChangeCommand(int changeId, const QList<QPair<int, int>> &changeRanges, QTextDocument *document, KUndo2Command *parent)
    : KoTextCommandBase(parent)
    , m_first(true)
    , m_changeId(changeId)
    , m_changeRanges(changeRanges)
    , m_document(document)
{
    setText(kundo2_i18n("Accept change"));

    m_changeTracker = KoTextDocument(m_document).changeTracker();
}

AcceptChangeCommand::~AcceptChangeCommand() = default;

void AcceptChangeCommand::redo()
{
    if (m_first) {
        m_first = false;
        QTextCursor cursor(m_document);
        if (m_changeTracker->elementById(m_changeId)->getChangeType() != KoGenChange::DeleteChange) {
            QList<QPair<int, int>>::const_iterator it;
            for (it = m_changeRanges.constBegin(); it != m_changeRanges.constEnd(); ++it) {
                cursor.setPosition((*it).first);
                cursor.setPosition((*it).second, QTextCursor::KeepAnchor);
                QTextCharFormat format = cursor.charFormat();
                int changeId = format.property(KoCharacterStyle::ChangeTrackerId).toInt();
                if (changeId == m_changeId) {
                    if (int parentChangeId = m_changeTracker->parent(m_changeId)) {
                        format.setProperty(KoCharacterStyle::ChangeTrackerId, parentChangeId);
                    } else {
                        format.clearProperty(KoCharacterStyle::ChangeTrackerId);
                    }
                    cursor.setCharFormat(format);
                }
            }
        } else {
            QList<QPair<int, int>>::const_iterator it;
            QStack<QPair<int, int>> deleteRanges;
            for (it = m_changeRanges.constBegin(); it != m_changeRanges.constEnd(); ++it) {
                deleteRanges.push(QPair<int, int>((*it).first, (*it).second));
            }
            while (!deleteRanges.isEmpty()) {
                QPair<int, int> range = deleteRanges.pop();
                cursor.setPosition(range.first);
                cursor.setPosition(range.second, QTextCursor::KeepAnchor);
                cursor.deleteChar();
            }
        }
        m_changeTracker->acceptRejectChange(m_changeId, true);
    } else {
        m_changeTracker->acceptRejectChange(m_changeId, true);
        KoTextCommandBase::redo();
        UndoRedoFinalizer finalizer(this);
    }
    Q_EMIT acceptRejectChange();
}

void AcceptChangeCommand::undo()
{
    m_changeTracker->acceptRejectChange(m_changeId, false);
    KoTextCommandBase::undo();
    UndoRedoFinalizer finalizer(this);
    Q_EMIT acceptRejectChange();
}
