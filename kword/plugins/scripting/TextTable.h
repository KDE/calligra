/* This file is part of the KOffice project
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

namespace Scripting {

    /**
    *
    */
    class TextTable : public QObject
    {
            Q_OBJECT
        public:
            TextTable( QObject* parent, QTextTable* table )
                : QObject( parent ), m_table( table ) {}
            virtual ~TextTable() {}

        public Q_SLOTS:

            int columns() const { return m_table->columns(); }
            int rows() const { return m_table->rows(); }

            QObject* firstCursorPosition(int row, int column) {
                QTextTableCell cell = m_table->cellAt(row, column);
                return cell.isValid() ? new TextCursor(this, cell.firstCursorPosition()) : 0;
            }
            QObject* lastCursorPosition(int row, int column) {
                QTextTableCell cell = m_table->cellAt(row, column);
                return cell.isValid() ? new TextCursor(this, cell.lastCursorPosition()) : 0;
            }

            void insertColumns(int index, int columns) { m_table->insertColumns(index, columns); }
            void insertRows(int index, int rows) { m_table->insertRows(index, rows); }
            void removeColumns(int index, int columns) { m_table->removeColumns(index, columns); }
            void removeRows(int index, int rows) { m_table->removeRows(index, rows); }
            void resize(int rows, int columns) { m_table->resize(rows, columns); }

        private:
            QTextTable* m_table;
    };

}

#endif
