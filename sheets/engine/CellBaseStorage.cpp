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

#include "CellBase.h"
#include "Formula.h"
#include "SheetBase.h"

#include "FormulaStorage.h"
#include "RectStorage.h"
#include "ValidityStorage.h"
#include "ValueStorage.h"

#include "Damages.h"
#include "DependencyManager.h"
#include "RecalcManager.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN CellBaseStorage::Private : public QSharedData
{
public:
    Private(SheetBase *sheet)
        : sheet(sheet)
        , commentStorage(new CommentStorage())
        , formulaStorage(new FormulaStorage())
        , matrixStorage(new MatrixStorage(sheet->map()))
        , namedAreaStorage(new NamedAreaStorage(sheet->map()))
        , userInputStorage(new UserInputStorage())
        , validityStorage(new ValidityStorage(sheet->map()))
        , valueStorage(new ValueStorage())
    {
    }

    Private(const Private &other, SheetBase *sheet)
        : sheet(sheet)
        , commentStorage(new CommentStorage(*other.commentStorage))
        , formulaStorage(new FormulaStorage(*other.formulaStorage))
        , matrixStorage(new MatrixStorage(*other.matrixStorage))
        , namedAreaStorage(new NamedAreaStorage(*other.namedAreaStorage))
        , userInputStorage(new UserInputStorage(*other.userInputStorage))
        , validityStorage(new ValidityStorage(*other.validityStorage))
        , valueStorage(new ValueStorage(*other.valueStorage))
    {
    }

    ~Private()
    {
        delete commentStorage;
        delete formulaStorage;
        delete matrixStorage;
        delete namedAreaStorage;
        delete userInputStorage;
        delete validityStorage;
        delete valueStorage;
    }

    void recalcFormulas(const Region &r);
    void updateBindings(const Region &r);

    SheetBase *sheet;

    CommentStorage *commentStorage;
    FormulaStorage *formulaStorage;
    MatrixStorage *matrixStorage;
    NamedAreaStorage *namedAreaStorage;
    UserInputStorage *userInputStorage;
    ValidityStorage *validityStorage;
    ValueStorage *valueStorage;
};

void CellBaseStorage::Private::recalcFormulas(const Region &r)
{
    PointStorage<Formula> subStorage = formulaStorage->subStorage(r);
    CellBase cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        CellBase cell(sheet, subStorage.col(i), subStorage.row(i));
        sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }

    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = sheet->map()->dependencyManager()->reduceToProvidingRegion(r);
    sheet->map()->addDamage(new CellDamage(sheet, providers, CellDamage::Value));
}

void CellBaseStorage::Private::updateBindings(const Region &r)
{
    sheet->map()->addDamage(new CellDamage(sheet, r, CellDamage::Binding | CellDamage::NamedArea));
}

CellBaseStorage::CellBaseStorage(SheetBase *sheet)
    : d(new Private(sheet))
#ifdef CALLIGRA_SHEETS_MT
    , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillStorages();
}

CellBaseStorage::CellBaseStorage(const CellBaseStorage &other)
    : d(new Private(*other.d, other.d->sheet))
#ifdef CALLIGRA_SHEETS_MT
    , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillStorages();
}

CellBaseStorage::CellBaseStorage(const CellBaseStorage &other, SheetBase *sheet)
    : d(new Private(*other.d, sheet))
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

void CellBaseStorage::fillStorages()
{
    storages.clear();
    storages.push_back(d->commentStorage);
    storages.push_back(d->formulaStorage);
    storages.push_back(d->matrixStorage);
    storages.push_back(d->namedAreaStorage);
    storages.push_back(d->userInputStorage);
    storages.push_back(d->validityStorage);
    storages.push_back(d->valueStorage);
}

SheetBase *CellBaseStorage::sheet() const
{
    return d->sheet;
}

CommentStorage *CellBaseStorage::commentStorage() const
{
    return d->commentStorage;
}

FormulaStorage *CellBaseStorage::formulaStorage() const
{
    return d->formulaStorage;
}

