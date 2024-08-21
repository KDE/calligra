/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoTable.h"
#include "KoCell.h"
#include "KoColumn.h"
#include "KoRow.h"

#include <KoXmlWriter.h>

KOSTYLE_DECLARE_SHARED_POINTER_IMPL(KoTable)

KoTable::KoTable()
    : m_columns()
    , m_rows()
    , m_cells()
    , m_rowCount()
    , m_columnCount()
    , m_style(nullptr)
    , m_printable(true)
    , m_name()
    , m_protected(false)
    , m_protectionKey()
{
}

KoTable::~KoTable()
{
    qDeleteAll(m_rows);
    qDeleteAll(m_columns);
    qDeleteAll(m_cells);
}

KoCell *KoTable::cellAt(int row, int column)
{
    const QPair<int, int> key(row, column);
    KoCell *cell = m_cells.value(key);

    if (!cell) {
        cell = new KoCell();
        m_cells.insert(key, cell);
        m_columnCount = qMax(column + 1, m_columnCount);
        m_rowCount = qMax(row + 1, m_rowCount);
    }

    return cell;
}

KoColumn *KoTable::columnAt(int column)
{
    KoColumn *columnAt = m_columns.value(column);

    if (!columnAt) {
        columnAt = new KoColumn();
        if (column >= m_columns.size()) {
            m_columns.resize(column + 1);
        }
        m_columns.insert(column, columnAt);
        m_columnCount = qMax(column + 1, m_columnCount);
    }

    return columnAt;
}

KoRow *KoTable::rowAt(int row)
{
    KoRow *rowAt = m_rows.value(row);

    if (!rowAt) {
        rowAt = new KoRow();
        if (row >= m_rows.size()) {
            m_rows.resize(row + 1);
        }
        m_rows.replace(row, rowAt);
        m_rowCount = qMax(row + 1, m_rowCount);
    }

    return rowAt;
}

int KoTable::columnCount() const
{
    return m_columnCount;
}

int KoTable::rowCount() const
{
    return m_rowCount;
}

void KoTable::saveOdf(KoXmlWriter &writer, KoGenStyles &styles)
{
    writer.startElement("table:table");

    writer.addAttribute("table:name", m_name);
    writer.addAttribute("table:protected", m_protected ? "true" : "false");
    if (!m_protectionKey.isEmpty()) {
        writer.addAttribute("table:protection-key", m_protectionKey);
    }
    if (!m_protectionAlgorithm.isEmpty()) {
        writer.addAttribute("table:protection-key-digest-algorithm", m_protectionAlgorithm);
    }

    if (m_style) {
        m_style->setName(m_style->saveOdf(styles));
        writer.addAttribute("table:style-name", m_style->name());
    }

    {
        KoColumn defaultColumn;
        for (int c = 0; c < columnCount(); ++c) {
            KoColumn *column = m_columns.value(c);
            if (column) {
                column->saveOdf(writer, styles);
            } else {
                defaultColumn.saveOdf(writer, styles);
            }
        }
    }

    {
        KoRow defaultRow;
        for (int r = 0; r < rowCount(); ++r) {
            KoRow *row = m_rows.value(r);
            if (row) {
                row->saveOdf(writer, styles);

                KoCell defaultCell;
                for (int c = 0; c < columnCount(); ++c) {
                    KoCell *cell = m_cells.value(QPair<int, int>(r, c));
                    if (cell) {
                        cell->saveOdf(writer, styles);
                    } else {
                        defaultCell.saveOdf(writer, styles);
                    }
                }

                row->finishSaveOdf(writer, styles);
            } else {
                defaultRow.saveOdf(writer, styles);

                KoCell defaultCell;
                for (int c = 0; c < columnCount(); ++c) {
                    KoCell *cell = m_cells.value(QPair<int, int>(r, c));
                    if (cell) {
                        cell->saveOdf(writer, styles);
                    } else {
                        defaultCell.saveOdf(writer, styles);
                    }
                }

                defaultRow.finishSaveOdf(writer, styles);
            }
        }
    }

    writer.endElement(); // table:table
}

void KoTable::setName(const QString &name)
{
    m_name = name;
}

QString KoTable::name() const
{
    return m_name;
}

void KoTable::setPrintable(bool printable)
{
    m_printable = printable;
}

bool KoTable::printable() const
{
    return m_printable;
}

void KoTable::setTableStyle(KoTblStyle::Ptr style)
{
    m_style = style;
}

KoTblStyle::Ptr KoTable::tableStyle()
{
    return m_style;
}

void KoTable::setProtected(bool isProtected)
{
    m_protected = isProtected;
}

void KoTable::setProtectionKey(const QString &password, const QString &protectionAlgorithmUri)
{
    m_protectionKey = password;
    m_protectionAlgorithm = protectionAlgorithmUri;
}

QString KoTable::protectionKey() const
{
    return m_protectionKey;
}

QString KoTable::protectionalgorithm() const
{
    return m_protectionAlgorithm;
}

bool KoTable::isPprotected() const
{
    return m_protected;
}
