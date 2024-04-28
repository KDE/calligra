/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "PartsListModel.h"
#include "../core/Part.h"
#include "../core/Sheet.h"

using namespace MusicCore;

PartsListModel::PartsListModel(Sheet *sheet)
    : m_sheet(sheet)
{
    connect(m_sheet, &Sheet::partAdded, this, &PartsListModel::partAdded);
    connect(m_sheet, &Sheet::partRemoved, this, &PartsListModel::partRemoved);
}

int PartsListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_sheet->partCount();
    } else {
        return 0;
    }
}

QVariant PartsListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        int row = index.row();
        if (row < 0 || row >= m_sheet->partCount())
            return QString("invalid");
        return m_sheet->part(row)->name();
    }
    return QVariant();
}

void PartsListModel::partAdded(int index, Part *part)
{
    Q_UNUSED(part);

    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void PartsListModel::partRemoved(int index, Part *part)
{
    Q_UNUSED(part);

    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}
