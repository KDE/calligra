/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetBase.h"
#include "MapBase.h"
#include "CellBaseStorage.h"
#include "Damages.h"
#include "FormulaStorage.h"

#include <QObject>


using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SheetBase::Private
{
public:
    Private(SheetBase *sheet);
    ~Private() {}

    MapBase *workbook;
    QString name;

    SheetBase *m_sheet;
    CellBaseStorage *cellStorage;
};


SheetBase::Private::Private (SheetBase *sheet)
    : m_sheet(sheet)
{
    cellStorage = nullptr;
}


SheetBase::SheetBase(MapBase* map, const QString &sheetName) :
    d(new Private(this))
{
    d->workbook = map;
    d->name = sheetName;
    d->cellStorage = new CellBaseStorage(this);
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
    delete d->cellStorage;
    delete d;
}

void SheetBase::setCellStorage(CellBaseStorage *storage)
{
    delete d->cellStorage;
    d->cellStorage = storage;
}

CellBaseStorage* SheetBase::cellStorage() const {
    return d->cellStorage;
}


const FormulaStorage* SheetBase::formulaStorage() const
{
    return d->cellStorage->formulaStorage();
}

const ValidityStorage* SheetBase::validityStorage() const
{
    return d->cellStorage->validityStorage();
}

const ValueStorage* SheetBase::valueStorage() const
{
    return d->cellStorage->valueStorage();
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

void SheetBase::changeCellTabName(QString const & old_name, QString const & new_name)
{
    const FormulaStorage *fs = formulaStorage();
    for (int c = 0; c < fs->count(); ++c) {
        if (fs->data(c).expression().contains(old_name)) {
            int nb = fs->data(c).expression().count(old_name + '!');
            QString tmp = old_name + '!';
            int len = tmp.length();
            tmp = fs->data(c).expression();

            for (int i = 0; i < nb; ++i) {
                int pos = tmp.indexOf(old_name + '!');
                tmp.replace(pos, len, new_name + '!');
            }
            CellBase cell(this, fs->col(c), fs->row(c));
            Formula formula(this, cell);
            formula.setExpression(tmp);
            cell.setFormula(formula);
        }
    }
}






