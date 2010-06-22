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
#include "ustring.h"
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
    unsigned row;
    unsigned column;
    Value value;
    UString formula;
    Format format;
    unsigned columnSpan;
    unsigned rowSpan;
    bool covered;
    int columnRepeat;
    bool hasHyperlink;
    UString hyperlinkDisplayName;
    UString hyperlinkLocation;
    UString hyperlinkTargetFrameName;
    UString note;
    std::vector<Picture*> pictures;
    std::vector<ChartObject*> charts;
};

}

using namespace Swinder;

Cell::Cell(Sheet* sheet, unsigned column, unsigned row)
{
    d = new Cell::Private();
    d->sheet      = sheet;
    d->column     = column;
    d->row        = row;
    d->value      = Value::empty();
    d->columnSpan = 1;
    d->rowSpan    = 1;
    d->covered    = false;
    d->columnRepeat = 1;
    d->hasHyperlink = false;
}

Cell::~Cell()
{
    qDeleteAll(d->pictures);
    qDeleteAll(d->charts);
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

UString Cell::name() const
{
    return name(column(), row());
}

UString Cell::name(unsigned column, unsigned row)
{
    return columnLabel(column) + UString::from(row);
}

UString Cell::columnLabel() const
{
    return columnLabel(column());
}

// FIXME be careful for integer overflow
UString Cell::columnLabel(unsigned column)
{
    UString str;
    unsigned digits = 1;
    unsigned offset = 0;

    for (unsigned limit = 26; column >= limit + offset; limit *= 26, digits++)
        offset += limit;

    for (unsigned c = column - offset; digits; --digits, c /= 26)
        str = UString(UChar('A' + (c % 26))) + str;

    return str;
}

Value Cell::value() const
{
    return d->value;
}

void Cell::setValue(const Value& value)
{
    d->value = value;
}

UString Cell::formula() const
{
    return d->formula;
}

void Cell::setFormula(const UString& formula)
{
    d->formula = formula;
}

Format Cell::format() const
{
    return d->format;
}

void Cell::setFormat(const Format& format)
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
            setFormat(curFormat);
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
            setFormat(curFormat);
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
    return d->hasHyperlink;
}

UString Cell::hyperlinkDisplayName() const
{
    return d->hyperlinkDisplayName;
}

UString Cell::hyperlinkLocation() const
{
    return d->hyperlinkLocation;
}

UString Cell::hyperlinkTargetFrameName() const
{
    return d->hyperlinkTargetFrameName;
}

void Cell::removeHyperlink()
{
    d->hyperlinkDisplayName = UString();
    d->hyperlinkLocation = UString();
    d->hyperlinkTargetFrameName = UString();
    d->hasHyperlink = false;
}

void Cell::setHyperlink(const UString& displayName, const UString& location, const UString& targetFrameName)
{
    d->hyperlinkDisplayName = displayName;
    d->hyperlinkLocation = location;
    d->hyperlinkTargetFrameName = targetFrameName;
    d->hasHyperlink = true;
}

UString Cell::note() const
{
    return d->note;
}

void Cell::setNote(const UString &n)
{
    d->note = n;
}

std::vector<Picture*> Cell::pictures() const
{
    return d->pictures;
}

void Cell::setPictures(std::vector<Picture*> pics)
{
    d->pictures = pics;
}

void Cell::addPicture(Picture* picture)
{
    d->pictures.push_back(picture);
}

std::vector<ChartObject*> Cell::charts() const
{
    return d->charts;
}

void Cell::addChart(ChartObject* chart)
{
    d->charts.push_back(chart);
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
    if (hasHyperlink() && ( hyperlinkDisplayName() != other.hyperlinkDisplayName() ||
                            hyperlinkLocation() != other.hyperlinkLocation() ||
                            hyperlinkTargetFrameName() != other.hyperlinkTargetFrameName() ) ) return false;

    if (note() != other.note()) return false;

    if (pictures().size() != other.pictures().size()) return false;
    for(int i = pictures().size() - 1; i >= 0; --i) {
        Picture* p1 = pictures()[i];
        Picture* p2 = other.pictures()[i];
        if(p1->m_id != p2->m_id) return false;
        if(p1->m_filename != p2->m_filename) return false;
        if(p1->m_colL != p2->m_colL) return false;
        if(p1->m_dxL != p2->m_dxL) return false;
        if(p1->m_rwT != p2->m_rwT) return false;
        if(p1->m_dyT != p2->m_dyT) return false;
        if(p1->m_colR != p2->m_colR) return false;
        if(p1->m_dxR != p2->m_dxR) return false;
        if(p1->m_rwB != p2->m_rwB) return false;
        if(p1->m_dyB != p2->m_dyB) return false;
    }

    if (charts().size() != other.charts().size()) return false;
    for(int i = charts().size() - 1; i >= 0; --i) {
        ChartObject* c1 = charts()[i];
        ChartObject* c2 = other.charts()[i];
        if(*c1 != *c2) return false;
    }

    return true;
}

bool Cell::operator!=(const Cell &other) const
{
    return ! (*this == other);
}
