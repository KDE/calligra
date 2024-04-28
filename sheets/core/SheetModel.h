/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHEET_MODEL
#define CALLIGRA_SHEETS_SHEET_MODEL

#include <QAbstractTableModel>

#include "sheets_core_export.h"

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
class CALLIGRA_SHEETS_CORE_EXPORT SheetModel : public QAbstractTableModel
{
public:
    explicit SheetModel(Sheet *sheet);
    ~SheetModel() override;

    // QAbstractTableModel interface
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setData(const QItemSelectionRange &range, const QVariant &value, int role = Qt::EditRole);
    bool setData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVariant &value, int role = Qt::EditRole);

protected:
    Sheet *sheet() const;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHEET_MODEL
