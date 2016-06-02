/* This file is part of the KDE project
 * Copyright (C) 2015-2016 MultiRacio Ltd. <multiracio@multiracio.com> (S.Schliszka, F.Novak, P.Rakyta)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MCTCELL_H
#define MCTCELL_H

#include "MctPropertyBase.h"

class MctCell : public MctPropertyBase
{
public:
    MctCell(const QString &m_cellName, const QString &m_tableName, int m_row = -1, int m_col = -1);
    MctCell(int m_row, int m_col, const QString &m_tableName = "noname");
    MctCell(const MctCell &cell);
    MctCell();
    ~MctCell();

    MctCell * addCelPos(MctCell *cell); // Copy the given cells position into a new one, and return with it
    void convertCellPos2CellName();
    void convertCellName2CellPos();

    QString cellName() const;
    void setCellName(QString cell);

    QString tableName() const;
    void setTableName(const QString &name);

    int row() const;
    void setRow(int r);

    int col() const;
    void setCol(int c);

    const static QString COLS;
    
protected:
    QString m_cellName;       // The name of the cell
    QString m_tableName;      // The name of the anchor table

private:
    int m_col;                // The column number of the cell
    int m_row;                // The row number of the cell
};

#endif // MCTCELL_H
