/*
 * This file is part of Office 2007 Filters for Calligra
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

#include <MsooXmlGlobal.h>
#include "XlsxXmlDrawingReader.h"

class Sheet;
class Cell;

class EmbeddedCellObjects
{
public:
    EmbeddedCellObjects(){}
    ~EmbeddedCellObjects(){ qDeleteAll(drawings); }
    QList<XlsxDrawingObject*> drawings;

    QList< QPair<QString,QString> > oleObjects;
    QList<QString> oleFrameBegins;
    QString hyperlink;
};

class Formula {
protected:
    explicit Formula() {}
public:
    virtual ~Formula() {}
    virtual bool isShared() const = 0;
};

class FormulaImpl : public Formula {
public:
    QString m_formula;
    explicit FormulaImpl(const QString &formula) : Formula(), m_formula(formula) {}
    bool isShared() const override { return false; }
    ~FormulaImpl() override {}
};

class SharedFormula : public Formula {
public:
    Cell *m_referencedCell;
    explicit SharedFormula(Cell *referencedCell) : Formula(), m_referencedCell(referencedCell) {}
    bool isShared() const override { return true; }
    ~SharedFormula() override {}
};

class Cell
{
public:
    void appendDrawing( XlsxDrawingObject* obj ){
        if (!embedded) {
            embedded = new EmbeddedCellObjects;
        }
        embedded->drawings.append( obj );
    }
    void appendOleObject( const QPair<QString,QString>& oleObject, const QString& oleFrameBegin ){
        if (!embedded) {
            embedded = new EmbeddedCellObjects;
        }
        embedded->oleObjects.append( oleObject );
        embedded->oleFrameBegins.append( oleFrameBegin );
    }
    void setHyperLink( const QString& link ) {
        if (!embedded) {
            embedded = new EmbeddedCellObjects;
        }
        embedded->hyperlink = link;
    }
    QList< QPair<QString,QString> > oleObjects() const {
        if (embedded) {
            return embedded->oleObjects;
        }
        else {
            return QList< QPair<QString,QString> >();
        }
    }
    QString hyperlink() const {
        if (embedded) {
            return embedded->hyperlink;
        }
        else {
            return QString();
        }
    }

    QString styleName;
    QString charStyleName;
    QString text;

    QString *valueAttrValue;

    Formula *formula;

    EmbeddedCellObjects* embedded;

    int column;
    int row;
    int rowsMerged;
    int columnsMerged;

    enum ValueType {
        ConstNone,
        ConstString,
        ConstBoolean,
        ConstDate,
        ConstFloat
    };
    ValueType valueType;

    enum ValueAttr {
        OfficeNone,
        OfficeValue,
        OfficeStringValue,
        OfficeBooleanValue,
        OfficeDateValue
    };
    ValueAttr valueAttr;

    bool isPlainText : 1;

    Cell(int columnIndex, int rowIndex) : valueAttrValue(0), formula(0), embedded(0), column(columnIndex), row(rowIndex), rowsMerged(1), columnsMerged(1), valueType(Cell::ConstNone), valueAttr(OfficeNone), isPlainText(true) {}
    ~Cell() { delete valueAttrValue; delete formula; delete embedded; }
};

class Row
{
public:
    QString styleName;
    int rowIndex;
    bool hidden : 1;

    Row(int index) : rowIndex(index), hidden(false) {}
    ~Row() {}
};

class Column
{
public:
    QString styleName;
    int columnIndex;
    bool hidden : 1;

    Column(int index) : columnIndex(index), hidden(false) {}
    ~Column() {}
};

class Sheet
{
public:
    QString m_name;
    double m_defaultRowHeight, m_defaultColWidth, m_baseColWidth;

    explicit Sheet(const QString &name) : m_name(name), m_defaultRowHeight(-1.0), m_defaultColWidth(-1.0), m_baseColWidth(-1.0), m_maxRow(0), m_maxColumn(0), m_visible(true) {}
    ~Sheet() { qDeleteAll(m_rows); qDeleteAll(m_columns); /*qDeleteAll(m_cells);*/ }

    Row* row(int rowIndex, bool autoCreate)
    {
        Row* r = m_rows[ rowIndex ];
        if (!r && autoCreate) {
            r = new Row(/*this,*/ rowIndex);
            m_rows[ rowIndex ] = r;
            if (rowIndex > m_maxRow) m_maxRow = rowIndex;
        }
        return r;
    }

    Column* column(int columnIndex, bool autoCreate)
    {
        Column* c = m_columns[ columnIndex ];
        if (!c && autoCreate) {
            c = new Column(/*this,*/ columnIndex);
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
            c = new Cell(columnIndex, rowIndex);
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

    bool visible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    QString pictureBackgroundPath() const { return m_pictureBackgroundPath; }
    void setPictureBackgroundPath(const QString& path) { m_pictureBackgroundPath = path; }

private:
    QHash<int, Row*> m_rows;
    QHash<int, Column*> m_columns;
    QHash<unsigned, Cell*> m_cells;
    QHash<int, int> m_maxCellsInRow;
    QString m_pictureBackgroundPath;
    int m_maxRow;
    int m_maxColumn;
    bool m_visible : 1;
};

#endif
