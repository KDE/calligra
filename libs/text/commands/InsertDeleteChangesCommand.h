/*
 *  SPDX-FileCopyrightText: 2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef INSERTDELETECHANGESCOMMAND_H
#define INSERTDELETECHANGESCOMMAND_H

#include <QPointer>
#include <kundo2command.h>

class QTextDocument;

class InsertDeleteChangesCommand : public KUndo2Command
{
public:
    explicit InsertDeleteChangesCommand(QTextDocument *document, KUndo2Command *parent = nullptr);
    void redo();

private:
    QPointer<QTextDocument> m_document;
    void insertDeleteChanges();
};

#endif // INSERTDELETECHANGESCOMMAND_H
