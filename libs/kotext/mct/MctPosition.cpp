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
    : startPar(startPar)
    , startChar(starChar)
    , endPar(endPar)
    , endChar(endChar)
    , startCellInf(startCellInf)
    , endCellInf(endCellInf)
    , anchoredPos(NULL)
{

}

MctPosition::MctPosition(const MctPosition &position)
    : startPar(position.startPar)
    , startChar(position.startChar)
    , endPar(position.endPar)
    , endChar(position.endChar)
    , startCellInf(position.startCellInf)
    , endCellInf(position.endCellInf)
    , anchoredPos(NULL)
{
    if (position.anchoredPos)
        this->anchoredPos = new MctPosition(*(position.anchoredPos)); // duplicate
}

MctPosition::~MctPosition()
{
    delete startCellInf;
    delete endCellInf;
    delete anchoredPos;
}

ulong MctPosition::getStartPar() const
{
    return startPar;
}

void MctPosition::setStartPar(ulong value)
{
    startPar = value;
}

ulong MctPosition::getEndPar() const
{
    return this->endPar;
}

void MctPosition::setEndPar(ulong value)
{
    endPar = value;
}

ulong MctPosition::getStartChar() const
{
    return startChar;
}

void MctPosition::setStartChar(ulong value)
{
    startChar = value;
}

ulong MctPosition::getEndChar() const
{
    return endChar;
}

void MctPosition::setEndChar(ulong value)
{
    endChar = value;
}

MctCell* MctPosition::getCellInfo() const
{
    return startCellInf;
}

void MctPosition::setCellInfo(MctCell* info)
{
    this->startCellInf = info;
}

void MctPosition::setCellInfoEnd(MctCell *info)
{
    this->endCellInf = info;
}

MctCell * MctPosition::getCellInfoEnd() const
{
    return endCellInf;
}

MctPosition* MctPosition::getAnchoredPos() const
{
    return anchoredPos;
}

void MctPosition::setAnchored(MctPosition *parentPos)
{
    this->anchoredPos = parentPos;
}

int MctPosition::getTableCellPosition(QTextCursor *cursor) const
{
    int blockpos  = cursor->block().position();
    MctPosition *tmp = new MctPosition(*this);
    QTextCursor tmpcursor(*cursor);
    QTextTable * table;
    while(tmp->getAnchoredPos()) {
        blockpos  = tmpcursor.block().position();
        bool hiddenTableHandling = tmpcursor.blockFormat().hasProperty(KoParagraphStyle::HiddenByTable);
        table = tmpcursor.currentTable();
        if(hiddenTableHandling) {
            tmpcursor.movePosition(QTextCursor::NextCharacter);
            table = tmpcursor.currentTable();
        }
        QTextTableCell cell = table->cellAt(tmp->getCellInfo()->row(), tmp->getCellInfo()->col());
        tmpcursor = cell.firstCursorPosition();
        tmp = tmp->getAnchoredPos();
        // if a nested table comes there will be +1 block to jump through
        int k = tmp->getAnchoredPos() ? 1 : 0;
        tmpcursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, k + tmp->getStartPar());
        tmpcursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, tmp->getStartChar());
    }
    return tmpcursor.position();
}

QString MctPosition::toString() const
{
    QString answer = "POSITION INFO:\nstart p: " + QString::number(this->startPar) + "\tchar: " + QString::number(this->startChar) + "\nend p: " + QString::number(this->endPar) + "\tchar:" + QString::number(this->endChar) + "\n";
    return answer;
}

