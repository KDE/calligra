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

/**
 * Position of a change
 *
 * A change position in MCT usually consists of four component.
 * The starting and ending paragraphs are counted and so the character positions.
 * Sometimes ending information is not give for example when Paragraph change is in action.
 */
class MctPosition
{
public:
    /// constructor
    MctPosition(ulong startPar = 0, ulong starChar = 0, ulong endPar = 0, ulong endChar = 0, MctCell *startCellInf = NULL, MctCell *endCellInf = NULL);
    MctPosition(const MctPosition &position);
    ~MctPosition();

    /// getter
    ulong startPar() const;
    /// setter
    void setStartPar(ulong value);
    /// getter
    ulong endPar() const;
    /// setter
    void setEndPar(ulong value);
    /// getter
    ulong startChar() const;
    /// setter
    void setStartChar(ulong value);
    /// getter
    ulong endChar() const;
    /// setter
    void setEndChar(ulong value);
    /// getter
    MctCell* startCellInfo() const;
    /// setter
    void setStartCellInfo(MctCell* info);
    /// getter
    MctCell* endCellInfoEnd() const;
    /// setter
    void setEndCellInfo(MctCell* info);
    /// getter
    MctPosition * anchoredPos() const;
    /// setter
    void setAnchored(MctPosition *parentPos);

    /**
     * correct table cell position calculation
     *
     * Calligra adds a hidden cursor position before tables,
     * this function correct the position regarding to the hidden point.
     * @param cursor which is point inside the table
     * @return absolute cursor position
     */
    int tableCellPosition(QTextCursor *cursor) const;

    /// simple debug function
    QString toString() const;

private:
    ulong m_startPar;   ///< start number of the paragraph where the change starts.
    ulong m_startChar;  ///< start number of the character in the paragraph where the change starts.
    ulong m_endPar;     ///< end number of the paragraph where the change starts.
    ulong m_endChar;    ///< end number of the character in the paragraph where the change starts.

    MctCell *m_startCellInf;    ///< cell which contains starting cell name and table name
    MctCell *m_endCellInf;      ///< cell which contains ending cell name and table name

    MctPosition *m_anchoredPos; ///< position of the object where this position is anchored

};

#endif // MCTPOSITION_H

