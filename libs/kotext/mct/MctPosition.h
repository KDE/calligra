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

#ifndef MCTPOSITION_H
#define MCTPOSITION_H

#include <QtGlobal>
#include <QTextCursor>
#include <QTextTableCell>
#include <KoParagraphStyle.h>

class MctCell;

class MctPosition
{
public:
    MctPosition(ulong startPar = 0, ulong starChar = 0, ulong endPar = 0, ulong endChar = 0, MctCell *startCellInf = NULL, MctCell *endCellInf = NULL);
    MctPosition(const MctPosition &position);
    ~MctPosition();

    ulong getStartPar() const;
    void setStartPar(ulong value);

    ulong getEndPar() const;
    void setEndPar(ulong value);

    ulong getStartChar() const;
    void setStartChar(ulong value);

    ulong getEndChar() const;
    void setEndChar(ulong value);

    MctCell* getCellInfo() const;
    void setCellInfo(MctCell* info);

    MctCell* getCellInfoEnd() const;
    void setCellInfoEnd(MctCell* info);

    MctPosition * getAnchoredPos() const;
    void setAnchored(MctPosition *parentPos);

    int getTableCellPosition(QTextCursor *cursor) const;

    QString toString() const;

private:
    ulong startPar;     // The start number of the paragraph where the change starts.
    ulong startChar;    // The start number of the character in the paragraph where the change starts.
    ulong endPar;       // The end number of the paragraph where the change starts.
    ulong endChar;      // The end number of the character in the paragraph where the change starts.

    MctCell *startCellInf;   // Cell class conatining starting cell name and table name
    MctCell *endCellInf;     // Cell class conatining ending cell name and table name

    MctPosition *anchoredPos;// Position class of the object where this position is anchored

};

#endif // MCTPOSITION_H

