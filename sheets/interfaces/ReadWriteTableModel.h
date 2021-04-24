/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
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
