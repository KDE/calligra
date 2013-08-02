/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
    Copyright (C) 2008 Thomas Zander <zander@kde.org>

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

#include "BindingModel.h"
#include "Binding.h"

#include <QRect>

#include <kdebug.h>

#include "CellStorage.h"
#include "Map.h"
#include "Sheet.h"
#include "Value.h"

using namespace Calligra::Sheets;

BindingModel::BindingModel(Binding* binding, QObject *parent)
        : QAbstractTableModel(parent)
        , m_binding(binding)
{
}

bool BindingModel::isCellRegionValid(const QString& regionName) const
{
    Q_CHECK_PTR(m_region.firstSheet());
    Q_CHECK_PTR(m_region.firstSheet()->map());
    return Region(regionName, m_region.firstSheet()->map()).isValid();
}

void BindingModel::emitChanged(const Region& region)
{
    emit changed(region);
}

void BindingModel::emitDataChanged(const QRect& rect)
{
    const QPoint tl = rect.topLeft();
    const QPoint br = rect.bottomRight();
    //kDebug(36005) << "emit QAbstractItemModel::dataChanged" << QString("%1:%2").arg(tl).arg(br);
    emit dataChanged(index(tl.y(), tl.x()), index(br.y(), br.x()));
}

QVariant BindingModel::data(const QModelIndex& index, int role) const
{
    if ((m_region.isEmpty()) || (role != Qt::EditRole && role != Qt::DisplayRole))
        return QVariant();
    const QPoint offset = m_region.firstRange().topLeft();
    const Sheet* sheet = m_region.firstSheet();
    int row = offset.y() + index.row();
    int column = offset.x() + index.column();
    Value value = sheet->cellStorage()->value(column, row);

    switch (role) {
        case Qt::DisplayRole: {
            // return the in the cell displayed test
            Cell c(sheet, column, row);
            bool showFormula = false;
            return c.displayText(Style(), &value, &showFormula);
        }
        case Qt::EditRole: {
            // return the actual cell value
            // KoChart::Value is either:
            //  - a double (interpreted as a value)
            //  - a QString (interpreted as a label)
            //  - a QDateTime (interpreted as a date/time value)
            //  - Invalid (interpreted as empty)
            QVariant variant;
            switch (value.type()) {
                case Value::Float:
                case Value::Integer:
                    if (value.format() == Value::fmt_DateTime ||
                            value.format() == Value::fmt_Date ||
                            value.format() == Value::fmt_Time) {
                        variant.setValue<QDateTime>(value.asDateTime(sheet->map()->calculationSettings()));
                        break;
                    } // fall through
                case Value::Boolean:
                case Value::Complex:
                case Value::Array:
                    variant.setValue<double>(numToDouble(value.asFloat()));
                    break;
                case Value::String:
                case Value::Error:
                    variant.setValue<QString>(value.asString());
                    break;
                case Value::Empty:
                case Value::CellRange:
                default:
                    break;
            }
            return variant;
        }
    }
    //kDebug() << index.column() <<"," << index.row() <<"," << variant;
    return QVariant();
}

const Calligra::Sheets::Region& BindingModel::region() const
{
    return m_region;
}

QVariant BindingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((m_region.isEmpty()) || (role != Qt::EditRole && role != Qt::DisplayRole))
        return QVariant();
    const QPoint offset = m_region.firstRange().topLeft();
    const int col = (orientation == Qt::Vertical) ? offset.x() : offset.x() + section;
    const int row = (orientation == Qt::Vertical) ? offset.y() + section : offset.y();
    const Sheet* sheet = m_region.firstSheet();
    const Value value = sheet->cellStorage()->value(col, row);
    return value.asVariant();
}

int BindingModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_region.isEmpty() ? 0 : m_region.firstRange().height();
}

int BindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_region.isEmpty() ? 0 : m_region.firstRange().width();
}

void BindingModel::setRegion(const Region& region)
{
    m_region = region;
}
