/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_READONLY_REGION_MODEL
#define KSPREAD_READONLY_REGION_MODEL

#include <QAbstractProxyModel>

namespace KSpread
{
class Region;

/**
 * A model for a contiguous cell region.
 */
class ReadOnlyRegionModel : QAbstractProxyModel
{
public:
    /**
     * Constructor.
     */
    explicit ReadOnlyRegionModel(const Region &region);

    /**
     * Destructor.
     */
    ~ReadOnlyRegionModel();

    // QAbstractTableModel interface
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;

    // QAbstractProxyModel interface
    virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_READONLY_REGION_MODEL
