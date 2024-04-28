// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "InsertTableRowCommand.h"

#include "KoTableColumnAndRowStyleManager.h"
#include <KoTextEditor.h>

#include <QTextTable>
#include <QTextTableCell>

#include "TextDebug.h"
#include <KLocalizedString>

InsertTableRowCommand::InsertTableRowCommand(KoTextEditor *te, QTextTable *t, bool below, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_first(true)
    , m_textEditor(te)
    , m_table(t)
    , m_below(below)
{
    if (below) {
        setText(kundo2_i18n("Insert Row Below"));
    } else {
        setText(kundo2_i18n("Insert Row Above"));
    }
}

void InsertTableRowCommand::undo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);

    carsManager.removeRows(m_row, 1);

    KUndo2Command::undo();
}

void InsertTableRowCommand::redo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);
    if (!m_first) {
        carsManager.insertRows(m_row, 1, m_style);
        KUndo2Command::redo();
    } else {
        m_first = false;
        QTextTableCell cell = m_table->cellAt(*m_textEditor->cursor());
        m_row = cell.row() + (m_below ? 1 : 0);
        m_style = carsManager.rowStyle(cell.row());
        m_table->insertRows(m_row, 1);
        carsManager.insertRows(m_row, 1, m_style);

        if (m_below && m_row == m_table->rows() - 1) {
            // Copy the cells styles. when Qt doesn't do it for us
            for (int col = 0; col < m_table->columns(); ++col) {
                QTextTableCell cell = m_table->cellAt(m_row - 1, col);
                QTextCharFormat format = cell.format();
                cell = m_table->cellAt(m_row, col);
                cell.setFormat(format);
            }
        }
    }
}
