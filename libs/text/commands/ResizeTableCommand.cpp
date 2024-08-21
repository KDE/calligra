// This file is part of the KDE project
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ResizeTableCommand.h"

#include "KoTableColumnAndRowStyleManager.h"
#include "KoTableColumnStyle.h"
#include "KoTableRowStyle.h"

#include <QTextCursor>
#include <QTextDocument>
#include <QTextTable>

#include "TextDebug.h"
#include <KLocalizedString>

ResizeTableCommand::ResizeTableCommand(QTextTable *t, bool horizontal, int band, qreal size, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_first(true)
    , m_tablePosition(t->firstPosition())
    , m_document(t->document())
    , m_horizontal(horizontal)
    , m_band(band)
    , m_size(size)
    , m_oldColumnStyle(nullptr)
    , m_oldRowStyle(nullptr)
{
    if (horizontal) {
        setText(kundo2_i18n("Adjust Column Width"));
    } else {
        setText(kundo2_i18n("Adjust Row Height"));
    }
}

ResizeTableCommand::~ResizeTableCommand()
{
    delete m_oldColumnStyle;
    delete m_oldRowStyle;
}

void ResizeTableCommand::undo()
{
    QTextCursor c(m_document);
    c.setPosition(m_tablePosition);
    QTextTable *table = c.currentTable();

    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(table);

    if (m_oldColumnStyle) {
        KoTableColumnStyle style = carsManager.columnStyle(m_band);
        style.copyProperties(m_oldColumnStyle);
        carsManager.setColumnStyle(m_band, style);
    }
    if (m_oldRowStyle) {
        KoTableRowStyle style = carsManager.rowStyle(m_band);
        style.copyProperties(m_oldRowStyle);
        carsManager.setRowStyle(m_band, style);
    }
    KUndo2Command::undo();
    m_document->markContentsDirty(m_tablePosition, table->lastPosition() - table->firstPosition());
}

void ResizeTableCommand::redo()
{
    QTextCursor c(m_document);
    c.setPosition(m_tablePosition);
    QTextTable *table = c.currentTable();

    KoTableColumnAndRowStyleManager carsManager = KoTableColumnAndRowStyleManager::getManager(table);

    if (!m_first) {
        if (m_horizontal) {
            KoTableColumnStyle style = carsManager.columnStyle(m_band);
            style.copyProperties(m_newColumnStyle);
            carsManager.setColumnStyle(m_band, style);
        } else {
            KoTableRowStyle style = carsManager.rowStyle(m_band);
            style.copyProperties(m_newRowStyle);
            carsManager.setRowStyle(m_band, style);
        }
        KUndo2Command::redo();
    } else {
        m_first = false;
        if (m_horizontal) {
            m_oldColumnStyle = carsManager.columnStyle(m_band).clone();
            // make sure the style is set (could have been a default style)
            carsManager.setColumnStyle(m_band, carsManager.columnStyle(m_band));

            KoTableColumnStyle style = carsManager.columnStyle(m_band);
            style.setColumnWidth(m_size);
            carsManager.setColumnStyle(m_band, style);

            m_newColumnStyle = carsManager.columnStyle(m_band).clone();
        } else {
            m_oldRowStyle = carsManager.rowStyle(m_band).clone();

            // make sure the style is set (could have been a default style)
            carsManager.setRowStyle(m_band, carsManager.rowStyle(m_band));

            KoTableRowStyle style = carsManager.rowStyle(m_band);
            style.setMinimumRowHeight(m_size);
            carsManager.setRowStyle(m_band, style);

            m_newRowStyle = carsManager.rowStyle(m_band).clone();
        }
    }
    m_document->markContentsDirty(m_tablePosition, table->lastPosition() - table->firstPosition());
}
