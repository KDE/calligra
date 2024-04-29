/*
 *  SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef REMOVEDELETECHANGESCOMMAND_H
#define REMOVEDELETECHANGESCOMMAND_H

#include <QPointer>
#include <kundo2command.h>

class QTextDocument;

class RemoveDeleteChangesCommand : public KUndo2Command
{
public:
    explicit RemoveDeleteChangesCommand(QTextDocument *document, KUndo2Command *parent = nullptr);
    void redo();

private:
    QPointer<QTextDocument> m_document;
    void removeDeleteChanges();
};

#endif // REMOVEDELETECHANGESCOMMAND_H
