// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef INSERTTABLECOLUMNCOMMAND_H
#define INSERTTABLECOLUMNCOMMAND_H

#include <KoTableColumnStyle.h>
#include <kundo2command.h>

class KoTextEditor;
class QTextTable;

class InsertTableColumnCommand : public KUndo2Command
{
public:
    InsertTableColumnCommand(KoTextEditor *te, QTextTable *t, bool right, KUndo2Command *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    bool m_first;
    KoTextEditor *m_textEditor;
    QTextTable *m_table;
    int m_column;
    bool m_right;
    KoTableColumnStyle m_style;
};

#endif // INSERTTABLECOLUMNCOMMAND_H
