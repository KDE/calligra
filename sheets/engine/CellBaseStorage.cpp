/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CellBaseStorage.h"

// Qt
#ifdef CALLIGRA_SHEETS_MT
#include <QReadLocker>
#include <QWriteLocker>
#endif

#include "Damages.h"
#include "Formula.h"
#include "MapBase.h"
#include "SheetBase.h"

#include "FormulaStorage.h"
#include "ValueStorage.h"

using namespace Calligra::Sheets;

typedef RectStorage<QString> NamedAreaStorage;

class Q_DECL_HIDDEN CellBaseStorage::Private
{
public:
    Private(SheetBase* sheet)
            : sheet(sheet)
            , bindingStorage(new BindingStorage(sheet->map()))
            , formulaStorage(new FormulaStorage())
            , namedAreaStorage(new NamedAreaStorage(sheet->map()))
            , userInputStorage(new UserInputStorage())
            , validityStorage(new ValidityStorage(sheet->map()))
            , valueStorage(new ValueStorage())
    {
    }

    Private(const Private& other, SheetBase* sheet)
            : sheet(sheet)
            , bindingStorage(new BindingStorage(*other.bindingStorage))
            , formulaStorage(new FormulaStorage(*other.formulaStorage))
            , namedAreaStorage(new NamedAreaStorage(*other.namedAreaStorage))
            , userInputStorage(new UserInputStorage(*other.userInputStorage))
            , validityStorage(new ValidityStorage(*other.validityStorage))
            , valueStorage(new ValueStorage(*other.valueStorage))
    {
    }

    ~Private() {
        delete bindingStorage;
        delete formulaStorage;
        delete namedAreaStorage;
        delete userInputStorage;
        delete validityStorage;
        delete valueStorage;
    }

    void recalcFormulas(const Region &r, bool includeDeps);
    void recalcFormulas(const Region &r, bool includeDeps);

    SheetBase*              sheet;

    BindingStorage*         bindingStorage;
    FormulaStorage*         formulaStorage;
    NamedAreaStorage*       namedAreaStorage;
    UserInputStorage*       userInputStorage;
    ValidityStorage*        validityStorage;
    ValueStorage*           valueStorage;
};

void CellBaseStorage::Private::recalcFormulas(const Region &r, bool includeDeps)
{
    PointStorage<Formula> subStorage = formulaStorage->subStorage(r);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(sheet, subStorage.col(i), subStorage.row(i));
        sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }

    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = sheet->map()->dependencyManager()->reduceToProvidingRegion(r);
    sheet->map()->addDamage(new CellDamage(sheet, providers, CellDamage::Value));
}

void CellBaseStorage::Private::updateBindings(const Region &r) {
    sheet->map()->addDamage(new CellDamage(sheet, r, CellDamage::Binding | CellDamage::NamedArea));
}



CellBaseStorage::CellBaseStorage(Sheet* sheet)
        : QObject(sheet)
        , d(new Private(sheet))
        , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
        , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillStorages();
}

CellBaseStorage::CellBaseStorage(const CellBaseStorage& other)
        : QObject(other.d->sheet)
        , CellBaseStorageBase(other),
        , d(new Private(*other.d, other.d->sheet))
        , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
        , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillStorages();
}

CellBaseStorage::CellBaseStorage(const CellBaseStorage& other, Sheet* sheet)
        : QObject(sheet)
        , CellBaseStorageBase(other, sheet),
        , d(new Private(*other.d, sheet))
        , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
        , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillStorages();
}

CellBaseStorage::~CellBaseStorage()
{
    storages.clear();
    delete d;
}

void CellBaseStorage::fillStorages() {
    storages.clear();
    storages.push_back (bindingStorage);
    storages.push_back (formulaStorage);
    storages.push_back (namedAreaStorage);
    storages.push_back (userInputStorage);
    storages.push_back (validityStorage);
    storages.push_back (valueStorage);
}


SheetBase* CellBaseStorage::sheet() const
{
    return d->sheet;
}


const BindingStorage* CellStorage::bindingStorage() const
{
    return d->bindingStorage;
}

const FormulaStorage* CellBaseStorage::formulaStorage() const
{
    return d->formulaStorage;
}

const UserInputStorage* CellBaseStorage::userInputStorage() const
{
    return d->userInputStorage;
}

const ValidityStorage* CellStorage::validityStorage() const
{
    return d->validityStorage;
}

const ValueStorage* CellBaseStorage::valueStorage() const
{
    return d->valueStorage;
}



Formula CellBaseStorage::formula(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->formulaStorage->lookup(column, row, Formula::empty());
}

