// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_RECALC_MANAGER
#define CALLIGRA_SHEETS_RECALC_MANAGER

#include "sheets_engine_export.h"
#include <Region.h>

#include <QObject>

namespace Calligra
{
namespace Sheets
{
class CellBase;
class MapBase;
class SheetBase;
class Updater;

/**
 * \class RecalcManager
 * \brief Manages the recalculations of cells containing a formula.
 * \ingroup Value
 *
 * The recalculations of a cell region, a sheet or the map are based
 * on the following principle:
 *
 * A cell could refer to other cells, which need to be recalculated
 * before. The order of recalculation is determined by the depth of
 * references, i.e. first the cells, which do not refer to other cells,
 * are recalculated. Cells referring to those are next. Then cells, which
 * refer to the ones in the last step follow and so on until all cells
 * have been updated.
 *
 * Cell value changes are blocked while doing this, i.e. they do not
 * trigger a new recalculation event.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT RecalcManager : public QObject
{
    Q_OBJECT
public:
    /**
     * Creates a RecalcManager. It is used for a whole map.
     *
     * \param map The Map which this RecalcManager belongs to.
     */
    explicit RecalcManager(MapBase *const map);

    /**
     * Destructor.
     */
    ~RecalcManager() override;

    /**
     * Recalculates the cells referring to cells in \p region .
     * The cells are recalculated sorted by the reference depth in ascending order.
     *
     * \see recalc()
     */
    void regionChanged(const Region &region);

    /**
     * Recalculates the sheet \p sheet .
     * The cells are recalculated sorted by the reference depth in ascending order.
     *
     * \see recalc()
     */
    void recalcSheet(SheetBase *const sheet);

    /**
     * Recalculates the whole map.
     * The cells are recalculated sorted by the reference depth in ascending order.
     *
     * \see recalc()
     */
    void recalcMap(Updater *updater = nullptr);

    /**
     * Returns the recalculation state.
     * \return \c true, if recalculations are in progress
     */
    bool isActive() const;

    /**
     * Prints out the cell depths in the current recalculation event.
     */
    void dump() const;

public Q_SLOTS:
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
     * Iterates over the map of cell with their reference depths
     * and calls recalcCell().
     *
     * \see recalcCell()
     */
    void recalc(Updater *updater = nullptr);

private:
    Q_DISABLE_COPY(RecalcManager)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RECALC_MANAGER
