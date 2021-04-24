/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_READONLY_TABLE_MODEL
#define KSPREAD_READONLY_TABLE_MODEL

#include <QAbstractTableModel>

namespace KSpread
{
class Sheet;

class ReadOnlyTableModel : public QAbstractTableModel
{
public:
    /**
     * Constructor.
     */
    explicit ReadOnlyTableModel(Sheet* sheet, int columns = 0, int rows = 0);

    /**
     * Destructor.
     */
    ~ReadOnlyTableModel() override;

    // QAbstractTableModel interface
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

protected:
    Sheet* sheet() const;
    const QSize& size() const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_READONLY_TABLE_MODEL
