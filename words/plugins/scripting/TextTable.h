/* This file is part of the Calligra project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTING_TEXTTABLE_H
#define SCRIPTING_TEXTTABLE_H

#include <QObject>
#include <QTextTable>
#include <QTextTableCell>

#include "TextCursor.h"

namespace Scripting
{

/**
* The TextList provides a table of cells within a \a TextDocument .
*/
class TextTable : public QObject
{
    Q_OBJECT
public:
    TextTable(QObject* parent, QTextTable* table)
            : QObject(parent), m_table(table) {}
    virtual ~TextTable() {}

public Q_SLOTS:

    /** Return the table content as plain text. */
    QString text() {
        return QTextCursor(m_table).block().text();
    }

    /** Return the number of columns the table has. */
    int columns() const {
        return m_table->columns();
    }
    /** Return the number of rows the table has. */
    int rows() const {
        return m_table->rows();
    }

    /** Return a \a TextCursor object for the first position of the cell
    defined with \p row and \p column . */
    QObject* firstCursorPosition(int row, int column) {
        QTextTableCell cell = m_table->cellAt(row, column);
        return cell.isValid() ? new TextCursor(this, cell.firstCursorPosition()) : 0;
    }
    /** Return a \a TextCursor object for the last position of the cell
    defined with \p row and \p column . */
    QObject* lastCursorPosition(int row, int column) {
        QTextTableCell cell = m_table->cellAt(row, column);
        return cell.isValid() ? new TextCursor(this, cell.lastCursorPosition()) : 0;
    }

    /** Insert \p columns number of columns at the position \p index . */
    void insertColumns(int index, int columns) {
        m_table->insertColumns(index, columns);
    }
    /** Insert \p rows number of rows at the position \p index . */
    void insertRows(int index, int rows) {
        m_table->insertRows(index, rows);
    }
    /** Remove \p columns number of columns from the position \p index . */
    void removeColumns(int index, int columns) {
        m_table->removeColumns(index, columns);
    }
    /** Remove \p rows number of rows from the position \p index . */
    void removeRows(int index, int rows) {
        m_table->removeRows(index, rows);
    }
    /** Resize the table to \p rows and \p columns . */
    void resize(int rows, int columns) {
        m_table->resize(rows, columns);
    }

private:
    QTextTable* m_table;
};

}

#endif
