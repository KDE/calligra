/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MATRIXDATAMODEL_H
#define MATRIXDATAMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class MatrixDataModel : public QAbstractTableModel
{
public:
    /// Creates a new matrix data model
    explicit MatrixDataModel(QObject *parent = nullptr);

    /// Sets the matrix data and rows/columns to use
    void setMatrix(const QVector<qreal> &matrix, int rows, int cols);

    /// Returns the matrix data
    QVector<qreal> matrix() const;

    // reimplemented
    int rowCount(const QModelIndex & /*parent*/) const override;
    // reimplemented
    int columnCount(const QModelIndex & /*parent*/) const override;
    // reimplemented
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // reimplemented
    bool setData(const QModelIndex &index, const QVariant &value, int /*role*/) override;
    // reimplemented
    Qt::ItemFlags flags(const QModelIndex & /*index*/) const override;

private:
    QVector<qreal> m_matrix; ///< the matrix data to handle
    int m_rows; ///< the number or rows in the matrix
    int m_cols; ///< the number of columns in the matrix
};

#endif // MATRIXDATAMODEL_H
