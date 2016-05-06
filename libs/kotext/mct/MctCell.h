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
    MctCell(QString cellName, QString tableName, int row = -1, int col = -1);
    MctCell(int row, int col, QString tableName = "noname");
    MctCell(const MctCell &cell);
    MctCell();
    ~MctCell();

    MctCell * addCelPos(MctCell *cell); // Copy the given cells position into a new one, and return with it
    void convertCellPos2CellName();
    void convertCellName2CellPos();

    QString getCellName() const;
    void setCellName(QString cell);

    QString getTableName() const;
    void setTableName(QString name);

    int getRow() const;
    void setRow(int r);

    int getCol() const;
    void setCol(int c);

    const static QString COLS;
    
protected:
    QString cellName;       // The name of the cell
    QString tableName;      // The name of the anchor table

private:
    int col;                // The column number of the cell
    int row;                // The row number of the cell
};

#endif // MCTCELL_H
