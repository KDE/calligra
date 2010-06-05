/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

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

#ifndef KSPREAD_DEPENDENCY_MANAGER
#define KSPREAD_DEPENDENCY_MANAGER

#include <QObject>

#include "Region.h"

namespace KSpread
{
class Region;

/**
 * Manages the dependencies between cells caused by references in formulas.
 * This dependency information is used for the recalculation of the cells.
 */
class KSPREAD_EXPORT DependencyManager : public QObject
{
    Q_OBJECT
    friend class TestDependencies;
    friend class RecalcManager;

public:
    /** constructor */
    DependencyManager(const Map* map);
    /** destructor */
    ~DependencyManager();

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
    void regionChanged(const Region& region);

    /**
     * Updates the whole \p map.
     */
    void updateAllDependencies(const Map* map);

    /**
     * Returns the cell depths.
     * \return the cell depths
     */
    QHash<Cell, int> depths() const;

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
    Region consumingRegion(const Cell& cell) const;

    /**
     * Returns the region, that is reduced to those parts of \p region, that provide values.
     * \return region providing values for others
     */
    Region reduceToProvidingRegion(const Region& region) const;

    /**
     * Adjusts formulas after cut & paste operations or column/row insertions/deletions.
     *
     * \param movedRegion the region, that was moved
     * \param destination the new upper left corner of the region
     */
    void regionMoved(const Region& movedRegion, const Cell& destination);

public Q_SLOTS:
    void namedAreaModified(const QString&);

    /**
     * Called after a sheet was added.
     */
    void addSheet(Sheet *sheet);

    /**
     * Called after a sheet was removed.
     */
    void removeSheet(Sheet *sheet);

protected:
    /**
     * \param cell the cell which formula should  be altered
     * \param oldLocation the location/range, that was cut
     * \param offset the relative movement and new sheet, if applicable
     *
     * \see regionMoved()
     */
    void updateFormula(const Cell& cell, const Region::Element* oldLocation, const Region::Point& offset);

private:
    Q_DISABLE_COPY(DependencyManager)

    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_DEPENDENCY_MANAGER
