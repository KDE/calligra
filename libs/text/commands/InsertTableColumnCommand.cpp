// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "InsertTableColumnCommand.h"

#include "KoTableColumnAndRowStyleManager.h"
#include <KoTextEditor.h>

#include <QTextTable>
#include <QTextTableCell>

#include "TextDebug.h"
#include <KLocalizedString>

InsertTableColumnCommand::InsertTableColumnCommand(KoTextEditor *te, QTextTable *t, bool right, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_first(true)
    , m_textEditor(te)
    , m_table(t)
    , m_right(right)
{
    if (right) {
        setText(kundo2_i18n("Insert Column Right"));
    } else {
        setText(kundo2_i18n("Insert Column Left"));
    }
}

void InsertTableColumnCommand::undo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);

    carsManager.removeColumns(m_column, 1);

    KUndo2Command::undo();
}

void InsertTableColumnCommand::redo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);
    if (!m_first) {
        carsManager.insertColumns(m_column, 1, m_style);
        KUndo2Command::redo();
    } else {
        m_first = false;
        QTextTableCell cell = m_table->cellAt(*m_textEditor->cursor());
        m_column = cell.column() + (m_right ? 1 : 0);
        m_style = carsManager.columnStyle(cell.column());
        m_table->insertColumns(m_column, 1);
        carsManager.insertColumns(m_column, 1, m_style);

        if (m_right && m_column == m_table->columns() - 1) {
            // Copy the cell style. for the bottomright cell which Qt doesn't
            QTextTableCell cell = m_table->cellAt(m_table->rows() - 1, m_column - 1);
            QTextCharFormat format = cell.format();
            cell = m_table->cellAt(m_table->rows() - 1, m_column);
            cell.setFormat(format);
        }
    }
}
