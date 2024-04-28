// This file is part of the KDE project
// SPDX-FileCopyrightText: 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-License-Identifier: LGPL-2.0-only

// Local
#include "Damages.h"

#include "CellBase.h"
#include "Region.h"
#include "SheetBase.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN WorkbookDamage::Private
{
public:
    Calligra::Sheets::MapBase *map;
    Changes changes;
};

class Q_DECL_HIDDEN SheetDamage::Private
{
public:
    Calligra::Sheets::SheetBase *sheet;
    Changes changes;
};

class Q_DECL_HIDDEN CellDamage::Private
{
public:
    Calligra::Sheets::SheetBase *sheet;
    Region region;
    Changes changes;
};

class Q_DECL_HIDDEN SelectionDamage::Private
{
public:
    Region region;
};

CellDamage::CellDamage(const Calligra::Sheets::CellBase &cell, Changes changes)
    : d(new Private)
{
    d->sheet = cell.sheet();
    if (Region::isValid(QPoint(cell.column(), cell.row())))
        d->region = Region(cell.column(), cell.row(), d->sheet);
    d->changes = changes;
}

CellDamage::CellDamage(Calligra::Sheets::SheetBase *sheet, const Region &region, Changes changes)
    : d(new Private)
{
    d->sheet = sheet;
    d->region = region;
    d->changes = changes;
}

CellDamage::~CellDamage()
{
    delete d;
}

SheetBase *CellDamage::sheet() const
{
    return d->sheet;
}

const Calligra::Sheets::Region &CellDamage::region() const
{
    return d->region;
}

CellDamage::Changes CellDamage::changes() const
{
    return d->changes;
}

SheetDamage::SheetDamage(Calligra::Sheets::SheetBase *sheet, Changes changes)
    : d(new Private)
{
    d->sheet = sheet;
    d->changes = changes;
}

SheetDamage::~SheetDamage()
{
    delete d;
}

SheetBase *SheetDamage::sheet() const
{
    return d->sheet;
}

SheetDamage::Changes SheetDamage::changes() const
{
    return d->changes;
}

WorkbookDamage::WorkbookDamage(Calligra::Sheets::MapBase *map, Changes changes)
    : d(new Private)
{
    d->map = map;
    d->changes = changes;
}

WorkbookDamage::~WorkbookDamage()
{
    delete d;
}

MapBase *WorkbookDamage::map() const
{
    return d->map;
}

WorkbookDamage::Changes WorkbookDamage::changes() const
{
    return d->changes;
}

SelectionDamage::SelectionDamage(const Region &region)
    : d(new Private)
{
    d->region = region;
}

SelectionDamage::~SelectionDamage()
{
    delete d;
}

const Calligra::Sheets::Region &SelectionDamage::region() const
{
    return d->region;
}

/***************************************************************************
  QDebug support
****************************************************************************/

QDebug operator<<(QDebug str, const Calligra::Sheets::Damage &d)
{
    switch (d.type()) {
    case Damage::Nothing:
        return str << "NoDamage";
    case Damage::Document:
        return str << "Document";
    case Damage::Workbook:
        return str << "Workbook";
    case Damage::Sheet:
        return str << "Sheet";
    case Damage::Range:
        return str << "Range";
    case Damage::Cell:
        return str << "Cell";
    case Damage::Selection:
        return str << "Selection";
    }
    return str;
}

QDebug operator<<(QDebug str, const Calligra::Sheets::CellDamage &d)
{
    str << "CellDamage: " << d.region().name(d.sheet());
    if (d.changes() & CellDamage::Appearance)
        str << " Appearance";
    if (d.changes() & CellDamage::Binding)
        str << " Binding";
    if (d.changes() & CellDamage::Formula)
        str << " Formula";
    if (d.changes() & CellDamage::Value)
        str << " Value";
    return str;
}

QDebug operator<<(QDebug str, const Calligra::Sheets::SheetDamage &d)
{
    str << "SheetDamage: " << (d.sheet() ? d.sheet()->sheetName() : "NULL POINTER!");
    switch (d.changes()) {
    case SheetDamage::None:
        return str << " None";
    case SheetDamage::ContentChanged:
        return str << " Content";
    case SheetDamage::PropertiesChanged:
        return str << " Properties";
    case SheetDamage::Hidden:
        return str << " Hidden";
    case SheetDamage::Shown:
        return str << " Shown";
    case SheetDamage::Name:
        return str << "Name";
    case SheetDamage::ColumnsChanged:
        return str << "Columns";
    case SheetDamage::RowsChanged:
        return str << "Rows";
    }
    return str;
}

QDebug operator<<(QDebug str, const Calligra::Sheets::SelectionDamage &d)
{
    str << "SelectionDamage: " << d.region().name();
    return str;
}
