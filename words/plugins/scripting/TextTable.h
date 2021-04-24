/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
