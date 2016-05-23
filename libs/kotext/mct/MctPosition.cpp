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

#include "MctPosition.h"
#include "MctCell.h"

MctPosition::MctPosition(ulong startPar , ulong starChar , ulong endPar , ulong endChar , MctCell *startCellInf , MctCell *endCellInf )
    : m_startPar(startPar)
    , m_startChar(starChar)
    , m_endPar(endPar)
    , m_endChar(endChar)
    , m_startCellInf(startCellInf)
    , m_endCellInf(endCellInf)
    , m_anchoredPos(NULL)
{

}

MctPosition::MctPosition(const MctPosition &position)
    : m_startPar(position.m_startPar)
    , m_startChar(position.m_startChar)
    , m_endPar(position.m_endPar)
    , m_endChar(position.m_endChar)
    , m_startCellInf(position.m_startCellInf)
    , m_endCellInf(position.m_endCellInf)
    , m_anchoredPos(NULL)
{
    if (position.m_anchoredPos)
        this->m_anchoredPos = new MctPosition(*(position.m_anchoredPos)); // duplicate
}

MctPosition::~MctPosition()
{
    delete m_startCellInf;
    delete m_endCellInf;
    delete m_anchoredPos;
}

ulong MctPosition::startPar() const
{
    return m_startPar;
}

void MctPosition::setStartPar(ulong value)
{
    m_startPar = value;
}

ulong MctPosition::endPar() const
{
    return this->m_endPar;
}

void MctPosition::setEndPar(ulong value)
{
    m_endPar = value;
}

ulong MctPosition::startChar() const
{
    return m_startChar;
}

void MctPosition::setStartChar(ulong value)
{
    m_startChar = value;
}

ulong MctPosition::endChar() const
{
    return m_endChar;
}

void MctPosition::setEndChar(ulong value)
{
    m_endChar = value;
}

MctCell* MctPosition::startCellInfo() const
{
    return m_startCellInf;
}

void MctPosition::setStartCellInfo(MctCell* info)
{
    this->m_startCellInf = info;
}

void MctPosition::setEndCellInfo(MctCell *info)
{
    this->m_endCellInf = info;
}

MctCell * MctPosition::endCellInfoEnd() const
{
    return m_endCellInf;
}

MctPosition* MctPosition::anchoredPos() const
{
    return m_anchoredPos;
}

void MctPosition::setAnchored(MctPosition *parentPos)
{
    this->m_anchoredPos = parentPos;
}

int MctPosition::tableCellPosition(QTextCursor *cursor) const
{
    int blockpos  = cursor->block().position();
    MctPosition *tmp = new MctPosition(*this);
    QTextCursor tmpcursor(*cursor);
    QTextTable * table;
    while(tmp->anchoredPos()) {
        blockpos  = tmpcursor.block().position();
        bool hiddenTableHandling = tmpcursor.blockFormat().hasProperty(KoParagraphStyle::HiddenByTable);
        table = tmpcursor.currentTable();
        if(hiddenTableHandling) {
            tmpcursor.movePosition(QTextCursor::NextCharacter);
            table = tmpcursor.currentTable();
        }
        QTextTableCell cell = table->cellAt(tmp->startCellInfo()->row(), tmp->startCellInfo()->col());
        tmpcursor = cell.firstCursorPosition();
        tmp = tmp->anchoredPos();
        // if a nested table comes there will be +1 block to jump through
        int k = tmp->anchoredPos() ? 1 : 0;
        tmpcursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, k + tmp->startPar());
        tmpcursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, tmp->startChar());
    }
    return tmpcursor.position();
}

QString MctPosition::toString() const
{
    QString answer = "POSITION INFO:\nstart p: " + QString::number(this->m_startPar) + "\tchar: " + QString::number(this->m_startChar) + "\nend p: " + QString::number(this->m_endPar) + "\tchar:" + QString::number(this->m_endChar) + "\n";
    return answer;
}

