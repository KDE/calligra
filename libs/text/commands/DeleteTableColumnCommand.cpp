// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "DeleteTableColumnCommand.h"

#include "KoTableColumnAndRowStyleManager.h"
#include <KoTextEditor.h>

#include <QTextTable>
#include <QTextTableCell>

#include "TextDebug.h"
#include <KLocalizedString>

DeleteTableColumnCommand::DeleteTableColumnCommand(KoTextEditor *te, QTextTable *t, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_first(true)
    , m_textEditor(te)
    , m_table(t)
{
    setText(kundo2_i18n("Delete Column"));
}

void DeleteTableColumnCommand::undo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);
    for (int i = 0; i < m_selectionColumnSpan; ++i) {
        carsManager.insertColumns(m_selectionColumn + i, 1, m_deletedStyles.at(i));
    }

    KUndo2Command::undo();
}

void DeleteTableColumnCommand::redo()
{
    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(m_table);
    if (!m_first) {
        carsManager.removeColumns(m_selectionColumn, m_selectionColumnSpan);
        KUndo2Command::redo();
    } else {
        m_first = false;
        int selectionRow;
        int selectionRowSpan;
        if (m_textEditor->hasComplexSelection()) {
            m_textEditor->cursor()->selectedTableCells(&selectionRow, &selectionRowSpan, &m_selectionColumn, &m_selectionColumnSpan);
        } else {
            QTextTableCell cell = m_table->cellAt(*m_textEditor->cursor());
            m_selectionColumn = cell.column();
            m_selectionColumnSpan = 1;
        }

        m_table->removeColumns(m_selectionColumn, m_selectionColumnSpan);

        m_deletedStyles.reserve(m_selectionColumnSpan);
        for (int i = m_selectionColumn; i < m_selectionColumn + m_selectionColumnSpan; ++i) {
            m_deletedStyles.append(carsManager.columnStyle(i));
        }
        carsManager.removeColumns(m_selectionColumn, m_selectionColumnSpan);
    }
}
