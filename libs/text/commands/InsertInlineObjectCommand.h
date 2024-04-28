/*
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef INSERTINLINEOBJECTCOMMAND_H
#define INSERTINLINEOBJECTCOMMAND_H

#include <kundo2command.h>

class KoInlineObject;
class QTextDocument;

class InsertInlineObjectCommand : public KUndo2Command
{
public:
    InsertInlineObjectCommand(KoInlineObject *inlineObject, QTextDocument *document, KUndo2Command *parent);
    ~InsertInlineObjectCommand() override;

    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;

private:
    KoInlineObject *m_inlineObject;
    QTextDocument *m_document;
    bool m_deleteInlineObject;
    bool m_first;
    int m_position;
};

#endif // INSERTINLINEOBJECTCOMMAND_H
