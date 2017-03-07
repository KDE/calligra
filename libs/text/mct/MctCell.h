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

/**
 * Representation of a table cell
 */
class MctCell : public MctPropertyBase
{
public:
    MctCell(const QString &m_cellName, const QString &m_tableName, int m_row = -1, int m_col = -1);
    MctCell(int m_row, int m_col, const QString &m_tableName = "noname");
    MctCell(const MctCell &cell);
    MctCell();
    ~MctCell();

    /**
     * merge cell with another
     *
     * Merge two cell positions and create new cell object at the position
     * @param cell to merge
     * @return new cell with extended position
     */
    MctCell * addCelPos(MctCell *cell);

    /**
     * convert the cell name string into interger positions
     *
     * @note example: "D2" -> col = 3*60^0 = 4, row = 2
     * @todo remove this function
     */
    void convertCellPos2CellName();


    /**
     * generate string position identifier as cellname from current position
     *
     * @note Pl: col = 4, row = 2 -> "D"+"2" = "D2"
     * @todo remove this function if not used
     */
    void convertCellName2CellPos();

    /// getter
    QString cellName() const;
    /// setter
    void setCellName(QString cell);

    /// getter
    QString tableName() const;
    /// setter
    void setTableName(const QString &name);

    /// getter
    int row() const;
    /// setter
    void setRow(int r);

    /// getter
    int col() const;
    /// setter
    void setCol(int c);

    const static QString COLS;  ///< cell indentifier string eg: A22 @note probably legacy code
    
protected:
    QString m_cellName; ///< name of the cell
    QString m_tableName;///< name of the anchor table

private:
    int m_col;  ///< column number of the cell
    int m_row;  ///< row number of the cell
};

#endif // MCTCELL_H
