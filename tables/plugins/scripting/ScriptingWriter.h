/*
 * This file is part of KSpread
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGWRITER_H
#define SCRIPTINGWRITER_H

#include "ScriptingModule.h"

#include <QString>
#include <QObject>
#include <klocale.h>

#include <part/Doc.h>
#include <Sheet.h>
#include <Map.h>
#include <Region.h>
#include <Cell.h>
#include <Value.h>

/**
* The ScriptingWriter class provides abstract high-level functionality to write
* content to KSpread sheets and to manipulate the content of cells.
*
* The following python sample demonstrates how to use the ScriptingWriter to
* write content to KSpread.
* \code
* # Import the KSpread module
* import KSpread
* # Create a writer instance.
* writer = KSpread.writer()
* # Set the sheet we like to write to.
* sheetname = "Sheet2"
* if not writer.setSheet(sheetname):
*     raise "Invalid sheet \"%s\" defined." % sheetname
* # Set the cell(s) we like to start to write to. If this
* # is not defined, writing starts from A1.
* cellname = "B12"
* if not writer.setCell(cellname):
*     raise "Invalid cell \"%s\" defined." % cellname
* # Now fill the cells with some content.
* for record in [ ["One,"Two"] , ["Three,"Four"] ]:
*     # Write the tuple to the current row.
*     if not writer.setValues(record):
*         raise "Failed to set record %s" % record
*     # Go to the next row.
*     writer.next()
* \endcode
*/
class ScriptingWriter : public QObject
{
    Q_OBJECT
public:

    /**
    * Constructor.
    * \param module The \a ScriptingModule instance that should be used
    * as datasource for this writer. The writer will operate on the
    * document those module provides us.
    */
    explicit ScriptingWriter(ScriptingModule* module) : QObject(module), m_module(module), m_cell(0) {
        clearAll();
    }

    /**
    * Destructor.
    */
    virtual ~ScriptingWriter() {}

public Q_SLOTS:

    /**
    * \return the current sheetname the writer is on. All operations done with
    * the writer are done on this sheet.
    */
    QString sheet() {
        return m_sheet ? m_sheet->sheetName() : QString();
    }

    /**
    * Set the current sheetname the writer is on to \p sheetname . If there exist
    * no sheet with such a sheetname false is returned else, so on success, true
    * is returned.
    */
    bool setSheet(const QString& sheetname) {
        KSpread::Sheet* s = m_module->kspreadDoc()->map()->findSheet(sheetname);
        if (! s) return false;
        clearAll();
        m_sheet = s;
        return true;
    }

    /**
    * \return the current cellname the writer is on. Operations like for example
    * the value() and setValue() methods are done on the defined sheet in the
    * defined cell. You may like to use it to manipulate the content of an
    * explicit cell.
    */
    QString cell() {
        return KSpread::Cell::name(m_column, m_row);
    }

    /**
    * Set the current cellname the writer is on to \p cellname . If such a cell
    * exist true is returned else, e.g. if the cellname was just wrong, false got
    * returned.
    */
    bool setCell(const QString& cellname) {
        if (! m_sheet) return false;
        const KSpread::Region region(cellname, m_sheet->doc()->map(), m_sheet);
        if (region.firstRange().isNull()) return false;
        QPoint point = region.firstRange().topLeft();
        m_column = point.x();
        m_row = point.y();
        clearCell();
        return true;
    }

    /**
    * Return the current row number.
    */
    int row() {
        return m_row;
    }

    /**
    * Set the current row number to \p rownumber .
    */
    void setRow(int rownumber) {
        m_row = rownumber;
        clearCell();
    }

    /**
    * Return the current column number.
    */
    int column() {
        return m_column;
    }

    /**
    * Set the current column number to \p columnnumber .
    */
    void setColumn(int columnnumber) {
        m_column = columnnumber;
        clearCell();
    }

    //int cellCount() {}
    //int rowCount() {}
    //int columnCount() {}

    //enum Filter { Any, NotEmpty, Numeric, Date, Time, String, Formula, Comment };
    //QString filter() {}
    //void setFilter(const QString& filter) {}

    //enum Direction { Cell, Row, Column };
    //QString direction() {}
    //void setDirection(const QString& direction) {}

    //void first() {}
    //void last() {}
    //void prev() {}

    /**
    * Go to the next row.
    */
    void next() {
        m_row++;
        clearCell();
    }

    //QVariant value() {}
    //QVariantList values() {}

    /**
    * Set the value of the current cell.
    *
    * \param value The value that should be set.
    * \param parse If this is true, the default, then the
    * value got parsed to look for the type else we
    * assume the value has the correct type.
    * \return true if the value was set successful else
    * false is returned.
    */
    bool setValue(const QVariant& value, bool parse = true) {
        KSpread::Value v;
        if (parse)
            v = KSpread::Value(value.toString());
        else {
            const KSpread::CalculationSettings* settings = m_module->kspreadDoc()->map()->calculationSettings();
            switch (value.type()) {
            case QVariant::Invalid:     v = KSpread::Value(); break;
            case QVariant::Bool:        v = KSpread::Value(value.toBool()); break;
            case QVariant::Int:         v = KSpread::Value(value.toInt()); break;
            case QVariant::ULongLong:   v = KSpread::Value(value.toLongLong()); break;
            case QVariant::Double:      v = KSpread::Value(value.toDouble()); break;
            case QVariant::String:      v = KSpread::Value(value.toString()); break;
            case QVariant::Date:        v = KSpread::Value(value.toDate(), settings); break;
            case QVariant::Time:        v = KSpread::Value(value.toTime(), settings); break;
            case QVariant::DateTime:    v = KSpread::Value(value.toDateTime(), settings); break;
            default: return false;
            }
        }
        //KSpread::Cell* c = getCell();
        //c->setValue(v);
        KSpread::Cell cell(m_sheet, m_column, m_row);
        if (!parse) {
            cell.setUserInput(value.toString());
            cell.setValue(v);
        } else {
            cell.parseUserInput(value.toString());
        }
        return true;
    }

    /**
    * Set the values of the cells in the current row.
    *
    * \param values The list of values that should be set.
    * \param parse If this is true, the default, then the
    * value got parsed to look for the type else we
    * assume the value has the correct type.
    * \return true if the values got set successful else
    * false is returned.
    */
    bool setValues(const QVariantList& values, bool parse = true) {
        bool ok = true;
        const int prevcolumn = m_column;
        m_module->doc()->beginMacro(i18n("Set Values"));
        foreach(QVariant v, values) {
            if (! setValue(v, parse)) ok = false;
            m_column++;
            clearCell();
        }
        m_module->doc()->endMacro();
        m_column = prevcolumn;
        return ok;
    }
    //bool insertValue(const QVariant& value) {}
    //bool insertValues(const QVariantList& value) {}

private:
    Q_DISABLE_COPY(ScriptingWriter)

    ScriptingModule* const m_module;
    KSpread::Sheet* m_sheet;
    int m_column, m_row;
    KSpread::Cell* m_cell;

    KSpread::Cell* getCell() {
        Q_ASSERT(m_sheet);
        if (m_cell) return m_cell;
        m_cell = new KSpread::Cell(m_sheet, m_column, m_row);
        return m_cell;
    }

    void clearCell() {
        delete m_cell;
        m_cell = 0;
    }

    void clearAll() {
        m_sheet = 0;
        m_column = 0;
        m_row = 0;
        clearCell();
    }
};

#endif
