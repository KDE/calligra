// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef DELETETABLEROWCOMMAND_H
#define DELETETABLEROWCOMMAND_H

#include <KoTableRowStyle.h>
#include <QVector>
#include <kundo2command.h>

class KoTextEditor;
class QTextTable;

class DeleteTableRowCommand : public KUndo2Command
{
public:
    DeleteTableRowCommand(KoTextEditor *te, QTextTable *t, KUndo2Command *parent = nullptr);

    void undo() override;
    void redo() override;

private:
    bool m_first;
    KoTextEditor *m_textEditor;
    QTextTable *m_table;
    int m_selectionRow;
    int m_selectionRowSpan;
    QVector<KoTableRowStyle> m_deletedStyles;
};

#endif // DELETETABLEROWCOMMAND_H
