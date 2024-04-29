// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef RESIZETABLECOMMAND_H
#define RESIZETABLECOMMAND_H

#include <kundo2command.h>

class QTextDocument;
class QTextTable;
class KoTableColumnStyle;
class KoTableRowStyle;

class ResizeTableCommand : public KUndo2Command
{
public:
    ResizeTableCommand(QTextTable *t, bool horizontal, int band, qreal size, KUndo2Command *parent = nullptr);
    ~ResizeTableCommand() override;

    void undo() override;
    void redo() override;

private:
    bool m_first;
    int m_tablePosition;
    QTextDocument *m_document;
    bool m_horizontal;
    int m_band;
    qreal m_size;
    KoTableColumnStyle *m_oldColumnStyle;
    KoTableColumnStyle *m_newColumnStyle;
    KoTableRowStyle *m_oldRowStyle;
    KoTableRowStyle *m_newRowStyle;
};

#endif // RESIZETABLECOMMAND_H
