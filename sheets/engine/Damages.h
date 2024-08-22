// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef CALLIGRA_SHEETS_DAMAGES
#define CALLIGRA_SHEETS_DAMAGES

#include "sheets_engine_export.h"

#include <QDebug>

namespace Calligra
{
namespace Sheets
{
class CellBase;
class MapBase;
class SheetBase;
class Region;

/**
 * \ingroup Damages
 * An abstract damage.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT Damage
{
public:
    virtual ~Damage() = default;

    typedef enum { Nothing = 0, Document, Workbook, Sheet, Range, Cell, Selection } Type;

    virtual Type type() const
    {
        return Nothing;
    }
};

/**
 * \ingroup Damages
 * A cell range damage.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT CellDamage : public Damage
{
public:
    enum Change {
        Binding = 0x02, ///< on value changes; always triggered; for binding updates
        Formula = 0x04, ///< triggers a dependency update
        NamedArea = 0x10, ///< triggers a named area update
        /// This indicates a value change. It is not triggered while a recalculation is in progress.
        /// RecalcManager takes over in this case. Otherwise, circular dependencies would cause
        /// infinite loops and the cells would be recalculated in arbitrary order.
        Value = 0x20,
        /// On style changes; invalidates the style storage cache.
        StyleCache = 0x40,
        /// The visual cache gets damaged, if any of CellView's data members is
        /// affected. E.g. the displayed text, the cell dimension or the merging.
        VisualCache = 0x80,
        // TODO Stefan: Detach the style cache from the CellView cache.
        /// Updates the caches and triggers a repaint of the cell region.
        Appearance = StyleCache | VisualCache
    };
    Q_DECLARE_FLAGS(Changes, Change)

    CellDamage(const Calligra::Sheets::CellBase &cell, Changes changes);
    CellDamage(Calligra::Sheets::SheetBase *sheet, const Region &region, Changes changes);

    ~CellDamage() override;

    Type type() const override
    {
        return Damage::Cell;
    }

    Calligra::Sheets::SheetBase *sheet() const;
    const Region &region() const;

    Changes changes() const;

private:
    Q_DISABLE_COPY(CellDamage)

    class Private;
    Private *const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(CellDamage::Changes)

/**
 * \ingroup Damages
 * A sheet damage.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT SheetDamage : public Damage
{
public:
    enum Change {
        None = 0x00,
        ContentChanged = 0x01,
        PropertiesChanged = 0x02,
        Hidden = 0x04,
        Shown = 0x08,
        Name = 0x10,
        ColumnsChanged = 0x20,
        RowsChanged = 0x40
    };
    Q_DECLARE_FLAGS(Changes, Change)

    SheetDamage(Calligra::Sheets::SheetBase *sheet, Changes changes);

    ~SheetDamage() override;

    Type type() const override
    {
        return Damage::Sheet;
    }

    Calligra::Sheets::SheetBase *sheet() const;

    Changes changes() const;

private:
    Q_DISABLE_COPY(SheetDamage)

    class Private;
    Private *const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SheetDamage::Changes)

/**
 * \ingroup Damages
 * A workbook damage.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT WorkbookDamage : public Damage
{
public:
    enum Change { None = 0x00, Formula = 0x01, Value = 0x02 };
    Q_DECLARE_FLAGS(Changes, Change)

    WorkbookDamage(Calligra::Sheets::MapBase *map, Changes changes);
    ~WorkbookDamage() override;

    Type type() const override
    {
        return Damage::Workbook;
    }
    Calligra::Sheets::MapBase *map() const;
    Changes changes() const;

private:
    Q_DISABLE_COPY(WorkbookDamage)

    class Private;
    Private *const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(WorkbookDamage::Changes)

/**
 * \ingroup Damages
 * A selection damage.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT SelectionDamage : public Damage
{
public:
    explicit SelectionDamage(const Region &region);
    ~SelectionDamage() override;

    Type type() const override
    {
        return Damage::Selection;
    }

    const Region &region() const;

private:
    Q_DISABLE_COPY(SelectionDamage)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

/***************************************************************************
  QDebug support
****************************************************************************/

CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug str, const Calligra::Sheets::Damage &d);
CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug str, const Calligra::Sheets::CellDamage &d);
CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug str, const Calligra::Sheets::SheetDamage &d);
CALLIGRA_SHEETS_ENGINE_EXPORT QDebug operator<<(QDebug str, const Calligra::Sheets::SelectionDamage &d);

#endif // CALLIGRA_SHEETS_DAMAGES
