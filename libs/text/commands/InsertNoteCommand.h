// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef INSERTNOTECOMMAND_H
#define INSERTNOTECOMMAND_H

#include "KoInlineNote.h"

#include <kundo2command.h>

#include <QPointer>

class QTextDocument;

class InsertNoteCommand : public KUndo2Command
{
public:
    InsertNoteCommand(KoInlineNote::Type type, QTextDocument *document);
    ~InsertNoteCommand() override;

    void undo() override;
    void redo() override;

    KoInlineNote *m_inlineNote;

private:
    QPointer<QTextDocument> m_document;
    bool m_first;
    int m_framePosition; // a cursor position inside the frame at the time of creation
};

#endif // INSERTNODECOMMAND_H
