/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SheetModel.h"

// Sheets
#include "Binding.h"
#include "Cell.h"
#include "CellStorage.h"
#include "Condition.h"
#include "Database.h"
#include "Map.h"
#include "ModelSupport.h"
#include "Sheet.h"
#include "ValueFormatter.h"
#include "engine/Formula.h"

// Qt
#include <QItemSelectionRange>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SheetModel::Private
{
public:
    Sheet *sheet;
};

SheetModel::SheetModel(Sheet *sheet)
    : QAbstractTableModel(sheet)
    , d(new Private)
{
    d->sheet = sheet;
}

SheetModel::~SheetModel()
{
    delete d;
}

int SheetModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.internalPointer() != d->sheet->map()) {
        return 0;
    }
    return KS_colMax;
}

QVariant SheetModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.model() != this) {
        return QVariant();
    }
    if (index.internalPointer() != d->sheet) {
        return QVariant();
    }
    if (index.parent().isValid()) {
        if (index.parent().internalPointer() != d->sheet->map()) {
            return QVariant();
        }
    }
    // NOTE Model indices start from 0, while Calligra Sheets column/row indices start from 1.
    const Cell cell = Cell(d->sheet, index.column() + 1, index.row() + 1).masterCell();
    const Style style = cell.effectiveStyle();
    if (role == Qt::DisplayRole) {
        // Display a formula if warranted.  If not, simply display the value.
        if (cell.isFormula() && d->sheet->getShowFormula() && !(d->sheet->isProtected() && style.hideFormula())) {
            return QVariant(cell.userInput());
        } else if (d->sheet->getHideZero() && cell.value().isNumber() && cell.value().asFloat() == 0.0) {
            // Hide zero.
            return QVariant();
        } else if (!cell.isEmpty()) {
            // Format the value appropriately and set the display text.
            // The format of the resulting value is used below to determine the alignment.
            Value value = d->sheet->fullMap()->formatter()->formatText(cell.value(),
                                                                       style.formatType(),
                                                                       style.precision(),
                                                                       style.floatFormat(),
                                                                       style.prefix(),
                                                                       style.postfix(),
                                                                       style.currency().symbol());
            return value.asString();
        }
    } else if (role == Qt::EditRole) {
        return cell.userInput();
    } else if (role == Qt::ToolTipRole) {
        return cell.comment();
    } else if (role == Qt::SizeHintRole) {
        // TODO
    } else if (role == Qt::FontRole) {
        return style.font();
    } else if (role == Qt::TextAlignmentRole) {
        // TODO
    } else if (role == Qt::BackgroundRole) {
        return style.backgroundBrush();
    } else if (role == Qt::BackgroundRole) {
        return style.backgroundColor();
    } else if (role == Qt::ForegroundRole) {
        return style.fontColor();
    }
    const int column = index.column() + 1;
    const int row = index.row() + 1;
    CellStorage *const storage = d->sheet->fullCellStorage();
    switch (role) {
    case UserInputRole:
        return storage->userInput(column, row);
    case FormulaRole: {
        QVariant data;
        data.setValue(storage->formula(column, row));
        return data;
    }
    case ValueRole: {
        QVariant data;
        data.setValue(storage->value(column, row));
        return data;
    }
    case LinkRole:
        return storage->link(column, row);
    }
    return QVariant();
}

Qt::ItemFlags SheetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    if (index.model() != this) {
        return Qt::NoItemFlags;
    }
    if (index.internalPointer() != d->sheet) {
        return Qt::NoItemFlags;
    }
    if (index.parent().isValid()) {
        if (index.parent().internalPointer() != d->sheet->map()) {
            return Qt::NoItemFlags;
        }
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant SheetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // NOTE Model indices start from 0, while Calligra Sheets column/row indices start from 1.
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return Cell::columnName(section + 1);
        } else {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}

QModelIndex SheetModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.internalPointer() != d->sheet->map()) {
        return QModelIndex();
    }
    // A cell in our sheet?
    if (!parent.isValid()) {
        return createIndex(row, column, d->sheet);
        // Embedded in a MapModel?
    } else if (parent.internalPointer() == d->sheet->map()) {
        return createIndex(row, column, d->sheet);
        // A sub-table?
    } else if (parent.internalPointer() == this) {
        // TODO sub-tables
    }
    return QModelIndex();
}

int SheetModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.internalPointer() != d->sheet->map()) {
        return 0;
    }
    return KS_rowMax;
}

bool SheetModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    if (index.model() != this) {
        return false;
    }
    if (index.internalPointer() != d->sheet) {
        return false;
    }
    if (index.parent().isValid()) {
        if (index.parent().internalPointer() != d->sheet->map()) {
            return false;
        }
    }
    // NOTE Model indices start from 0, while Calligra Sheets column/row indices start from 1.
    const int column = index.column() + 1;
    const int row = index.row() + 1;
    Cell cell = Cell(sheet(), index.column() + 1, index.row() + 1).masterCell();
    CellStorage *const storage = d->sheet->fullCellStorage();
    switch (role) {
    case Qt::EditRole:
        cell.parseUserInput(value.toString());
        break;
    case UserInputRole:
        storage->setUserInput(column, row, value.toString());
        break;
    case FormulaRole:
        storage->setFormula(column, row, value.value<Formula>());
        break;
    case ValueRole:
        storage->setValue(column, row, value.value<Value>());
        break;
    case LinkRole:
        storage->setLink(column, row, value.toString());
        break;
    case CommentRole:
        storage->setComment(column, row, value.toString());
        break;
    default:
        return false;
    }
    Q_EMIT dataChanged(index, index);
    return true;
}

bool SheetModel::setData(const QItemSelectionRange &range, const QVariant &value, int role)
{
    const QRect _range = toRange(range);
    const Region region(_range, d->sheet);
    CellStorage *const storage = d->sheet->fullCellStorage();
    switch (role) {
    case ConditionRole:
        storage->setConditions(region, value.value<Conditions>());
        break;
    case FusionedRangeRole:
        storage->mergeCells(_range.left(), _range.top(), _range.width() - 1, _range.height() - 1, !value.value<bool>());
        break;
    case LockedRangeRole:
        if (value.value<bool>())
            storage->lockCells(_range);
        else
            storage->unlockCells(_range);
        break;
    case NamedAreaRole: {
        QString namedAreaName = value.toString();
        if (namedAreaName.isEmpty())
            return false;
        storage->emitInsertNamedArea(region, namedAreaName);
        break;
    }
    case SourceRangeRole:
        storage->setBinding(region, value.value<Binding>());
        break;
    case StyleRole:
        // TODO
        //         storage->setStyle(region, value.value<Style>());
        break;
    case TargetRangeRole:
        storage->setDatabase(region, value.value<Database>());
        break;
    case ValidityRole:
        storage->setValidity(region, value.value<Validity>());
        break;
    default:
        return false;
    }
    Q_EMIT dataChanged(range.topLeft(), range.bottomRight());
    return true;
}

bool SheetModel::setData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &value, int role)
{
    return setData(QItemSelectionRange(topLeft, bottomRight), value, role);
}

Sheet *SheetModel::sheet() const
{
    return d->sheet;
}