void CellBaseStorage::setFormula(int column, int row, const Formula& formula)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    Formula old = Formula::empty();
    if (formula.expression().isEmpty())
        old = d->formulaStorage->take(column, row, Formula::empty());
    else
        old = d->formulaStorage->insert(column, row, formula);

    // formula changed?
    if (formula != old) {
        if (!d->sheet->map()->isLoading()) {
            // trigger an update of the dependencies and a recalculation
            d->sheet->map()->addDamage(new CellDamage(Cell(d->sheet, column, row), CellDamage::Formula | CellDamage::Value));
        }
    }
}

QString CellBaseStorage::userInput(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->userInputStorage->lookup(column, row);
}

void CellBaseStorage::setUserInput(int column, int row, const QString& userInput)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    if (userInput.isEmpty())
        d->userInputStorage->take(column, row);
    else
        d->userInputStorage->insert(column, row, userInput);
}

Value CellBaseStorage::value(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->valueStorage->lookup(column, row);
}

Value CellBaseStorage::valueRegion(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    // create a subStorage with adjusted origin
    return Value(d->valueStorage->subStorage(region, false), region.boundingRect().size());
}

void CellBaseStorage::setValue(int column, int row, const Value& value)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    // release any lock
    unlockCells(column, row);

    Value old;
    if (value.isEmpty())
        old = d->valueStorage->take(column, row);
    else
        old = d->valueStorage->insert(column, row, value);

    // value changed?
    if (value != old) {
        if (!d->sheet->map()->isLoading()) {
            // Always trigger a repainting and a binding update.
            CellDamage::Changes changes = CellDamage::Appearance | CellDamage::Binding;
            // Trigger a recalculation of the consuming cells, only if we are not
            // already in a recalculation process.
            if (!d->sheet->map()->recalcManager()->isActive())
                changes |= CellDamage::Value;
            d->sheet->map()->addDamage(new CellDamage(Cell(d->sheet, column, row), changes));
            // Also trigger a relayouting of the first non-empty cell to the left of this one
            int prevCol;
            Value v = d->valueStorage->prevInRow(column, row, &prevCol);
            if (!v.isEmpty())
                d->sheet->map()->addDamage(new CellDamage(Cell(d->sheet, prevCol, row), CellDamage::Appearance));
        }
    }
}

Binding CellBaseStorage::binding(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->bindingStorage->contains(QPoint(column, row));
}

void CellBaseStorage::setBinding(const Region& region, const Binding& binding)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->bindingStorage->insert(region, binding);
}

void CellBaseStorage::removeBinding(const Region& region, const Binding& binding)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->bindingStorage->remove(region, binding);
}

Validity CellStorage::validity(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->validityStorage->contains(QPoint(column, row));
}

void CellStorage::setValidity(const Region& region, Validity validity)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->validityStorage->insert(region, validity);
}




void CellBaseStorage::insertColumns(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    // FIXME Stefan: Would it be better to directly alter the dependency tree?
    // TODO Stefan: Optimize: Avoid the double creation of the sub-storages, but don't process
    //              formulas, that will get out of bounds after the operation.
    const Region invalidRegion(QRect(QPoint(position, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    // Trigger an update of the bindings and the named areas.
    d->updateBindings(invalidRegion);

    for (BaseStorage *storage : storages)
        storage->insertColumns(position, number);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::removeColumns(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(QPoint(position, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    const Region region(QRect(QPoint(position - 1, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->updateBindings(region);

    for (BaseStorage *storage : storages)
        storage->removeColumns(position, number);

    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::insertRows(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(QPoint(1, position), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    d->updateBindings(invalidRegion);

    for (BaseStorage *storage : storages)
        storage->insertRows(position, number);

    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::removeRows(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(QPoint(1, position), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    const Region region(QRect(QPoint(1, position - 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->updateBindings(region);

    for (BaseStorage *storage : storages)
        storage->removeRows(position, number);

    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::removeShiftLeft(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    const Region region(QRect(rect.topLeft() - QPoint(1, 0), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->updateBindings(region);

    for (BaseStorage *storage : storages)
        storage->removeShiftLeft(rect);

    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::insertShiftRight(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    d->updateBindings(invalidRegion);

    for (BaseStorage *storage : storages)
        storage->insertShiftRight(rect);

    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::removeShiftUp(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    const Region region(QRect(rect.topLeft() - QPoint(0, 1), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->updateBindings(region);

    for (BaseStorage *storage : storages)
        storage->removeShiftUp(rect);

    d->recalcFormulas(invalidRegion, true);
}

void CellBaseStorage::insertShiftDown(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion, false);
    d->updateBindings(region);

    for (BaseStorage *storage : storages)
        storage->insertShiftDown(rect);

    d->recalcFormulas(invalidRegion, true);
}




