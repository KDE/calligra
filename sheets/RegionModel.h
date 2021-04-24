/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_REGION_MODEL
#define CALLIGRA_SHEETS_REGION_MODEL

#include "SheetModel.h"

namespace Calligra
{
namespace Sheets
{
class Region;

/**
 * A model for a contiguous cell region.
 * \ingroup Model
 */
class RegionModel : public SheetModel
{
public:
    explicit RegionModel(const Region &region);
    ~RegionModel() override;

    /**
     * Existing data gets overwritten.
     * The cell region gets the sheet boundaries as maxima.
     */
    void setOverwriteMode(bool enable);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_REGION_MODEL
