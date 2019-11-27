/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus stefan.nikolaus@kdemail.net

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

#ifndef KSPREAD_READWRITE_TABLE_MODEL
#define KSPREAD_READWRITE_TABLE_MODEL

#include "ReadOnlyTableModel.h"

namespace KSpread
{

class ReadWriteTableModel : public ReadOnlyTableModel
{
public:
    /**
     * Constructor.
     */
    explicit ReadWriteTableModel(Sheet* sheet, int columns = 0, int rows = 0);

    /**
     * Destructor.
     */
    ~ReadWriteTableModel() override;

    // QAbstractTableModel interface
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_READWRITE_TABLE_MODEL
