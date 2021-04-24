/*
 *  SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef REMOVEDELETECHANGESCOMMAND_H
#define REMOVEDELETECHANGESCOMMAND_H

#include <kundo2command.h>
#include <QPointer>

class QTextDocument;

class RemoveDeleteChangesCommand : public KUndo2Command
{
public:
    explicit RemoveDeleteChangesCommand(QTextDocument *document, KUndo2Command *parent = 0);
    void redo();

private:
    QPointer<QTextDocument> m_document;
    void removeDeleteChanges();
};

#endif // REMOVEDELETECHANGESCOMMAND_H
