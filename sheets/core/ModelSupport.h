/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_MODEL_SUPPORT
#define KSPREAD_MODEL_SUPPORT

#include <QAbstractItemModel>
#include <QItemSelection>
#include <QRect>

namespace Calligra
{
namespace Sheets
{

/**
 * Item roles representing sheet data.
 * \ingroup Model
 */
enum SheetDataRole {
    // Qt::UserRole = 32
    // Sheet properties; MapModel, MapViewModel
    VisibilityRole = Qt::UserRole, ///< sheet visibility; bool
    ProtectionRole, ///< sheet protection; bool
    ActivityRole ///< active sheet; bool
};

/**
 * Item roles representing cell data.
 * \ingroup Model
 */
enum CellDataRole {
    // Qt::UserRole     = 0x00000020 = 32
    NoCellDataRole = Qt::UserRole, ///< used for iterating over all data, default and non-default
    // Cell contents; SheetModel, RegionModel
    UserInputRole = 0x00000100, ///< cell's user input; QString
    FormulaRole = 0x00000200, ///< cell's formula; Formula
    ValueRole = 0x00000400, ///< cell's value; Value
    LinkRole = 0x00000800, ///< cell's hyperlink; QString
    RichTextRole = 0x00001000, ///< cell's rich text; QSharedPointer<QTextDocument>
    // Cell range associations; SheetModel, RegionModel
    CommentRole = 0x00002000, ///< a comment; QString
    ConditionRole = 0x00004000, ///< a conditional style; Conditions
    StyleRole = 0x00008000, ///< a style; Style
    ValidityRole = 0x00010000, ///< a cell validition; Validity
    FusionedRangeRole = 0x00020000, ///< a fusioned cell range; bool
    LockedRangeRole = 0x00040000, ///< a locked cell range; bool
    NamedAreaRole = 0x00080000, ///< a named area; QString
    SourceRangeRole = 0x00100000, ///< a source cell range; Binding
    TargetRangeRole = 0x00200000, ///< a target cell range; Database
    AllCellDataRoles = 0x00FFFF00 ///< used for iterating over the non-default data only
};
Q_DECLARE_FLAGS(CellDataRoles, CellDataRole)

/**
 * Converts an item range into a range in cell coordinates.
 * \ingroup Model
 */
static inline QRect toRange(const QItemSelectionRange &range)
{
    return QRect(range.left() + 1, range.top() + 1, range.width(), range.height());
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

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_MODEL_SUPPORT
