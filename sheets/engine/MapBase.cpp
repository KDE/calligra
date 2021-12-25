/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "MapBase.h"
#include "SheetBase.h"
#include <QObject>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN MapBase::Private
{
public:

    bool isLoading;
    /**
     * List of all sheets in this map.
     */
    QList<SheetBase *> lstSheets;
};





MapBase::MapBase() :
    d(new Private)
{
    d->isLoading = false;
}

MapBase::~MapBase()
{
}



SheetBase *MapBase::sheet(int index) const
{
    return d->lstSheets.value(index);
}

int MapBase::indexOf(SheetBase *sheet) const
{
    return d->lstSheets.indexOf(sheet);
}

QList<SheetBase *>& MapBase::sheetList() const
{
    return d->lstSheets;
}

int MapBase::count() const
{
    return d->lstSheets.count();
}


SheetBase *MapBase::findSheet(const QString & _name) const
{
    for (SheetBase* sheet : d->lstSheets) {
        if (_name.toLower() == sheet->sheetName().toLower())
            return sheet;
    }
    return nullptr;
}

SheetBase *MapBase::nextSheet(SheetBase *currentSheet) const
{
    bool returnNext = false;
    for (SheetBase* sheet : d->lstSheets) {
        if (returnNext) return sheet;
        if (sheet == currentSheet) returnNext = true;
    }
    // If returnNext is set here, it means that currentSheet was last in the list.
    if (returnNext) return currentSheet;
    return nullptr;
}

SheetBase * MapBase::previousSheet(SheetBase *currentSheet) const
{
    SheetBase *prev = nullptr;
    for (SheetBase* sheet : d->lstSheets) {
        if (sheet == currentSheet) {
            if (prev) return prev;
            return currentSheet;  // this means that currentSheet was first in the list
        }
        prev = sheet;
    }
    return nullptr;
}



bool Map::isLoading() const
{
    return d->isLoading;
}

void Map::setLoading(bool l) {
    d->isLoading = l;
}


