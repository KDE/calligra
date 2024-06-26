/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_ODF_SAVING_CONTEXT
#define KSPREAD_ODF_SAVING_CONTEXT

#include "GenValidationStyle.h"
#include "core/Style.h"
#include "engine/calligra_sheets_limits.h"

class KoShape;

namespace Calligra
{
namespace Sheets
{

class Sheet;

namespace Odf
{

/**
 * \ingroup OpenDocument
 * Data used while saving.
 */
class OdfSavingContext
{
public:
    explicit OdfSavingContext(KoShapeSavingContext &shapeContext)
        : shapeContext(shapeContext)
    {
    }

    void insertCellAnchoredShape(const Sheet *sheet, int row, int column, KoShape *shape)
    {
        Q_ASSERT_X(1 <= column && column <= KS_colMax, __FUNCTION__, QString("%1 out of bounds").arg(column).toLocal8Bit());
        Q_ASSERT_X(1 <= row && row <= KS_rowMax, __FUNCTION__, QString("%1 out of bounds").arg(row).toLocal8Bit());
        m_cellAnchoredShapes[sheet][row].insert(column, shape);
    }

    bool rowHasCellAnchoredShapes(const Sheet *sheet, int row) const
    {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it == m_cellAnchoredShapes.constEnd())
            return false;
        return (*it).contains(row);
    }

    bool cellHasAnchoredShapes(const Sheet *sheet, int row, int column) const
    {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it == m_cellAnchoredShapes.constEnd())
            return false;
        AnchoredShape::const_iterator rit = (*it).constFind(row);
        if (rit == (*it).constEnd())
            return false;
        return (*rit).contains(column);
    }

    int nextAnchoredShape(const Sheet *sheet, int row, int column) const
    {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it != m_cellAnchoredShapes.constEnd()) {
            AnchoredShape::const_iterator rit = (*it).constFind(row);
            if (rit != (*it).constEnd()) {
                QMultiHash<int, KoShape *>::const_iterator cit((*rit).constBegin()), cend((*rit).constEnd());
                for (; cit != cend; ++cit)
                    if (cit.key() > column)
                        return cit.key(); // found one
            }
        }
        return 0;
    }

    QList<KoShape *> cellAnchoredShapes(const Sheet *sheet, int row, int column) const
    {
        AnchoredShapes::const_iterator it = m_cellAnchoredShapes.constFind(sheet);
        if (it != m_cellAnchoredShapes.constEnd()) {
            AnchoredShape::const_iterator rit = (*it).constFind(row);
            if (rit != (*it).constEnd()) {
                return (*rit).values(column);
            }
        }
        return QList<KoShape *>();
    }

public:
    KoShapeSavingContext &shapeContext;
    GenValidationStyles valStyle;
    QMap<int, Style> columnDefaultStyles;
    QMap<int, Style> rowDefaultStyles;

private:
    typedef QHash<int /*row*/, QMultiHash<int /*col*/, KoShape *>> AnchoredShape;
    typedef QHash<const Sheet *, AnchoredShape> AnchoredShapes;
    AnchoredShapes m_cellAnchoredShapes;
};

} // namespace Odf
} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_ODF_SAVING_CONTEXT
