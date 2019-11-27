/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus stefan.nikolaus@kdemail.net

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

#ifndef CALLIGRA_SHEETS_SHEET_MODEL
#define CALLIGRA_SHEETS_SHEET_MODEL

#include <QAbstractTableModel>

#include "sheets_odf_export.h"

class QItemSelectionRange;

namespace Calligra
{
namespace Sheets
{
class Sheet;

/**
 * A model for a sheet.
 * \ingroup Model
 */
class CALLIGRA_SHEETS_ODF_EXPORT SheetModel : public QAbstractTableModel
{
public:
    explicit SheetModel(Sheet* sheet);
    ~SheetModel() override;

    // QAbstractTableModel interface
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    bool setData(const QItemSelectionRange &range, const QVariant &value, int role = Qt::EditRole);
    bool setData(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                 const QVariant &value, int role = Qt::EditRole);

protected:
    Sheet* sheet() const;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHEET_MODEL
