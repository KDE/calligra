/*
 *  SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "RemoveDeleteChangesCommand.h"

#include <QTextDocument>
#include <QVector>

#include <KoChangeTracker.h>
#include <KoChangeTrackerElement.h>
#include <KoTextDocument.h>

#include <algorithm>

RemoveDeleteChangesCommand::RemoveDeleteChangesCommand(QTextDocument *document, KUndo2Command *parent)
    : KUndo2Command("Insert Delete Changes", parent)
    , m_document(document)
{
}

void RemoveDeleteChangesCommand::redo()
{
    removeDeleteChanges();
}

void RemoveDeleteChangesCommand::removeDeleteChanges()
{
    int numDeletedChars = 0;
    QVector<KoChangeTrackerElement *> elementVector;
    KoTextDocument(m_document).changeTracker()->getDeletedChanges(elementVector);
    std::sort(elementVector.begin(), elementVector.end());
}
