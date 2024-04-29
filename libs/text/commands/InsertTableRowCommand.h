// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef INSERTTABLEROWCOMMAND_H
#define INSERTTABLEROWCOMMAND_H

#include <KoTableRowStyle.h>
#include <kundo2command.h>

class KoTextEditor;
class QTextTable;

class InsertTableRowCommand : public KUndo2Command
{
public:
    InsertTableRowCommand(KoTextEditor *te, QTextTable *t, bool below, KUndo2Command *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    bool m_first;
    KoTextEditor *m_textEditor;
    QTextTable *m_table;
    int m_row;
    bool m_below;
    KoTableRowStyle m_style;
};

#endif // INSERTTABLEROWCOMMAND_H
