/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DAMAGES
#define KSPREAD_DAMAGES


#include "kdebug.h"

#include "kspread_export.h"

namespace KSpread
{
class Cell;
class Map;
class Sheet;
class Region;

/**
 * \ingroup Damages
 * An abstract damage.
 */
class KSPREAD_EXPORT Damage
{
public:
    virtual ~Damage() {}

    typedef enum {
        Nothing = 0,
        Document,
        Workbook,
        Sheet,
        Range,
        Cell,
        Selection
    } Type;

    virtual Type type() const {
        return Nothing;
    }
};

/**
 * \ingroup Damages
 * A cell range damage.
 */
class KSPREAD_EXPORT CellDamage : public Damage
{
public:
    enum Change {
        Appearance = 0x01, ///< triggers a layout update and then a repaint
        Binding    = 0x02, ///< on value changes; always triggered; for binding updates
        Formula    = 0x04, ///< triggers a dependency update
        NamedArea  = 0x10, ///< triggers a named area update
        /// This indicates a value change. It is not triggered while a recalculation is in progress.
        /// RecalcManager takes over in this case. Otherwise, circular dependencies would cause
        /// infinite loops and the cells would be recalculated in arbitrary order.
        Value      = 0x20
    };
    Q_DECLARE_FLAGS(Changes, Change)

    CellDamage(const KSpread::Cell& cell, Changes changes);
    CellDamage(KSpread::Sheet* sheet, const Region& region, Changes changes);

    virtual ~CellDamage();

    virtual Type type() const {
        return Damage::Cell;
    }

    KSpread::Sheet* sheet() const;
    const Region& region() const;

    Changes changes() const;

private:
    Q_DISABLE_COPY(CellDamage)

    class Private;
    Private * const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(CellDamage::Changes)


/**
 * \ingroup Damages
 * A sheet damage.
 */
class KSPREAD_EXPORT SheetDamage : public Damage
{
public:

    enum Change {
        None              = 0x00,
        ContentChanged    = 0x01,
        PropertiesChanged = 0x02,
        Hidden            = 0x04,
        Shown             = 0x10,
        ColumnsChanged    = 0x20,
        RowsChanged       = 0x40
    };
    Q_DECLARE_FLAGS(Changes, Change)

    SheetDamage(KSpread::Sheet* sheet, Changes changes);

    virtual ~SheetDamage();

    virtual Type type() const {
        return Damage::Sheet;
    }

    KSpread::Sheet* sheet() const;

    Changes changes() const;

private:
    Q_DISABLE_COPY(SheetDamage)

    class Private;
    Private * const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SheetDamage::Changes)


/**
 * \ingroup Damages
 * A workbook damage.
 */
class WorkbookDamage : public Damage
{
public:
    enum Change {
        None       = 0x00,
        Formula    = 0x01,
        Value      = 0x02
    };
    Q_DECLARE_FLAGS(Changes, Change)

    WorkbookDamage(KSpread::Map* map, Changes changes);
    virtual ~WorkbookDamage();

    virtual Type type() const {
        return Damage::Workbook;
    }
    KSpread::Map* map() const;
    Changes changes() const;

private:
    Q_DISABLE_COPY(WorkbookDamage)

    class Private;
    Private * const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(WorkbookDamage::Changes)


/**
 * \ingroup Damages
 * A selection damage.
 */
class KSPREAD_EXPORT SelectionDamage : public Damage
{
public:
    SelectionDamage(const Region& region);
    virtual ~SelectionDamage();

    virtual Type type() const {
        return Damage::Selection;
    }

    const Region& region() const;

private:
    Q_DISABLE_COPY(SelectionDamage)

    class Private;
    Private * const d;
};

} // namespace KSpread


/***************************************************************************
  kDebug support
****************************************************************************/

kdbgstream operator<<(kdbgstream str, const KSpread::Damage& d);
kdbgstream operator<<(kdbgstream str, const KSpread::CellDamage& d);
kdbgstream operator<<(kdbgstream str, const KSpread::SheetDamage& d);
kdbgstream operator<<(kdbgstream str, const KSpread::SelectionDamage& d);

#endif // KSPREAD_DAMAGES
