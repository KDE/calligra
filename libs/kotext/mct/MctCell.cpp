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
    , m_cellName(cellName)
    , m_tableName(tableName)
    , m_row(row)
    , m_col(col)
{

}

MctCell::MctCell(int row, int col, QString tableName)
    : m_row(row)
    , m_col(col)
    , m_tableName(tableName)
{

}

MctCell::MctCell(const MctCell &cell)
    : MctPropertyBase(cell.cellName())
{
    m_cellName = cell.cellName();
    m_tableName = cell.tableName();
    m_row = cell.row();
    m_col = cell.col();

    // az os elvileg inicializalja es nem null
    if (m_props == nullptr)
        m_props = new PropertyDictionary;
    *m_props = *cell.m_props;

    if (m_props2export == nullptr)
        m_props2export = new PropertyDictionary;
    *m_props2export = *cell.m_props2export;

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
    if (m_tableName != cell->tableName())
        //qDebug << "The cells arent in the same table!";

    if (cell->row() < 0 || cell->col() < 0)
        cell->convertCellName2CellPos();

    if (m_row < 0 || m_col < 0)
        this->convertCellName2CellPos();

    if (cell->row() < 0 || cell->col() < 0 || m_row < 0 || m_col < 0)
        return NULL; // Failed converting

    int newCol = cell->col() + m_col - 1;
    int newRow = cell->row() + m_row - 1;

    return new MctCell(newRow, newCol, m_tableName);
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
    if (m_row < 0 || m_col < 0)
        return;

    int colNumMod = COLS.length();

    int mod = m_col % colNumMod;

    m_cellName = COLS[mod] + QString::number(m_row + 1);
}

QString MctCell::tableName() const
{
    return m_tableName;
}

void MctCell::setTableName(QString name)
{
    m_tableName = name;
}

void MctCell::setCellName(QString cell)
{
    m_cellName = cell;
}

QString MctCell::cellName() const
{
    return m_cellName;
}

void MctCell::setRow(int r)
{
    m_row = r;
}

int MctCell::row() const
{
    return m_row;
}

int MctCell::col() const
{
    return m_col;
}

void MctCell::setCol(int c)
{
    m_col = c;
}