MatrixStorage *CellBaseStorage::matrixStorage() const
{
    return d->matrixStorage;
}

NamedAreaStorage *CellBaseStorage::namedAreaStorage() const
{
    return d->namedAreaStorage;
}

UserInputStorage *CellBaseStorage::userInputStorage() const
{
    return d->userInputStorage;
}

ValidityStorage *CellBaseStorage::validityStorage() const
{
    return d->validityStorage;
}

ValueStorage *CellBaseStorage::valueStorage() const
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

void CellBaseStorage::setFormula(int column, int row, const Formula &formula)
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
            CellBase cell(d->sheet, column, row);
            // trigger an update of the dependencies and a recalculation
            d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula | CellDamage::Value));
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

void CellBaseStorage::setUserInput(int column, int row, const QString &userInput)
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

Value CellBaseStorage::valueRegion(const Region &region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    // create a subStorage with adjusted origin
    return Value(d->valueStorage->subStorage(region, false), region.boundingRect().size());
}

void CellBaseStorage::setValue(int column, int row, const Value &value)
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
            d->sheet->map()->addDamage(new CellDamage(CellBase(d->sheet, column, row), changes));
            // Also trigger a relayouting of the first non-empty cell to the left of this one
            int prevCol;
            Value v = d->valueStorage->prevInRow(column, row, &prevCol);
            if (!v.isEmpty())
                d->sheet->map()->addDamage(new CellDamage(CellBase(d->sheet, prevCol, row), CellDamage::Appearance));
        }
    }
}

QString CellBaseStorage::comment(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->commentStorage->lookup(column, row);
}

void CellBaseStorage::setComment(int column, int row, const QString &comment)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    QString old;
    if (comment.isEmpty())
        d->commentStorage->take(column, row);
    else
        d->commentStorage->insert(column, row, comment);
}

Validity CellBaseStorage::validity(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->validityStorage->contains(QPoint(column, row));
}

void CellBaseStorage::setValidity(const Region &region, Validity validity)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->validityStorage->insert(region, validity);
}

QString CellBaseStorage::namedArea(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    QPair<QRectF, QString> pair = d->namedAreaStorage->containedPair(QPoint(column, row));
    if (pair.first.isEmpty())
        return QString();
    if (pair.second.isEmpty())
        return QString();
    return pair.second;
}

QVector<QPair<QRectF, QString>> CellBaseStorage::namedAreas(const Region &region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->namedAreaStorage->intersectingPairs(region);
}

void CellBaseStorage::setNamedArea(const Region &region, const QString &namedArea)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->namedAreaStorage->insert(region, namedArea);
}

void CellBaseStorage::removeNamedArea(const Region &region, const QString &namedArea)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->namedAreaStorage->remove(region, namedArea);
}

bool CellBaseStorage::locksCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return false;
    if (pair.second == false)
        return false;
    // master cell?
    if (pair.first.toRect().topLeft() != QPoint(column, row))
        return false;
    return true;
}

bool CellBaseStorage::isLocked(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return false;
    if (pair.second == false)
        return false;
    // master cell?
    if (pair.first.toRect().topLeft() == QPoint(column, row))
        return false;
    return true;
}

bool CellBaseStorage::hasLockedCells(const Region &region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    typedef QPair<QRectF, bool> RectBoolPair;
    QVector<QPair<QRectF, bool>> pairs = d->matrixStorage->intersectingPairs(region);
    for (const RectBoolPair &pair : pairs) {
        if (pair.first.isNull())
            continue;
        if (pair.second == false)
            continue;
        // more than just the master cell in the region?
        const QPoint topLeft = pair.first.toRect().topLeft();
        if (pair.first.width() >= 1) {
            if (region.contains(topLeft + QPoint(1, 0), d->sheet))
                return true;
        }
        if (pair.first.height() >= 1) {
            if (region.contains(topLeft + QPoint(0, 1), d->sheet))
                return true;
        }
    }
    return false;
}

