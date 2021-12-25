/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetBase.h"
#include "MapBase.h"
#include "Damages.h"

#include <QObject>


using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SheetBase::Private
{
public:
    Private(SheetBase *sheet);
    ~Private() {}

    MapBase *workbook;
    QString name;
private:
    SheetBase *m_sheet;
};


SheetBase::Private::Private (SheetBase *sheet)
    : m_sheet(sheet)
{
}


SheetBase::SheetBase(MapBase* map, const QString &sheetName) :
    d(new Private(this))
{
    d->workbook = map;
    d->name = sheetName;
}

SheetBase::SheetBase(const SheetBase &other)
        : d(new Private(this))
{
    d->workbook = other.d->workbook;

    // create a unique name
    int i = 1;
    do
        d->name = other.d->name + QString("_%1").arg(i++);
    while (d->workbook->findSheet(d->name));
}

SheetBase::~SheetBase()
{
    delete d;
}

MapBase* SheetBase::map() const
{
    return d->workbook;
}

QString SheetBase::sheetName() const
{
    return d->name;
}

bool SheetBase::setSheetName(const QString& name)
{
    if (map()->findSheet(name))
        return false;

    QString old_name = d->name;
    if (old_name == name)
        return true;

    d->name = name;

    for (SheetBase *sheet : map()->sheetList())
        sheet->changeCellTabName(old_name, name);

    map()->addDamage(new SheetDamage(this, SheetDamage::Name));

    return true;
}





