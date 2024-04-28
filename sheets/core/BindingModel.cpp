/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
    SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "BindingModel.h"

#include "Binding.h"
#include "Cell.h"
#include "CellStorage.h"
#include "Sheet.h"
#include "Style.h"
#include "engine/MapBase.h"
#include "engine/Value.h"

using namespace Calligra::Sheets;

QHash<QString, QVector<QRect>> BindingModel::cellRegion() const
{
    QHash<QString, QVector<QRect>> answer;
    Region::ConstIterator end = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != end; ++it) {
        if (!(*it)->isValid()) {
            continue;
        }
        answer[(*it)->name()].append((*it)->rect());
    }
    return answer;
}

bool BindingModel::setCellRegion(const QString &regionName)
{
    Q_ASSERT(m_region.isValid());
    Q_ASSERT(m_region.firstSheet());
    const MapBase *const map = m_region.firstSheet()->map();
    const Region region = map->regionFromName(regionName, nullptr);
    if (!region.isValid()) {
        debugSheets << qPrintable(regionName) << "is not a valid region.";
        return false;
    }
    // Clear the old binding.
    Region::ConstIterator end = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != end; ++it) {
        if (!(*it)->isValid()) {
            continue;
        }
        SheetBase *sheet = (*it)->sheet();
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        // FIXME Stefan: This may also clear other bindings!
        fullSheet->fullCellStorage()->setBinding(Region((*it)->rect(), (*it)->sheet()), Binding());
    }
    // Set the new region
    m_region = region;
    end = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != end; ++it) {
        if (!(*it)->isValid()) {
            continue;
        }
        SheetBase *sheet = (*it)->sheet();
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        fullSheet->fullCellStorage()->setBinding(Region((*it)->rect(), (*it)->sheet()), *m_binding);
    }
    return true;
}

/////// BindingModel

BindingModel::BindingModel(Binding *binding, QObject *parent)
    : QAbstractTableModel(parent)
    , m_binding(binding)
{
}

bool BindingModel::isCellRegionValid(const QString &regionName) const
{
    Q_CHECK_PTR(m_region.firstSheet());
    Q_CHECK_PTR(m_region.firstSheet()->map());
    return m_region.firstSheet()->map()->regionFromName(regionName).isValid();
}

void BindingModel::emitChanged(const Region &region)
{
    emit changed(region);
}

void BindingModel::emitDataChanged(const QRect &rect)
{
    const QPoint tl = rect.topLeft();
    const QPoint br = rect.bottomRight();
    // debugSheetsUI << "emit QAbstractItemModel::dataChanged" << QString("%1:%2").arg(tl).arg(br);
    emit dataChanged(index(tl.y(), tl.x()), index(br.y(), br.x()));
}

QVariant BindingModel::data(const QModelIndex &index, int role) const
{
    if ((m_region.isEmpty()) || (role != Qt::EditRole && role != Qt::DisplayRole))
        return QVariant();
    const QPoint offset = m_region.firstRange().topLeft();
    SheetBase *sheet = m_region.firstSheet();
    int row = offset.y() + index.row();
    int column = offset.x() + index.column();
    Value value = sheet->cellStorage()->value(column, row);

    switch (role) {
    case Qt::DisplayRole: {
        // return the in the cell displayed test
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        Cell c(fullSheet, column, row);
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
            if (value.format() == Value::fmt_DateTime || value.format() == Value::fmt_Date || value.format() == Value::fmt_Time) {
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
    // debugSheets << index.column() <<"," << index.row() <<"," << variant;
    return QVariant();
}

const Calligra::Sheets::Region &BindingModel::region() const
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
    const SheetBase *sheet = m_region.firstSheet();
    const Value value = sheet->cellStorage()->value(col, row);
    return value.asVariant();
}

int BindingModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_region.isEmpty() ? 0 : m_region.firstRange().height();
}

int BindingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_region.isEmpty() ? 0 : m_region.firstRange().width();
}

void BindingModel::setRegion(const Region &region)
{
    m_region = region;
}