void CellBaseStorage::lockCells(const QRect &rect)
{
    // Start by unlocking the cells that we lock right now
    unlockCells(rect);

#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(rect.topLeft()); // FIXME
    if (!pair.first.isNull())
        d->matrixStorage->insert(Region(pair.first.toRect()), false);
    // Lock the cells
    if (rect.width() > 1 || rect.height() > 1)
        d->matrixStorage->insert(Region(rect), true);
}

void CellBaseStorage::unlockCells(int column, int row)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return;
    if (pair.second == false)
        return;
    const QRect rect = pair.first.toRect();
    d->matrixStorage->insert(Region(rect, d->sheet), false);
}

void CellBaseStorage::unlockCells(const QRect &rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    QRect validRect = rect.intersected(d->matrixStorage->usedArea());
    if (validRect.isEmpty())
        return;
    d->matrixStorage->insert(Region(validRect, d->sheet), false);
}

QRect CellBaseStorage::lockedCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return QRect(column, row, 1, 1);
    if (pair.second == false)
        return QRect(column, row, 1, 1);
    if (pair.first.toRect().topLeft() != QPoint(column, row))
        return QRect(column, row, 1, 1);
    return pair.first.toRect();
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
    d->recalcFormulas(invalidRegion);
    // Trigger an update of the bindings and the named areas.
    d->updateBindings(invalidRegion);

    for (StorageBase *storage : storages)
        storage->insertColumns(position, number);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::removeColumns(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(QPoint(position, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion);
    const Region region(QRect(QPoint(position - 1, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->updateBindings(region);

    for (StorageBase *storage : storages)
        storage->removeColumns(position, number);

    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::insertRows(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(QPoint(1, position), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion);
    d->updateBindings(invalidRegion);

    for (StorageBase *storage : storages)
        storage->insertRows(position, number);

    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::removeRows(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(QPoint(1, position), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion);
    const Region region(QRect(QPoint(1, position - 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->updateBindings(region);

    for (StorageBase *storage : storages)
        storage->removeRows(position, number);

    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::removeShiftLeft(const QRect &rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->recalcFormulas(invalidRegion);
    const Region region(QRect(rect.topLeft() - QPoint(1, 0), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->updateBindings(region);

    for (StorageBase *storage : storages)
        storage->removeShiftLeft(rect);

    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::insertShiftRight(const QRect &rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->recalcFormulas(invalidRegion);
    d->updateBindings(invalidRegion);

    for (StorageBase *storage : storages)
        storage->insertShiftRight(rect);

    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::removeShiftUp(const QRect &rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion);
    const Region region(QRect(rect.topLeft() - QPoint(0, 1), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->updateBindings(region);

    for (StorageBase *storage : storages)
        storage->removeShiftUp(rect);

    d->recalcFormulas(invalidRegion);
}

void CellBaseStorage::insertShiftDown(const QRect &rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->recalcFormulas(invalidRegion);
    d->updateBindings(invalidRegion);

    for (StorageBase *storage : storages)
        storage->insertShiftDown(rect);

    d->recalcFormulas(invalidRegion);
}

int CellBaseStorage::columns(bool /*includeStyles*/) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    int max = 0;
    max = qMax(max, d->commentStorage->columns());
    max = qMax(max, d->validityStorage->usedArea().right());
    max = qMax(max, d->formulaStorage->columns());
    max = qMax(max, d->valueStorage->columns());

    return max;
}

int CellBaseStorage::rows(bool /*includeStyles*/) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    int max = 0;
    max = qMax(max, d->commentStorage->rows());
    max = qMax(max, d->validityStorage->usedArea().bottom());
    max = qMax(max, d->formulaStorage->rows());
    max = qMax(max, d->valueStorage->rows());

    return max;
}

QRect CellBaseStorage::trimToUsedArea(const QRect &rect) const
{
    int c = columns();
    int r = rows();
    if ((rect.right() <= c) && (rect.bottom() <= r))
        return rect; // all good
    return rect.intersected(QRect(1, 1, c, r));
}
