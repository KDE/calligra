/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
*/

#include "cell.h"
#include "excel.h"
#include "format.h"
#include "value.h"
#include "objects.h"

#include <iostream>

using namespace Swinder;

Cell::Cell(Sheet* sheet, unsigned column, unsigned row)
    : m_sheet(sheet)
    , m_value(0)
    , m_formula(0)
    , m_note(0)
    , m_format(0)
    , m_row(row)
    , m_column(column)
    , m_rowSpan(1)
    , m_columnSpan(1)
    , m_columnRepeat(1)
    , m_covered(false)
{
}

Cell::~Cell()
{
    delete m_value;
    delete m_formula;
    delete m_note;
    //m_format is owned and destroyed by the Workbook
}

Sheet* Cell::sheet()
{
    return m_sheet;
}

unsigned Cell::column() const
{
    return m_column;
}

unsigned Cell::row() const
{
    return m_row;
}

QString Cell::name() const
{
    return name(column(), row());
}

QString Cell::name(unsigned column, unsigned row)
{
    return columnLabel(column) + QString::number(row);
}

QString Cell::columnLabel() const
{
    return columnLabel(column());
}

QString Cell::columnLabel(unsigned column)
{
    QString str;
    unsigned digits = 1;
    unsigned offset = 0;

    for (unsigned limit = 26; column >= limit + offset; limit *= 26, ++digits)
        offset += limit;

    for (unsigned c = column - offset; digits; --digits, c /= 26)
        str = QString(QChar('A' + (c % 26))) + str;

    return str;
}

// provide safety for overflows if an integer is explicit casted to an unsigned
QString Cell::columnLabel(int column)
{
    //Q_ASSERT(column >= 0);
    return columnLabel(unsigned(qMax(0, column)));
}

Value Cell::value() const
{
    return m_value ? *m_value : Value::empty();
}

void Cell::setValue(const Value& value)
{
    if (value.isEmpty()) {
        delete m_value;
        m_value = 0;
    } else {
        if (m_value)
            *m_value = value;
        else
            m_value = new Value(value);
    }
}

QString Cell::formula() const
{
    return m_formula ? *(m_formula) : QString();
}

void Cell::setFormula(const QString& formula)
{
    if (formula.isNull()) {
        delete m_formula;
        m_formula = 0;
    } else {
        if (m_formula)
            *(m_formula) = formula;
        else
            m_formula = new QString(formula);
    }
}

const Format& Cell::format() const
{
    static const Format null;
    if (!m_format) return null;
    return *(m_format);
}

void Cell::setFormat(const Format* format)
{
    m_format = format;
}

unsigned Cell::columnSpan() const
{
    return m_columnSpan;
}

void Cell::setColumnSpan(unsigned span)
{
    if (span < 1) return;
    m_columnSpan = span;
    // correctly set right border
    if (span > 1) {
        Cell* lastCell = m_sheet->cell(m_column + span - 1, m_row, false);
        if (lastCell) {
            Format curFormat = format();
            curFormat.borders().setRightBorder(lastCell->format().borders().rightBorder());
            setFormat(sheet()->workbook()->format(sheet()->workbook()->addFormat(curFormat)));
        }
    }
}

unsigned Cell::rowSpan() const
{
    return m_rowSpan;
}

void Cell::setRowSpan(unsigned span)
{
    if (span < 1) return;
    m_rowSpan = span;
    // correctly set bottom border
    if (span > 1) {
        Cell* lastCell = m_sheet->cell(m_column, m_row + span - 1, false);
        if (lastCell) {
            Format curFormat = format();
            curFormat.borders().setBottomBorder(lastCell->format().borders().bottomBorder());
            setFormat(sheet()->workbook()->format(sheet()->workbook()->addFormat(curFormat)));
        }
    }
}

bool Cell::isCovered() const
{
    return m_covered;
}

void Cell::setCovered(bool covered)
{
    m_covered = covered;
}

int Cell::columnRepeat() const
{
    return m_columnRepeat;
}

void Cell::setColumnRepeat(int repeat)
{
    m_columnRepeat = repeat;
}

bool Cell::hasHyperlink() const
{
    return m_sheet->hyperlink(m_column, m_row).isValid;
}

Hyperlink Cell::hyperlink() const
{
    return m_sheet->hyperlink(m_column, m_row);
}

void Cell::setHyperlink(const Hyperlink& link)
{
    m_sheet->setHyperlink(m_column, m_row, link);
}

QString Cell::note() const
{
    return m_note ? *(m_note) : QString();
}

void Cell::setNote(const QString &n)
{
    if (n.isNull()) {
        delete m_note;
        m_note = 0;
    } else {
        if (m_note)
            *(m_note) = n;
        else
            m_note = new QString(n);
    }
}

QList<ChartObject*> Cell::charts() const
{
    return m_sheet->charts(m_column, m_row);
}

void Cell::addChart(ChartObject* chart)
{
    m_sheet->addChart(m_column, m_row, chart);
}

QList<OfficeArtObject*> Cell::drawObjects() const
{
    return m_sheet->drawObjects(m_column, m_row);
}

void Cell::addDrawObject(OfficeArtObject* of)
{
    m_sheet->addDrawObject(m_column, m_row, of);
}

bool Cell::operator==(const Cell &other) const
{
    if (value() != other.value()) return false;
    if (formula() != other.formula()) return false;
    if (format() != other.format()) return false;
    if (columnSpan() != other.columnSpan()) return false;
    if (rowSpan() != other.rowSpan()) return false;
    if (isCovered() != other.isCovered()) return false;
    if (columnRepeat() != other.columnRepeat()) return false;

    if (hasHyperlink() != other.hasHyperlink()) return false;
    if (hasHyperlink() && hyperlink() != other.hyperlink()) return false;

    if (note() != other.note()) return false;

    if (charts().size() != other.charts().size()) return false;
    for(int i = charts().size() - 1; i >= 0; --i) {
        ChartObject* c1 = charts()[i];
        ChartObject* c2 = other.charts()[i];
        if(*c1 != *c2) return false;
    }

    if (drawObjects().size() != other.drawObjects().size()) return false;
    for(int i = drawObjects().size() - 1; i >= 0; --i) {
        OfficeArtObject* o1 = drawObjects()[i];
        OfficeArtObject* o2 = other.drawObjects()[i];
        if(*o1 != *o2) return false;
    }

    return true;
}

bool Cell::operator!=(const Cell &other) const
{
    return ! (*this == other);
}
