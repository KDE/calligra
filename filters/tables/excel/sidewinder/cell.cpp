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

namespace Swinder
{

class Cell::Private
{
public:
    Sheet* sheet;
    Value* value;
    QString* formula;
    QString* note;
    const Format* format;

    unsigned row : 21; // KS_rowMax
    unsigned column : 17; // KS_colMax
    unsigned rowSpan : 21;
    unsigned columnSpan : 17;
    unsigned columnRepeat : 17;
    bool covered : 1;
};

}

using namespace Swinder;

Cell::Cell(Sheet* sheet, unsigned column, unsigned row)
{
    d = new Cell::Private();
    d->sheet      = sheet;
    d->value      = 0;
    d->formula    = 0;
    d->note       = 0;
    d->format     = 0;
    d->row        = row;
    d->column     = column;
    d->rowSpan    = 1;
    d->columnSpan = 1;
    d->columnRepeat = 1;
    d->covered    = false;
}

Cell::~Cell()
{
    delete d->formula;
    delete d->note;
    delete d;
}

Sheet* Cell::sheet()
{
    return d->sheet;
}

unsigned Cell::column() const
{
    return d->column;
}

unsigned Cell::row() const
{
    return d->row;
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

    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
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
    return d->value ? *d->value : Value::empty();
}

void Cell::setValue(const Value& value)
{
    if (value.isEmpty()) {
        delete d->value;
        d->value = 0;
    } else {
        if (d->value)
            *d->value = value;
        else
            d->value = new Value(value);
    }
}

QString Cell::formula() const
{
    return d->formula ? *(d->formula) : QString();
}

void Cell::setFormula(const QString& formula)
{
    if (formula.isNull()) {
        delete d->formula;
        d->formula = 0;
    } else {
        if (d->formula)
            *(d->formula) = formula;
        else
            d->formula = new QString(formula);
    }
}

const Format& Cell::format() const
{
    static const Format null;
    if (!d->format) return null;
    return *(d->format);
}

void Cell::setFormat(const Format* format)
{
    d->format = format;
}

unsigned Cell::columnSpan() const
{
    return d->columnSpan;
}

void Cell::setColumnSpan(unsigned span)
{
    if (span < 1) return;
    d->columnSpan = span;
    // correctly set right border
    if (span > 1) {
        Cell* lastCell = d->sheet->cell(d->column + span - 1, d->row, false);
        if (lastCell) {
            Format curFormat = format();
            curFormat.borders().setRightBorder(lastCell->format().borders().rightBorder());
            setFormat(sheet()->workbook()->format(sheet()->workbook()->addFormat(curFormat)));
        }
    }
}

unsigned Cell::rowSpan() const
{
    return d->rowSpan;
}

void Cell::setRowSpan(unsigned span)
{
    if (span < 1) return;
    d->rowSpan = span;
    // correctly set bottom border
    if (span > 1) {
        Cell* lastCell = d->sheet->cell(d->column, d->row + span - 1, false);
        if (lastCell) {
            Format curFormat = format();
            curFormat.borders().setBottomBorder(lastCell->format().borders().bottomBorder());
            setFormat(sheet()->workbook()->format(sheet()->workbook()->addFormat(curFormat)));
        }
    }
}

bool Cell::isCovered() const
{
    return d->covered;
}

void Cell::setCovered(bool covered)
{
    d->covered = covered;
}

int Cell::columnRepeat() const
{
    return d->columnRepeat;
}

void Cell::setColumnRepeat(int repeat)
{
    d->columnRepeat = repeat;
}

bool Cell::hasHyperlink() const
{
    return d->sheet->hyperlink(d->column, d->row).isValid;
}

Hyperlink Cell::hyperlink() const
{
    return d->sheet->hyperlink(d->column, d->row);
}

void Cell::setHyperlink(const Hyperlink& link)
{
    d->sheet->setHyperlink(d->column, d->row, link);
}

QString Cell::note() const
{
    return d->note ? *(d->note) : QString();
}

void Cell::setNote(const QString &n)
{
    if (n.isNull()) {
        delete d->note;
        d->note = 0;
    } else {
        if (d->note)
            *(d->note) = n;
        else
            d->note = new QString(n);
    }
}

QList<ChartObject*> Cell::charts() const
{
    return d->sheet->charts(d->column, d->row);
}

void Cell::addChart(ChartObject* chart)
{
    d->sheet->addChart(d->column, d->row, chart);
}

QList<OfficeArtObject*> Cell::drawObjects() const
{
    return d->sheet->drawObjects(d->column, d->row);
}

void Cell::addDrawObject(OfficeArtObject* of)
{
    d->sheet->addDrawObject(d->column, d->row, of);
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
