// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later/

#include "DeleteTableRowCommand.h"

#include "KoTableColumnAndRowStyleManager.h"
#include <KoTextEditor.h>

#include <QTextTable>
#include <QTextTableCell>

#include "TextDebug.h"
#include <KLocalizedString>

DeleteTableRowCommand::DeleteTableRowCommand(KoTextEditor *te, QTextTable *t, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_first(true)
    , m_textEditor(te)
    , m_table(t)
{
    setText(kundo2_i18n("Delete Row"));
}

void DeleteTableRowCommand::undo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);
    for (int i = 0; i < m_selectionRowSpan; ++i) {
        carsManager.insertRows(m_selectionRow + i, 1, m_deletedStyles.at(i));
    }
    KUndo2Command::undo();
}

void DeleteTableRowCommand::redo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);
    if (!m_first) {
        carsManager.removeRows(m_selectionRow, m_selectionRowSpan);
        KUndo2Command::redo();
    } else {
        m_first = false;
        int selectionColumn;
        int selectionColumnSpan;
        if (m_textEditor->hasComplexSelection()) {
            m_textEditor->cursor()->selectedTableCells(&m_selectionRow, &m_selectionRowSpan, &selectionColumn, &selectionColumnSpan);
        } else {
            QTextTableCell cell = m_table->cellAt(*m_textEditor->cursor());
            m_selectionRow = cell.row();
            m_selectionRowSpan = 1;
        }

        m_deletedStyles.reserve(m_selectionRowSpan);
        for (int i = m_selectionRow; i < m_selectionRow + m_selectionRowSpan; ++i) {
            m_deletedStyles.append(carsManager.rowStyle(i));
        }
        carsManager.removeRows(m_selectionRow, m_selectionRowSpan);

        m_table->removeRows(m_selectionRow, m_selectionRowSpan);
    }
}
