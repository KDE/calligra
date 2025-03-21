/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2004 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_DEPENDENCY_MANAGER_P
#define KSPREAD_DEPENDENCY_MANAGER_P

// Local
#include "DependencyManager.h"

#include "CellBase.h"
#include "RTree.h"

namespace Calligra
{
namespace Sheets
{
class Formula;
class MapBase;
class SheetBase;

class Q_DECL_HIDDEN DependencyManager::Private
{
public:
    /**
     * Clears internal structures.
     */
    void reset();

    /**
     * Generates the dependencies of \p cell .
     * First, it removes the old providing region. Then, the new providing
     * region is computed. Finally, adds \p cell as consumer and the new
     * providing region to the data structures.
     * \see removeDependencies
     * \see computeDependencies
     */
    void generateDependencies(const CellBase &cell, const Formula &formula);

    /**
     * Computes the reference depth.
     * Depth means the maximum depth of all cells this cell depends on plus one,
     * while a cell, which do not refer to other cells, has a depth
     * of zero.
     *
     * Examples:
     * \li A1: '=1.0'
     * \li A2: '=A1+A1'
     * \li A3: '=A1+A1+A2'
     *
     * \li depth(A1) = 0
     * \li depth(A2) = 1
     * \li depth(A3) = 2
     */
    int computeDepth(CellBase cell) const;

    /**
     * Used in the recalculation events for changed regions.
     * Determines the reference depth for each position in \p region .
     *
     * \see computeDepth
     * \see generateDepths(CellBase cell)
     */
    void generateDepths(const Region &region);

    /**
     * Generates the depth of cell and all of its consumers.
     * Calls itself recursively for the cell's consuming cells.
     */
    void generateDepths(CellBase cell, QSet<CellBase> &computedDepths);

    /**
     * Returns the region, that consumes the value of \p cell.
     * \see DependencyManager::consumingRegion(const CellBase&)
     * \return region consuming \p cell 's value
     */
    Region consumingRegion(const CellBase &cell) const;

    void namedAreaModified(const QString &name);

    /**
     * Removes all dependencies of \p cell .
     */
    void removeDependencies(const CellBase &cell);

    /**
     * Removes the depths of \p cell and all its consumers.
     */
    void removeDepths(const CellBase &cell);

    /**
     * Computes and stores the dependencies.
     *
     * Parses \p formula and adds each contained reference to a
     * cell, a cell range or a named area to the providing regions
     * of \p cell.
     * Additionally, the opposite direction is also stored:
     * Each consumed region, i.e. each reference, points to \p cell.
     *
     * The \p formula has to belong to \p cell. It would have been
     * possible to look it up from \p cell, but is passed separately
     * for performance reasons.
     * Do not call this method for a \p cell not containing a \p formula.
     */
    void computeDependencies(const CellBase &cell, const Formula &formula);

    enum Direction {
        Forward,
        Backward
    };
    /**
     * Removes the circular dependency flag from \p region and all their dependencies.
     */
    void removeCircularDependencyFlags(const Region &region, Direction direction);

    /**
     * For debugging/testing purposes.
     */
    void dump() const;

    const MapBase *map;
    // stores providing regions ordered by their consuming cell locations
    // use QMap rather then QHash cause it's faster for our use-case
    QMap<CellBase, Region> providers;
    // stores consuming cell locations ordered by their providing regions
    QHash<SheetBase *, RTree<CellBase> *> consumers;
    // stores consuming cell locations ordered by their providing named area
    // (in addition to the general storage of the consuming cell locations)
    QHash<QString, QList<CellBase>> namedAreaConsumers;
    /*
     * Stores cells with its reference depth.
     * Depth means the maximum depth of all cells this cell depends on plus one,
     * while a cell which has a formula without cell references has a depth
     * of zero.
     *
     * Examples:
     * \li A1: '=1.0'
     * \li A2: '=A1+A1'
     * \li A3: '=A1+A1+A2'
     *
     * \li depth(A1) = 0
     * \li depth(A2) = 1
     * \li depth(A3) = 2
     */
    // use QMap rather then QHash cause it's faster for our use-case
    QMap<CellBase, int> depths;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_DEPENDENCY_MANAGER_P
