/* This file is part of the KDE project
   Copyright 2010 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_MODEL_SUPPORT
#define KSPREAD_MODEL_SUPPORT

#include <QAbstractItemModel>
#include <QItemSelection>

#include "Cell.h"
#include "Region.h"
#include "Sheet.h"

namespace KSpread
{

/**
 * Item roles representing sheet data.
 * \ingroup Model
 */
enum SheetDataRole {
    // Qt::UserRole = 32
    // Sheet properties; MapModel, MapViewModel
    VisibilityRole      = Qt::UserRole, ///< sheet visibility; bool
    ProtectionRole,                     ///< sheet protection; bool
    ActivityRole                        ///< active sheet; bool
};


/**
 * Item roles representing cell data.
 * \ingroup Model
 */
enum CellDataRole {
    // Qt::UserRole     = 0x00000020 = 32
    NoCellDataRole      = Qt::UserRole, ///< used for iterating over all data, default and non-default
    // Cell contents; SheetModel, RegionModel
    UserInputRole       = 0x00000100,   ///< cell's user input; QString
    FormulaRole         = 0x00000200,   ///< cell's formula; Formula
    ValueRole           = 0x00000400,   ///< cell's value; Value
    LinkRole            = 0x00000800,   ///< cell's hyperlink; QString
    RichTextRole        = 0x00001000,   ///< cell's rich text; QSharedPointer<QTextDocument>
    // Cell range associations; SheetModel, RegionModel
    CommentRole         = 0x00002000,   ///< a comment; QString
    ConditionRole       = 0x00004000,   ///< a conditional style; Conditions
    StyleRole           = 0x00008000,   ///< a style; Style
    ValidityRole        = 0x00010000,   ///< a cell validition; Validity
    FusionedRangeRole   = 0x00020000,   ///< a fusioned cell range; bool
    LockedRangeRole     = 0x00040000,   ///< a locked cell range; bool
    NamedAreaRole       = 0x00080000,   ///< a named area; QString
    SourceRangeRole     = 0x00100000,   ///< a source cell range; Binding
    TargetRangeRole     = 0x00200000,   ///< a target cell range; Database
    AllCellDataRoles    = 0x00FFFF00    ///< used for iterating over the non-default data only
};
Q_DECLARE_FLAGS(CellDataRoles, CellDataRole)


/**
 * Converts a model index into a Cell.
 * \ingroup Model
 */
static inline Cell toCell(const QModelIndex &index)
{
    const int column = index.column() + 1;
    const int row = index.row() + 1;
    Sheet *const sheet = static_cast<Sheet*>(index.internalPointer());
    return Cell(sheet, column, row);
}

/**
 * Converts a model index into cell coordinates.
 * \ingroup Model
 */
static inline QPoint toPoint(const QModelIndex &index)
{
    const int column = index.column() + 1;
    const int row = index.row() + 1;
    return QPoint(column, row);
}

/**
 * Converts an item range into a range in cell coordinates.
 * \ingroup Model
 */
static inline QRect toRange(const QItemSelectionRange &range)
{
    return QRect(range.left() + 1, range.top() + 1, range.width(), range.height());
}

/**
 * Converts an item selection into a cell region.
 * \ingroup Model
 */
static inline Region toRegion(const QItemSelection &selection)
{
    Region region;
    for (int i = 0; i < selection.count(); ++i) {
        const QItemSelectionRange range = selection[i];
        Sheet *const sheet = static_cast<Sheet*>(range.topLeft().internalPointer());
        region.add(toRange(range), sheet);
    }
    return region;
}

/**
 * Converts a range in cell coordinates into a model item range.
 * \ingroup Model
 */
static inline QItemSelectionRange fromRange(const QRect &range, const QAbstractItemModel *const model)
{
    const QModelIndex topLeft = model->index(range.top() - 1, range.left() - 1);
    const QModelIndex bottomRight = model->index(range.bottom() - 1, range.right() - 1);
    return QItemSelectionRange(topLeft, bottomRight);
}

/**
 * Converts a range in cell coordinates into a model item range.
 * \ingroup Model
 */
static inline QItemSelectionRange fromRange(const QRect &range, Sheet *const sheet)
{
    return fromRange(range, sheet->model());
}

/**
 * Converts a cell region into an item selection.
 * \ingroup Model
 */
static inline QItemSelection fromRegion(const Region &region)
{
    QItemSelection selection;
    for (Region::ConstIterator it = region.constBegin(), end = region.constEnd(); it != end; ++it) {
        selection.append(fromRange((*it)->rect(), (*it)->sheet()));
    }
    return selection;
}

} // namespace KSpread

#endif // KSPREAD_MODEL_SUPPORT
