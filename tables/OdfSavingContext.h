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

namespace Calligra
{
namespace Tables
{

/**
 * \ingroup OpenDocument
 * Data used while saving.
 */
class OdfSavingContext
{
public:
    OdfSavingContext(KoShapeSavingContext& shapeContext)
            : shapeContext(shapeContext) {}

    void insertCellAnchoredShape(const Sheet *sheet, int row, int column, KoShape* shape) {
        m_cellAnchoredShapes[sheet][row].insert(column, shape);
    }

    bool rowHasCellAnchoredShapes(const Sheet* sheet, int row) const {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it == m_cellAnchoredShapes.constEnd())
            return false;
        return (*it).contains(row);
    }

    bool cellHasAnchoredShapes(const Sheet *sheet, int row, int column) const {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it == m_cellAnchoredShapes.constEnd())
            return false;
        AnchoredShape::const_iterator rit = (*it).constFind(row);
        if (rit == (*it).constEnd())
            return false;
        return (*rit).contains(column);
    }

    int nextAnchoredShape(const Sheet *sheet, int row, int column) const {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it != m_cellAnchoredShapes.constEnd()) {
            AnchoredShape::const_iterator rit = (*it).constFind(row);
            if (rit != (*it).constEnd()) {
                QMultiHash<int, KoShape*>::const_iterator cit((*rit).constBegin()), cend((*it).constEnd());
                for (; cit != cend; ++cit)
                    if (cit.key() > column) return cit.key();  // found one

            }
        }
        return 0;
    }

    QList<KoShape*> cellAnchoredShapes(const Sheet *sheet, int row, int column) const {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it != m_cellAnchoredShapes.constEnd()) {
            AnchoredShape::const_iterator rit = (*it).constFind(row);
            if (rit != (*it).constEnd()) {
                return (*rit).values(column);
            }
        }
        return QList<KoShape*>();
    }

public:
    KoShapeSavingContext& shapeContext;
    GenValidationStyles valStyle;
    QMap<int, Style> columnDefaultStyles;
    QMap<int, Style> rowDefaultStyles;

private:
    typedef QHash < int /*row*/, QMultiHash < int /*col*/, KoShape* > > AnchoredShape;
    typedef QHash < const Sheet*, AnchoredShape > AnchoredShapes;
    AnchoredShapes m_cellAnchoredShapes;
};

} // namespace Tables
} // namespace Calligra

#endif // KSPREAD_ODF_SAVING_CONTEXT
