/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ReadWriteTableModel.h"

// Sheets
#include "Cell.h"
#include "Sheet.h"

// Qt
#include <QSize>

using namespace KSpread;

class ReadWriteTableModel::Private
{
public:
};

ReadWriteTableModel::ReadWriteTableModel(Sheet* sheet, int columns, int rows)
        : ReadOnlyTableModel(sheet, columns, rows)
        , d(new Private)
{
}

ReadWriteTableModel::~ReadWriteTableModel()
{
    delete d;
}

Qt::ItemFlags ReadWriteTableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid() || !hasIndex(index.row(), index.column(), index.parent())) {
        return 0;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool ReadWriteTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    // NOTE Model indices start from 0, while Calligra Sheets column/row indices start from 1.
    Cell cell = Cell(sheet(), index.column() + 1, index.row() + 1).masterCell();
    if (role == Qt::EditRole) {
        cell.parseUserInput(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}
