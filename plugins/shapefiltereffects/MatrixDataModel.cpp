/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "MatrixDataModel.h"

MatrixDataModel::MatrixDataModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_rows(0)
    , m_cols(0)
{
}

void MatrixDataModel::setMatrix(const QVector<qreal> &matrix, int rows, int cols)
{
    m_matrix = matrix;
    m_rows = rows;
    m_cols = cols;
    Q_ASSERT(m_rows);
    Q_ASSERT(m_cols);
    Q_ASSERT(m_matrix.count() == m_rows * m_cols);
    beginResetModel();
    endResetModel();
}

QVector<qreal> MatrixDataModel::matrix() const
{
    return m_matrix;
}

int MatrixDataModel::rowCount(const QModelIndex & /*parent*/) const
{
    return m_rows;
}

int MatrixDataModel::columnCount(const QModelIndex & /*parent*/) const
{
    return m_cols;
}

QVariant MatrixDataModel::data(const QModelIndex &index, int role) const
{
    int element = index.row() * m_cols + index.column();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return QVariant(QString("%1").arg(m_matrix[element], 2));
        break;
    default:
        return QVariant();
    }
}

bool MatrixDataModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
    int element = index.row() * m_cols + index.column();
    bool valid = false;
    qreal elementValue = value.toDouble(&valid);
    if (!valid)
        return false;
    m_matrix[element] = elementValue;
    Q_EMIT dataChanged(index, index);
    return true;
}

Qt::ItemFlags MatrixDataModel::flags(const QModelIndex & /*index*/) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}
