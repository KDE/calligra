/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "MapBase.h"
#include "Damages.h"
#include "Region.h"
#include "SheetBase.h"

#include "BindingManager.h"
#include "DependencyManager.h"
#include "NamedAreaManager.h"
#include "RecalcManager.h"

#include "CalculationSettings.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueParser.h"

#include <QTimer>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN MapBase::Private
{
public:

    bool isLoading;
    /**
     * List of all sheets in this map.
     */
    QList<SheetBase *> lstSheets;

    CalculationSettings* calculationSettings;
    ValueCalc* calc;
    ValueConverter* converter;
    ValueParser* parser;

    BindingManager* bindingManager;
    DependencyManager* dependencyManager;
    NamedAreaManager* namedAreaManager;
    RecalcManager* recalcManager;

    QList<Damage*> damages;
};





MapBase::MapBase() :
    QObject()
    , d(new Private)
{
    d->isLoading = false;

    d->bindingManager = new BindingManager(this);
    d->dependencyManager = new DependencyManager(this);
    d->namedAreaManager = new NamedAreaManager(this);
    d->recalcManager = new RecalcManager(this);
    d->calculationSettings = new CalculationSettings();

    d->parser = new ValueParser(d->calculationSettings);
    d->converter = new ValueConverter(d->parser);
    d->calc = new ValueCalc(d->converter);

    connect(this, SIGNAL(damagesFlushed()), this, SLOT(handleDamages()));

    connect(this, SIGNAL(sheetAdded(sheet)), d->dependencyManager, SLOT(addSheet(sheet)));
    connect(this, SIGNAL(sheetAdded(sheet)), d->recalcManager, SLOT(addSheet(sheet)));
    connect(this, SIGNAL(sheetRemoved(sheet)), d->dependencyManager, SLOT(removeSheet(sheet)));
    connect(this, SIGNAL(sheetRemoved(sheet)), d->recalcManager, SLOT(removeSheet(sheet)));
    connect(this, SIGNAL(sheetRevived(sheet)), d->dependencyManager, SLOT(addSheet(sheet)));
    connect(this, SIGNAL(sheetRevived(sheet)), d->recalcManager, SLOT(addSheet(sheet)));

    connect(d->namedAreaManager, &NamedAreaManager::namedAreaModified,
            d->dependencyManager, &DependencyManager::namedAreaModified);

}

MapBase::~MapBase()
{
    delete d->bindingManager;
    delete d->dependencyManager;
    delete d->namedAreaManager;
    delete d->recalcManager;

    delete d->parser;
    delete d->converter;
    delete d->calc;
    delete d->calculationSettings;
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

void MapBase::addSheet(SheetBase *_sheet)
{
    d->lstSheets.append(_sheet);
    emit sheetAdded(_sheet);
}



ValueParser* MapBase::parser() const
{
    return d->parser;
}

ValueConverter* MapBase::converter() const
{
    return d->converter;
}

ValueCalc* MapBase::calc() const
{
    return d->calc;
}


BindingManager* MapBase::bindingManager() const
{
    return d->bindingManager;
}

DependencyManager* MapBase::dependencyManager() const
{
    return d->dependencyManager;
}

NamedAreaManager* MapBase::namedAreaManager() const
{
    return d->namedAreaManager;
}

RecalcManager* MapBase::recalcManager() const
{
    return d->recalcManager;
}

CalculationSettings* MapBase::calculationSettings() const
{
    return d->calculationSettings;
}



void MapBase::addDamage(Damage* damage)
{
    // Do not create a new Damage, if we are in loading process. Check for it before
    // calling this function. This prevents unnecessary memory allocations (new).
    // see FIXME in Sheet::setSheetName().
//     Q_ASSERT(!isLoading());
    Q_CHECK_PTR(damage);

#ifndef NDEBUG
    if (damage->type() == Damage::Cell) {
        debugSheetsDamage << "Adding\t" << *static_cast<CellDamage*>(damage);
    } else if (damage->type() == Damage::Sheet) {
        debugSheetsDamage << "Adding\t" << *static_cast<SheetDamage*>(damage);
    } else if (damage->type() == Damage::Selection) {
        debugSheetsDamage << "Adding\t" << *static_cast<SelectionDamage*>(damage);
    } else {
        debugSheetsDamage << "Adding\t" << *damage;
    }
#endif

    d->damages.append(damage);

    if (d->damages.count() == 1) {
        QTimer::singleShot(0, this, SLOT(flushDamages()));
    }
}

void MapBase::flushDamages()
{
    // Copy the damages to process. This allows new damages while processing.
    QList<Damage*> damages = d->damages;
    d->damages.clear();
    emit damagesFlushed(damages);
    qDeleteAll(damages);
}

void MapBase::handleDamages(const QList<Damage*>& damages)
{
    Region bindingChangedRegion;
    Region formulaChangedRegion;
    Region namedAreaChangedRegion;
    Region valueChangedRegion;
    WorkbookDamage::Changes workbookChanges = WorkbookDamage::None;
    bool allValues = false, allFormulas = false;   // These are for improved code readability

    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *cellDamage;
            SheetBase* const damagedSheet = cellDamage->sheet();
            const Region& region = cellDamage->region();
            const CellDamage::Changes changes = cellDamage->changes();

            if (!allValues) {
                if (changes & CellDamage::Binding)
                    bindingChangedRegion.add(region, damagedSheet);
                if (changes & CellDamage::Value)
                    valueChangedRegion.add(region, damagedSheet);
            }
            if (!allFormulas) {
                if (changes & CellDamage::Formula)
                    formulaChangedRegion.add(region, damagedSheet);
                if (changes & CellDamage::NamedArea)
                    namedAreaChangedRegion.add(region, damagedSheet);
            }
            continue;
        }

        if (damage->type() == Damage::Sheet) {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *sheetDamage;
//             SheetBase* damagedSheet = sheetDamage->sheet();

            if (sheetDamage->changes() & SheetDamage::PropertiesChanged) {
            }
            continue;
        }

        if (damage->type() == Damage::Workbook) {
            WorkbookDamage* workbookDamage = static_cast<WorkbookDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *damage;
            const WorkbookDamage::Changes changes = workbookDamage->changes();

            workbookChanges |= changes;
            if (changes & WorkbookDamage::Formula)
                allFormulas = true;
            if (changes & WorkbookDamage::Value)
                allValues = true;
            continue;
        }
//         debugSheetsDamage <<"Unhandled\t" << *damage;
    }

    if (allFormulas) {
        d->namedAreaManager->updateAllNamedAreas();
        d->dependencyManager->updateAllDependencies(this);
    } else {
        if (!namedAreaChangedRegion.isEmpty())
            d->namedAreaManager->regionChanged(namedAreaChangedRegion);
        if (!formulaChangedRegion.isEmpty())
            d->dependencyManager->regionChanged(formulaChangedRegion);
    }

    if (allValues) {
        d->recalcManager->recalcMap();
        d->bindingManager->updateAllBindings();
    } else {
        if (!valueChangedRegion.isEmpty())
            d->recalcManager->regionChanged(valueChangedRegion);
        if (!bindingChangedRegion.isEmpty())
            d->bindingManager->regionChanged(bindingChangedRegion);
    }
}



bool MapBase::isLoading() const
{
    return d->isLoading;
}

void MapBase::setLoading(bool l) {
    d->isLoading = l;
}


