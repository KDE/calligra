/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DEPENDENCY_MANAGER
#define CALLIGRA_SHEETS_DEPENDENCY_MANAGER

#include "Region.h"
#include "sheets_engine_export.h"

#include <QObject>

namespace Calligra
{
namespace Sheets
{
class CellBase;
class MapBase;
class Region;
class SheetBase;
class Updater;

/**
 * \ingroup Value
 * Manages the dependencies between cells caused by references in formulas.
 * This dependency information is used for the recalculation of the cells.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT DependencyManager : public QObject
{
    Q_OBJECT
    friend class TestDependencies;
    friend class RecalcManager;

public:
    /** constructor */
    explicit DependencyManager(const MapBase *map);
    /** destructor */
    ~DependencyManager() override;

    /** clear all data */
    void reset();

    /**
     * Handles the fact, that formulas have changed in \p region.
     * The \p region needs to contain only those areas, in which
     * each cell has a changed formula. That can also be a removed
     * formula. This class has no chance to know the old formula
     * locations, but the caller of this method has. So, usually the
     * \p region consists of several cell locations, not cell ranges.
     * The caller has to take care of that, because each and every
     * cell in \p region is traversed.
     */
    void regionChanged(const Region &region);

    /**
     * Updates the whole \p map.
     */
    void updateAllDependencies(const MapBase *map, Updater *updater = nullptr);

    /**
     * Returns the cell depths.
     * \return the cell depths
     */
    QMap<CellBase, int> depths() const;

    /**
     * Returns the region, that consumes the value of \p cell.
     *
     * I.e. the returned region contains all cells, that have
     * got a formula referencing \p cell. Even if the formula
     * references a complete cell range or a named area, that
     * contains \p cell.
     *
     * \return region consuming \p cell 's value
     */
    Region consumingRegion(const CellBase &cell) const;

    /**
     * Returns the region, that is reduced to those parts of \p region, that provide values.
     * \return region providing values for others
     */
    Region reduceToProvidingRegion(const Region &region) const;

    /**
     * Adjusts formulas after cut & paste operations or column/row insertions/deletions.
     *
     * \param movedRegion the region, that was moved
     * \param destination the new upper left corner of the region
     */
    void regionMoved(const Region &movedRegion, const CellBase &destination);

public Q_SLOTS:
    void namedAreaModified(const QString &);

    /**
     * Called after a sheet was added.
     */
    void addSheet(SheetBase *sheet);

    /**
     * Called after a sheet was removed.
     */
    void removeSheet(SheetBase *sheet);

protected:
    /**
     * \param cell the cell which formula should  be altered
     * \param oldLocation the location/range, that was cut
     * \param offset the relative movement and new sheet, if applicable
     *
     * \see regionMoved()
     */
    void updateFormula(const CellBase &cell, const Region::Element *oldLocation, const Region::Point &offset);

private:
    Q_DISABLE_COPY(DependencyManager)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DEPENDENCY_MANAGER
