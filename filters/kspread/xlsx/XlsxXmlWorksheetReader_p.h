/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef XLSXXMLWORKSHEETREADER_P_H
#define XLSXXMLWORKSHEETREADER_P_H

//! @todo the workboot was designed after filters/kspread/excel/sidewinder to allow to shared as much
//!       logic as possible. Goal is to let them both share the same structures and OpenDocument logic
//!       some day.
class Sheet;

class Cell
{
public:
    Sheet* sheet;
    int column, row;
    int rowsMerged, columnsMerged;
    QString styleName;
    QString charStyleName;
    QString text;
    bool isPlainText;
    QString valueType;
    QByteArray valueAttr;
    QString valueAttrValue;
    QString formula;
    QString hyperlink;
    QList<XlsxXmlDrawingReaderContext*> drawings;

    //QPair< oleObjectFile, imageReplacementFile>
    QList< QPair<QString,QString> > oleObjects;

    Cell(Sheet* s, int columnIndex, int rowIndex) : sheet(s), column(columnIndex), row(rowIndex), rowsMerged(1), columnsMerged(1), isPlainText(true) {}
    ~Cell() { qDeleteAll(drawings); }
};

class Row
{
public:
    Sheet* sheet;
    int rowIndex;
    bool hidden;
    QString styleName;

    Row(Sheet* s, int index) : sheet(s), rowIndex(index), hidden(false) {}
    ~Row() {}
};

class Column
{
public:
    Sheet* sheet;
    int columnIndex;
    bool hidden;

    Column(Sheet* s, int index) : sheet(s), columnIndex(index), hidden(false) {}
    ~Column() {}
};

class Sheet
{
public:
    double m_defaultRowHeight, m_defaultColWidth, m_baseColWidth;
    explicit Sheet() : m_defaultRowHeight(-1.0), m_defaultColWidth(-1.0), m_baseColWidth(-1.0), m_maxRow(0), m_maxColumn(0), m_visible(true) {}
    ~Sheet() { qDeleteAll(m_rows); qDeleteAll(m_columns); qDeleteAll(m_cells); }

    Row* row(int rowIndex, bool autoCreate)
    {
        Row* r = m_rows[ rowIndex ];
        if (!r && autoCreate) {
            r = new Row(this, rowIndex);
            m_rows[ rowIndex ] = r;
            if (rowIndex > m_maxRow) m_maxRow = rowIndex;
        }
        return r;
    }

    Column* column(int columnIndex, bool autoCreate)
    {
        Column* c = m_columns[ columnIndex ];
        if (!c && autoCreate) {
            c = new Column(this, columnIndex);
            m_columns[ columnIndex ] = c;
            if (columnIndex > m_maxColumn) m_maxColumn = columnIndex;
        }
        return c;
    }

    Cell* cell(int columnIndex, int rowIndex, bool autoCreate)
    {
        const unsigned hashed = (rowIndex + 1) * MSOOXML::maximumSpreadsheetColumns() + columnIndex + 1;
        Cell* c = m_cells[ hashed ];
        if (!c && autoCreate) {
            c = new Cell(this, columnIndex, rowIndex);
            m_cells[ hashed ] = c;
            this->column(columnIndex, true);
            this->row(rowIndex, true);
            if (rowIndex > m_maxRow) m_maxRow = rowIndex;
            if (columnIndex > m_maxColumn) m_maxColumn = columnIndex;
            if (!m_maxCellsInRow.contains(rowIndex) || columnIndex > m_maxCellsInRow[rowIndex])
                m_maxCellsInRow[rowIndex] = columnIndex;
        }
        return c;
    }

    int maxRow() const { return m_maxRow; }
    int maxColumn() const { return m_maxColumn; }
    int maxCellsInRow(int rowIndex) const { return m_maxCellsInRow[rowIndex]; }

    bool visible() { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    QString pictureBackgroundPath() { return m_pictureBackgroundPath; }
    void setPictureBackgroundPath(const QString& path) { m_pictureBackgroundPath = path; }

private:
    QHash<int, Row*> m_rows;
    QHash<int, Column*> m_columns;
    QHash<unsigned, Cell*> m_cells;
    int m_maxRow;
    int m_maxColumn;
    QHash<int, int> m_maxCellsInRow;
    bool m_visible;
    QString m_pictureBackgroundPath;
};

#endif
