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

#include "MctCell.h"

#include <QDebug>
#include <QTextTableCellFormat>

#include <math.h>
const QString MctCell::COLS = "ABCDEFGHIJKLMNOPQRTSUVWXYZabcdefghijklmnopqrstuvwxyz";

/**
 * @brief MctCell::MctCell copy-costructor
 * @param cell
 */
MctCell::MctCell(QString cellName, QString tableName, int row, int col)
    : MctPropertyBase(cellName)
    , cellName(cellName)
    , tableName(tableName)
    , row(row)
    , col(col)
{

}

MctCell::MctCell(int row, int col, QString tableName)
    : row(row)
    , col(col)
    , tableName(tableName)
{

}

MctCell::MctCell(const MctCell &cell)
    : MctPropertyBase(cell.cellName)
{
    cellName = cell.cellName;
    tableName = cell.tableName;
    row = cell.row;
    col = cell.col;

    // az os elvileg inicializalja es nem null
    if (props == nullptr)
        props = new PropertyDictionary;
    *props = *cell.props;

    if (props2export == nullptr)
        props2export = new PropertyDictionary;
    *props2export = *cell.props2export;

}

MctCell::MctCell()
{

}

MctCell::~MctCell()
{

}

/**
 * @brief MctCell::addCelPos Add two cell positions and create new cell object at the position
 * @param cell The second cell
 * @return The new cell
 */
MctCell *MctCell::addCelPos(MctCell *cell)
{
    if (this->tableName != cell->tableName)
        //qDebug << "The cells arent in the same table!";

    if (cell->row < 0 || cell->col < 0)
        cell->convertCellName2CellPos();

    if (this->row < 0 || this->col < 0)
        this->convertCellName2CellPos();

    if (cell->row < 0 || cell->col < 0 || this->row < 0 || this->col < 0)
        return NULL; // Failed converting

    int newCol = cell->col + this->col - 1;
    int newRow = cell->row + this->row - 1;

    return new MctCell(newRow, newCol, this->tableName);
}

/**
 * @brief MctCell::convertCellName2CellPos Convert the cell name string into interger positions
 * @note Pl: "D2" -> col = 3*60^0 = 4, row = 2
 */
void MctCell::convertCellName2CellPos()
{
/// FIXME: do we use this function in calligra?

//    return; /// ????

//    if (cellName == "") return;

//    int i = 0;
//    for (auto it = cellName.begin(); it != cellName.end(); ++it) {
//        if (!COLS.contains(*it))
//            break;
//        ++i;
//    }

//    QString colString = cellName.mid(0, i);
//    int row = cellName.mid(i).toInt();

//    int colNumMod = COLS.length();
//    int col = 0;
//    i = 0;

//    auto beforeEnd = colString.end();
//    beforeEnd--;
//    for (auto it = colString.begin(); it != beforeEnd; ++it) {
//        int delta = COLS.indexOf(*it) + 1;

//        if (delta < 0)
//            delta = 0;

//        col += delta * pow(colNumMod, i);
//        ++i;
//    }

//    this->row = row;
//    this->col = col;
}

/**
 * @brief MctCell::convertCellPos2CellName Generate string position identifier as cellname from current position
 * @note Pl: col = 4, row = 2 -> "D"+"2" = "D2"
 */
void MctCell::convertCellPos2CellName()
{
/// FIXME: this could be also legacy function
    if (this->row < 0 || this->col < 0)
        return;

    int colNumMod = COLS.length();

    int mod = this->col % colNumMod;

    this->cellName = COLS[mod] + QString::number(this->row + 1);
}

QString MctCell::getTableName() const
{
    return this->tableName;
}

void MctCell::setTableName(QString name)
{
    tableName = name;
}

void MctCell::setCellName(QString cell)
{
    cellName = cell;
}

QString MctCell::getCellName() const
{
    return cellName;
}

void MctCell::setRow(int r)
{
    row = r;
}

int MctCell::getRow() const
{
    return row;
}

int MctCell::getCol() const
{
    return col;
}

void MctCell::setCol(int c)
{
    col = c;
}

