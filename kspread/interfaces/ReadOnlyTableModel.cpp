/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus stefan.nikolaus@kdemail.net

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
   Boston, MA 02110-1301, USA.
*/

#include "ReadOnlyTableModel.h"

// KSpread
#include "Cell.h"
#include "Limits.h"
#include "Map.h"
#include "Sheet.h"
#include "Style.h"
#include "Value.h"
#include "ValueFormatter.h"

// Qt
#include <QSize>

using namespace KSpread;

class ReadOnlyTableModel::Private
{
public:
    Sheet* sheet;
    QSize size;
};

ReadOnlyTableModel::ReadOnlyTableModel(Sheet* sheet, int columns, int rows)
        : QAbstractTableModel(sheet)
        , d(new Private)
{
    d->sheet = sheet;
    d->size = (columns && rows) ? QSize(columns, rows) : QSize(KS_colMax, KS_rowMax);
}

ReadOnlyTableModel::~ReadOnlyTableModel()
{
    delete d;
}

int ReadOnlyTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return (d->sheet->usedArea() & QRect(QPoint(1, 1), d->size)).width();
}

int ReadOnlyTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return (d->sheet->usedArea() & QRect(QPoint(1, 1), d->size)).height();
}

QVariant ReadOnlyTableModel::data(const QModelIndex& index, int role) const
{
    // NOTE Model indices start from 0, while KSpread column/row indices start from 1.
    const Cell cell = Cell(d->sheet, index.column() + 1, index.row() + 1).masterCell();
    const Style style = cell.effectiveStyle();
    if (role == Qt::DisplayRole) {
        // Display a formula if warranted.  If not, simply display the value.
        if (cell.isFormula() && d->sheet->getShowFormula() &&
                !(d->sheet->isProtected() && style.hideFormula())) {
            return QVariant(cell.userInput());
        } else if (d->sheet->getHideZero() && cell.value().isNumber() && cell.value().asFloat() == 0.0) {
            // Hide zero.
            return QVariant();
        } else if (!cell.isEmpty()) {
            // Format the value appropriately and set the display text.
            // The format of the resulting value is used below to determine the alignment.
            Value value = d->sheet->map()->formatter()->formatText(cell.value(), style.formatType(),
                          style.precision(), style.floatFormat(),
                          style.prefix(), style.postfix(),
                          style.currency().symbol());
            return value.asString();
        }
    } else if (role == Qt::EditRole) {
        return cell.userInput();
    } else if (role == Qt::ToolTipRole) {
        return cell.comment();
    }
    return QVariant();
}

QVariant ReadOnlyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // NOTE Model indices start from 0, while KSpread column/row indices start from 1.
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return Cell::columnName(section + 1);
        } else {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}

Sheet* ReadOnlyTableModel::sheet() const
{
    return d->sheet;
}

const QSize& ReadOnlyTableModel::size() const
{
    return d->size;
}
