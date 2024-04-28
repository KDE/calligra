/*
 *  SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "InsertDeleteChangesCommand.h"

#include "KoChangeTrackerElement.h"
#include <KoChangeTracker.h>
#include <KoTextDocument.h>

#include <QTextDocument>
#include <QVector>

#include <algorithm>

InsertDeleteChangesCommand::InsertDeleteChangesCommand(QTextDocument *document, KUndo2Command *parent)
    : KUndo2Command("Insert Delete Changes", parent)
    , m_document(document)
{
}

void InsertDeleteChangesCommand::redo()
{
    insertDeleteChanges();
}

void InsertDeleteChangesCommand::insertDeleteChanges()
{
    int numAddedChars = 0;
    QVector<KoChangeTrackerElement *> elementVector;
    KoTextDocument(m_document).changeTracker()->getDeletedChanges(elementVector);
    std::sort(elementVector.begin(), elementVector.end());
}
