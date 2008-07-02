/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_ODF_SAVING_CONTEXT
#define KSPREAD_ODF_SAVING_CONTEXT

#include "Cell.h"
#include "GenValidationStyle.h"
#include "Sheet.h"

#include <KoShapeSavingContext.h>

#include <QMap>
#include <QMultiHash>

class KoShape;

namespace KSpread
{

class OdfSavingContext
{
public:
    OdfSavingContext(KoShapeSavingContext& shapeContext)
        : shapeContext(shapeContext) {}

    void insertCellAnchoredShape(const Cell& cell, KoShape* shape)
    {
        m_cellAnchoredShapes[cell.sheet()][cell.row()].insert(cell.column(), shape);
    }

    bool rowHasCellAnchoredShapes(const Sheet* sheet, int row) const
    {
        if (!m_cellAnchoredShapes.contains(sheet)) {
            return false;
        }
        return m_cellAnchoredShapes[sheet].contains(row);
    }

    bool cellHasAnchoredShapes(const Cell& cell) const
    {
        if (!m_cellAnchoredShapes.contains(cell.sheet())) {
            return false;
        } else if (!m_cellAnchoredShapes[cell.sheet()].contains(cell.row())) {
            return false;
        }
        return m_cellAnchoredShapes[cell.sheet()][cell.row()].contains(cell.column());
    }

    QList<KoShape*> cellAnchoredShapes(const Cell& cell) const
    {
        if (!m_cellAnchoredShapes.contains(cell.sheet())) {
            return QList<KoShape*>();
        } else if (!m_cellAnchoredShapes[cell.sheet()].contains(cell.row())) {
            return QList<KoShape*>();
        }
        return m_cellAnchoredShapes[cell.sheet()][cell.row()].values(cell.column());
    }

public:
    KoShapeSavingContext& shapeContext;
    GenValidationStyles valStyle;
    QMap<int, Style> columnDefaultStyles;
    QMap<int, Style> rowDefaultStyles;

private:
    QHash<const Sheet*, QHash<int /*row*/, QMultiHash<int /*col*/, KoShape*> > > m_cellAnchoredShapes;
};

} // namespace KSpread

#endif // KSPREAD_ODF_SAVING_CONTEXT
