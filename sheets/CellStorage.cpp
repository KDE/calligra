/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CellStorage.h"

// Qt
#ifdef CALLIGRA_SHEETS_MT
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#endif

// Sheets - storages
#include "BindingStorage.h"
#include "ConditionsStorage.h"
#include "FormulaStorage.h"
#include "RectStorage.h"
#include "StyleStorage.h"
#include "ValidityStorage.h"
#include "ValueStorage.h"

// Sheets - rest
#include "Damages.h"
#include "DependencyManager.h"
#include "Map.h"
#include "ModelSupport.h"
#include "RecalcManager.h"
#include "Sheet.h"

// commands
#include "commands/PointStorageUndoCommand.h"
#include "commands/RectStorageUndoCommand.h"
#include "commands/StyleStorageUndoCommand.h"

// database
#include "database/DatabaseStorage.h"
#include "database/DatabaseManager.h"

Q_DECLARE_METATYPE(QSharedPointer<QTextDocument>)

using namespace Calligra::Sheets;

typedef RectStorage<QString> NamedAreaStorage;

class Q_DECL_HIDDEN CellStorage::Private
{
public:
    Private(Sheet* sheet)
            : sheet(sheet)
            , bindingStorage(new BindingStorage(sheet->map()))
            , commentStorage(new CommentStorage(sheet->map()))
            , conditionsStorage(new ConditionsStorage(sheet->map()))
            , databaseStorage(new DatabaseStorage(sheet->map()))
            , formulaStorage(new FormulaStorage())
            , fusionStorage(new FusionStorage(sheet->map()))
            , linkStorage(new LinkStorage())
            , matrixStorage(new MatrixStorage(sheet->map()))
            , namedAreaStorage(new NamedAreaStorage(sheet->map()))
            , styleStorage(new StyleStorage(sheet->map()))
            , userInputStorage(new UserInputStorage())
            , validityStorage(new ValidityStorage(sheet->map()))
            , valueStorage(new ValueStorage())
            , richTextStorage(new RichTextStorage())
            , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
            , bigUglyLock(QReadWriteLock::Recursive)
#endif
    {}

    Private(const Private& other, Sheet* sheet)
            : sheet(sheet)
            , bindingStorage(new BindingStorage(*other.bindingStorage))
            , commentStorage(new CommentStorage(*other.commentStorage))
            , conditionsStorage(new ConditionsStorage(*other.conditionsStorage))
            , databaseStorage(new DatabaseStorage(*other.databaseStorage))
            , formulaStorage(new FormulaStorage(*other.formulaStorage))
            , fusionStorage(new FusionStorage(*other.fusionStorage))
            , linkStorage(new LinkStorage(*other.linkStorage))
            , matrixStorage(new MatrixStorage(*other.matrixStorage))
            , namedAreaStorage(new NamedAreaStorage(*other.namedAreaStorage))
            , styleStorage(new StyleStorage(*other.styleStorage))
            , userInputStorage(new UserInputStorage(*other.userInputStorage))
            , validityStorage(new ValidityStorage(*other.validityStorage))
            , valueStorage(new ValueStorage(*other.valueStorage))
            , richTextStorage(new RichTextStorage(*other.richTextStorage))
            , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
            , bigUglyLock(QReadWriteLock::Recursive)
#endif
    {}

    ~Private() {
        delete bindingStorage;
        delete commentStorage;
        delete conditionsStorage;
        delete databaseStorage;
        delete formulaStorage;
        delete fusionStorage;
        delete linkStorage;
        delete matrixStorage;
        delete namedAreaStorage;
        delete styleStorage;
        delete userInputStorage;
        delete validityStorage;
        delete valueStorage;
        delete richTextStorage;
    }

    void createCommand(KUndo2Command *parent) const;

    Sheet*                  sheet;
    BindingStorage*         bindingStorage;
    CommentStorage*         commentStorage;
    ConditionsStorage*      conditionsStorage;
    DatabaseStorage*        databaseStorage;
    FormulaStorage*         formulaStorage;
    FusionStorage*          fusionStorage;
    LinkStorage*            linkStorage;
    MatrixStorage*          matrixStorage;
    NamedAreaStorage*       namedAreaStorage;
    StyleStorage*           styleStorage;
    UserInputStorage*       userInputStorage;
    ValidityStorage*        validityStorage;
    ValueStorage*           valueStorage;
    RichTextStorage*        richTextStorage;

    bool undoEnabled;

#ifdef CALLIGRA_SHEETS_MT
    QReadWriteLock bigUglyLock;
#endif
};

void CellStorage::Private::createCommand(KUndo2Command *parent) const
{
    if (!bindingStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<Binding> *const command
        = new RectStorageUndoCommand<Binding>(sheet->model(), SourceRangeRole, parent);
        command->add(bindingStorage->undoData());
    }
    if (!commentStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<QString> *const command
        = new RectStorageUndoCommand<QString>(sheet->model(), CommentRole, parent);
        command->add(commentStorage->undoData());
    }
    if (!conditionsStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<Conditions> *const command
        = new RectStorageUndoCommand<Conditions>(sheet->model(), ConditionRole, parent);
        command->add(conditionsStorage->undoData());
    }
    if (!databaseStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<Database> *const command
        = new RectStorageUndoCommand<Database>(sheet->model(), TargetRangeRole, parent);
        command->add(databaseStorage->undoData());
    }
    if (!formulaStorage->undoData().isEmpty()) {
        PointStorageUndoCommand<Formula> *const command
        = new PointStorageUndoCommand<Formula>(sheet->model(), FormulaRole, parent);
        command->add(formulaStorage->undoData());
    }
    if (!fusionStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<bool> *const command
        = new RectStorageUndoCommand<bool>(sheet->model(), FusionedRangeRole, parent);
        command->add(fusionStorage->undoData());
    }
    if (!linkStorage->undoData().isEmpty()) {
        PointStorageUndoCommand<QString> *const command
        = new PointStorageUndoCommand<QString>(sheet->model(), LinkRole, parent);
        command->add(linkStorage->undoData());
    }
    if (!matrixStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<bool> *const command
        = new RectStorageUndoCommand<bool>(sheet->model(), LockedRangeRole, parent);
        command->add(matrixStorage->undoData());
    }
    if (!namedAreaStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<QString> *const command
        = new RectStorageUndoCommand<QString>(sheet->model(), NamedAreaRole, parent);
        command->add(namedAreaStorage->undoData());
    }
    if (!richTextStorage->undoData().isEmpty()) {
        PointStorageUndoCommand<QSharedPointer<QTextDocument> > *const command
        = new PointStorageUndoCommand<QSharedPointer<QTextDocument> >(sheet->model(), RichTextRole, parent);
        command->add(richTextStorage->undoData());
    }
    if (!styleStorage->undoData().isEmpty()) {
        StyleStorageUndoCommand *const command
        = new StyleStorageUndoCommand(styleStorage, parent);
        command->add(styleStorage->undoData());
    }
    if (!userInputStorage->undoData().isEmpty()) {
        PointStorageUndoCommand<QString> *const command
        = new PointStorageUndoCommand<QString>(sheet->model(), UserInputRole, parent);
        command->add(userInputStorage->undoData());
    }
    if (!validityStorage->undoData().isEmpty()) {
        RectStorageUndoCommand<Validity> *const command
        = new RectStorageUndoCommand<Validity>(sheet->model(), ValidityRole, parent);
        command->add(validityStorage->undoData());
    }
    if (!valueStorage->undoData().isEmpty()) {
        PointStorageUndoCommand<Value> *const command
        = new PointStorageUndoCommand<Value>(sheet->model(), ValueRole, parent);
        command->add(valueStorage->undoData());
    }
}


CellStorage::CellStorage(Sheet* sheet)
        : QObject(sheet)
        , d(new Private(sheet))
{
}

CellStorage::CellStorage(const CellStorage& other)
        : QObject(other.d->sheet)
        , d(new Private(*other.d, other.d->sheet))
{
}

CellStorage::CellStorage(const CellStorage& other, Sheet* sheet)
        : QObject(sheet)
        , d(new Private(*other.d, sheet))
{
}

CellStorage::~CellStorage()
{
    delete d;
}

Sheet* CellStorage::sheet() const
{
    return d->sheet;
}

void CellStorage::take(int col, int row)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif

    d->formulaStorage->take(col, row);
    d->linkStorage->take(col, row);
    d->userInputStorage->take(col, row);
    d->valueStorage->take(col, row);
    d->richTextStorage->take(col, row);

    if (!d->sheet->map()->isLoading()) {
        // Trigger a recalculation of the consuming cells.
        CellDamage::Changes changes = CellDamage:: Binding | CellDamage::Formula | CellDamage::Value;
        d->sheet->map()->addDamage(new CellDamage(Cell(d->sheet, col, row), changes));
    }
    // also trigger a relayout of the first non-empty cell to the left of this cell
    int prevCol;
    Value v = d->valueStorage->prevInRow(col, row, &prevCol);
    if (!v.isEmpty())
        d->sheet->map()->addDamage(new CellDamage(Cell(d->sheet, prevCol, row), CellDamage::Appearance));

}

Binding CellStorage::binding(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->bindingStorage->contains(QPoint(column, row));
}

void CellStorage::setBinding(const Region& region, const Binding& binding)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->bindingStorage->insert(region, binding);
}

void CellStorage::removeBinding(const Region& region, const Binding& binding)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->bindingStorage->remove(region, binding);
}

QString CellStorage::comment(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->commentStorage->contains(QPoint(column, row));
}

void CellStorage::setComment(const Region& region, const QString& comment)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->commentStorage->insert(region, comment);
}

Conditions CellStorage::conditions(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->conditionsStorage->contains(QPoint(column, row));
}

void CellStorage::setConditions(const Region& region, Conditions conditions)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->conditionsStorage->insert(region, conditions);
}

Database CellStorage::database(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    QPair<QRectF, Database> pair = d->databaseStorage->containedPair(QPoint(column, row));
    if (pair.first.isEmpty())
        return Database();
    if (pair.second.isEmpty())
        return Database();
    // update the range, which might get changed
    Database database = pair.second;
    database.setRange(Region(pair.first.toRect(), d->sheet));
    return database;
}

QList< QPair<QRectF, Database> > CellStorage::databases(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->databaseStorage->intersectingPairs(region);
}

void CellStorage::setDatabase(const Region& region, const Database& database)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->databaseStorage->insert(region, database);
}

Formula CellStorage::formula(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->formulaStorage->lookup(column, row, Formula::empty());
}

void CellStorage::setFormula(int column, int row, const Formula& formula)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
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

QString CellStorage::link(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->linkStorage->lookup(column, row);
}

void CellStorage::setLink(int column, int row, const QString& link)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    if (link.isEmpty())
        d->linkStorage->take(column, row);
    else
        d->linkStorage->insert(column, row, link);
}

QString CellStorage::namedArea(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    QPair<QRectF, QString> pair = d->namedAreaStorage->containedPair(QPoint(column, row));
    if (pair.first.isEmpty())
        return QString();
    if (pair.second.isEmpty())
        return QString();
    return pair.second;
}

QList< QPair<QRectF, QString> > CellStorage::namedAreas(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->namedAreaStorage->intersectingPairs(region);
}

void CellStorage::setNamedArea(const Region& region, const QString& namedArea)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->namedAreaStorage->insert(region, namedArea);
}

void CellStorage::removeNamedArea(const Region& region, const QString& namedArea)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->namedAreaStorage->remove(region, namedArea);
}


void CellStorage::emitInsertNamedArea(const Region &region, const QString &namedArea)
{
    emit insertNamedArea(region, namedArea);
}

Style CellStorage::style(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->styleStorage->contains(QPoint(column, row));
}

Style CellStorage::style(const QRect& rect) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->styleStorage->contains(rect);
}

void CellStorage::setStyle(const Region& region, const Style& style)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->styleStorage->insert(region, style);
}

void CellStorage::insertSubStyle(const QRect &rect, const SharedSubStyle &subStyle)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->styleStorage->insert(rect, subStyle);
}

QString CellStorage::userInput(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->userInputStorage->lookup(column, row);
}

void CellStorage::setUserInput(int column, int row, const QString& userInput)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    if (userInput.isEmpty())
        d->userInputStorage->take(column, row);
    else
        d->userInputStorage->insert(column, row, userInput);
}

QSharedPointer<QTextDocument> CellStorage::richText(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->richTextStorage->lookup(column, row);
}

void CellStorage::setRichText(int column, int row, QSharedPointer<QTextDocument> text)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    if (text.isNull())
        d->richTextStorage->take(column, row);
    else
        d->richTextStorage->insert(column, row, text);
}

Validity CellStorage::validity(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->validityStorage->contains(QPoint(column, row));
}

void CellStorage::setValidity(const Region& region, Validity validity)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->validityStorage->insert(region, validity);
}

Value CellStorage::value(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    return d->valueStorage->lookup(column, row);
}

Value CellStorage::valueRegion(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    // create a subStorage with adjusted origin
    return Value(d->valueStorage->subStorage(region, false), region.boundingRect().size());
}

void CellStorage::setValue(int column, int row, const Value& value)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
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

bool CellStorage::doesMergeCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->fusionStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return false;
    if (pair.second == false)
        return false;
    // master cell?
    if (pair.first.toRect().topLeft() != QPoint(column, row))
        return false;
    return true;
}

bool CellStorage::isPartOfMerged(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->fusionStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return false;
    if (pair.second == false)
        return false;
    // master cell?
    if (pair.first.toRect().topLeft() == QPoint(column, row))
        return false;
    return true;
}

void CellStorage::mergeCells(int column, int row, int numXCells, int numYCells)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Start by unmerging the cells that we merge right now
    const QPair<QRectF, bool> pair = d->fusionStorage->containedPair(QPoint(column, row));
    if (!pair.first.isNull())
        d->fusionStorage->insert(Region(pair.first.toRect()), false);
    // Merge the cells
    if (numXCells != 0 || numYCells != 0)
        d->fusionStorage->insert(Region(column, row, numXCells + 1, numYCells + 1), true);
}

Cell CellStorage::masterCell(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->fusionStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return Cell(d->sheet, column, row);
    if (pair.second == false)
        return Cell(d->sheet, column, row);
    return Cell(d->sheet, pair.first.toRect().topLeft());
}

int CellStorage::mergedXCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->fusionStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return 0;
    // Not the master cell?
    if (pair.first.topLeft() != QPoint(column, row))
        return 0;
    return pair.first.toRect().width() - 1;
}

int CellStorage::mergedYCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->fusionStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return 0;
    // Not the master cell?
    if (pair.first.topLeft() != QPoint(column, row))
        return 0;
    return pair.first.toRect().height() - 1;
}

QList<Cell> CellStorage::masterCells(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    const QList<QPair<QRectF, bool> > pairs = d->fusionStorage->intersectingPairs(region);
    if (pairs.isEmpty())
        return QList<Cell>();
    QList<Cell> masterCells;
    for (int i = 0; i < pairs.count(); ++i) {
        if (pairs[i].first.isNull())
            continue;
        if (pairs[i].second == false)
            continue;
        masterCells.append(Cell(d->sheet, pairs[i].first.toRect().topLeft()));
    }
    return masterCells;
}

bool CellStorage::locksCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
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

bool CellStorage::isLocked(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
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

bool CellStorage::hasLockedCells(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    typedef QPair<QRectF, bool> RectBoolPair;
    QList<QPair<QRectF, bool> > pairs = d->matrixStorage->intersectingPairs(region);
    foreach (const RectBoolPair& pair, pairs) {
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

void CellStorage::lockCells(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Start by unlocking the cells that we lock right now
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(rect.topLeft());  // FIXME
    if (!pair.first.isNull())
        d->matrixStorage->insert(Region(pair.first.toRect()), false);
    // Lock the cells
    if (rect.width() > 1 || rect.height() > 1)
        d->matrixStorage->insert(Region(rect), true);
}

void CellStorage::unlockCells(int column, int row)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    const QPair<QRectF, bool> pair = d->matrixStorage->containedPair(QPoint(column, row));
    if (pair.first.isNull())
        return;
    if (pair.second == false)
        return;
    if (pair.first.toRect().topLeft() != QPoint(column, row))
        return;
    const QRect rect = pair.first.toRect();
    d->matrixStorage->insert(Region(rect), false);
    // clear the values
    for (int r = rect.top(); r <= rect.bottom(); ++r) {
        for (int c = rect.left(); c <= rect.right(); ++c) {
            if (r != rect.top() || c != rect.left())
                setValue(c, r, Value());
        }
    }
}

QRect CellStorage::lockedCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
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

void CellStorage::insertColumns(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    // FIXME Stefan: Would it be better to directly alter the dependency tree?
    // TODO Stefan: Optimize: Avoid the double creation of the sub-storages, but don't process
    //              formulas, that will get out of bounds after the operation.
    const Region invalidRegion(QRect(QPoint(position, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    d->sheet->map()->addDamage(new CellDamage(d->sheet, invalidRegion, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->insertColumns(position, number);
    d->commentStorage->insertColumns(position, number);
    d->conditionsStorage->insertColumns(position, number);
    d->databaseStorage->insertColumns(position, number);
    d->formulaStorage->insertColumns(position, number);
    d->fusionStorage->insertColumns(position, number);
    d->linkStorage->insertColumns(position, number);
    d->matrixStorage->insertColumns(position, number);
    d->namedAreaStorage->insertColumns(position, number);
    d->styleStorage->insertColumns(position, number);
    d->userInputStorage->insertColumns(position, number);
    d->richTextStorage->insertColumns(position, number);
    d->validityStorage->insertColumns(position, number);
    d->valueStorage->insertColumns(position, number);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::removeColumns(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(QPoint(position, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    const Region region(QRect(QPoint(position - 1, 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, region, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->removeColumns(position, number);
    d->commentStorage->removeColumns(position, number);
    d->conditionsStorage->removeColumns(position, number);
    d->databaseStorage->removeColumns(position, number);
    d->formulaStorage->removeColumns(position, number);
    d->fusionStorage->removeColumns(position, number);
    d->linkStorage->removeColumns(position, number);
    d->matrixStorage->removeColumns(position, number);
    d->namedAreaStorage->removeColumns(position, number);
    d->styleStorage->removeColumns(position, number);
    d->userInputStorage->removeColumns(position, number);
    d->validityStorage->removeColumns(position, number);
    d->valueStorage->removeColumns(position, number);
    d->richTextStorage->removeColumns(position, number);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::insertRows(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(QPoint(1, position), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    d->sheet->map()->addDamage(new CellDamage(d->sheet, invalidRegion, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->insertRows(position, number);
    d->commentStorage->insertRows(position, number);
    d->conditionsStorage->insertRows(position, number);
    d->databaseStorage->insertRows(position, number);
    d->formulaStorage->insertRows(position, number);
    d->fusionStorage->insertRows(position, number);
    d->linkStorage->insertRows(position, number);
    d->matrixStorage->insertRows(position, number);
    d->namedAreaStorage->insertRows(position, number);
    d->styleStorage->insertRows(position, number);
    d->userInputStorage->insertRows(position, number);
    d->validityStorage->insertRows(position, number);
    d->valueStorage->insertRows(position, number);
    d->richTextStorage->insertRows(position, number);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::removeRows(int position, int number)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(QPoint(1, position), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    const Region region(QRect(QPoint(1, position - 1), QPoint(KS_colMax, KS_rowMax)), d->sheet);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, region, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->removeRows(position, number);
    d->commentStorage->removeRows(position, number);
    d->conditionsStorage->removeRows(position, number);
    d->databaseStorage->removeRows(position, number);
    d->formulaStorage->removeRows(position, number);
    d->fusionStorage->removeRows(position, number);
    d->linkStorage->removeRows(position, number);
    d->matrixStorage->removeRows(position, number);
    d->namedAreaStorage->removeRows(position, number);
    d->styleStorage->removeRows(position, number);
    d->userInputStorage->removeRows(position, number);
    d->validityStorage->removeRows(position, number);
    d->valueStorage->removeRows(position, number);
    d->richTextStorage->removeRows(position, number);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::removeShiftLeft(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom())), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    const Region region(QRect(rect.topLeft() - QPoint(1, 0), QPoint(KS_colMax, rect.bottom())), d->sheet);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, region, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->removeShiftLeft(rect);
    d->commentStorage->removeShiftLeft(rect);
    d->conditionsStorage->removeShiftLeft(rect);
    d->databaseStorage->removeShiftLeft(rect);
    d->formulaStorage->removeShiftLeft(rect);
    d->fusionStorage->removeShiftLeft(rect);
    d->linkStorage->removeShiftLeft(rect);
    d->matrixStorage->removeShiftLeft(rect);
    d->namedAreaStorage->removeShiftLeft(rect);
    d->styleStorage->removeShiftLeft(rect);
    d->userInputStorage->removeShiftLeft(rect);
    d->validityStorage->removeShiftLeft(rect);
    d->valueStorage->removeShiftLeft(rect);
    d->richTextStorage->removeShiftLeft(rect);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::insertShiftRight(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(KS_colMax, rect.bottom())), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    d->sheet->map()->addDamage(new CellDamage(d->sheet, invalidRegion, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->insertShiftRight(rect);
    d->commentStorage->insertShiftRight(rect);
    d->conditionsStorage->insertShiftRight(rect);
    d->databaseStorage->insertShiftRight(rect);
    d->formulaStorage->insertShiftRight(rect);
    d->fusionStorage->insertShiftRight(rect);
    d->linkStorage->insertShiftRight(rect);
    d->matrixStorage->insertShiftRight(rect);
    d->namedAreaStorage->insertShiftRight(rect);
    d->styleStorage->insertShiftRight(rect);
    d->userInputStorage->insertShiftRight(rect);
    d->validityStorage->insertShiftRight(rect);
    d->valueStorage->insertShiftRight(rect);
    d->richTextStorage->insertShiftRight(rect);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::removeShiftUp(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax)), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    const Region region(QRect(rect.topLeft() - QPoint(0, 1), QPoint(rect.right(), KS_rowMax)), d->sheet);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, region, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->removeShiftUp(rect);
    d->commentStorage->removeShiftUp(rect);
    d->conditionsStorage->removeShiftUp(rect);
    d->databaseStorage->removeShiftUp(rect);
    d->formulaStorage->removeShiftUp(rect);
    d->fusionStorage->removeShiftUp(rect);
    d->linkStorage->removeShiftUp(rect);
    d->matrixStorage->removeShiftUp(rect);
    d->namedAreaStorage->removeShiftUp(rect);
    d->styleStorage->removeShiftUp(rect);
    d->userInputStorage->removeShiftUp(rect);
    d->validityStorage->removeShiftUp(rect);
    d->valueStorage->removeShiftUp(rect);
    d->richTextStorage->removeShiftUp(rect);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

void CellStorage::insertShiftDown(const QRect& rect)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // Trigger a dependency update of the cells, which have a formula. (old positions)
    const Region invalidRegion(QRect(rect.topLeft(), QPoint(rect.right(), KS_rowMax)), d->sheet);
    PointStorage<Formula> subStorage = d->formulaStorage->subStorage(invalidRegion);
    Cell cell;
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger an update of the bindings and the named areas.
    d->sheet->map()->addDamage(new CellDamage(d->sheet, invalidRegion, CellDamage::Binding | CellDamage::NamedArea));

    d->bindingStorage->insertShiftDown(rect);
    d->commentStorage->insertShiftDown(rect);
    d->conditionsStorage->insertShiftDown(rect);
    d->databaseStorage->insertShiftDown(rect);
    d->formulaStorage->insertShiftDown(rect);
    d->fusionStorage->insertShiftDown(rect);
    d->linkStorage->insertShiftDown(rect);
    d->matrixStorage->insertShiftDown(rect);
    d->namedAreaStorage->insertShiftDown(rect);
    d->styleStorage->insertShiftDown(rect);
    d->userInputStorage->insertShiftDown(rect);
    d->validityStorage->insertShiftDown(rect);
    d->valueStorage->insertShiftDown(rect);
    d->richTextStorage->insertShiftDown(rect);

    // Trigger a dependency update of the cells, which have a formula. (new positions)
    subStorage = d->formulaStorage->subStorage(invalidRegion);
    for (int i = 0; i < subStorage.count(); ++i) {
        cell = Cell(d->sheet, subStorage.col(i), subStorage.row(i));
        d->sheet->map()->addDamage(new CellDamage(cell, CellDamage::Formula));
    }
    // Trigger a recalculation only for the cells, that depend on values in the changed region.
    Region providers = d->sheet->map()->dependencyManager()->reduceToProvidingRegion(invalidRegion);
    d->sheet->map()->addDamage(new CellDamage(d->sheet, providers, CellDamage::Value));
}

Cell CellStorage::firstInColumn(int col, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    Q_UNUSED(visiting);

    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->firstInColumn(col, &tmpRow);
    newRow = tmpRow;
    d->valueStorage->firstInColumn(col, &tmpRow);
    if (tmpRow)
        newRow = newRow ? qMin(newRow, tmpRow) : tmpRow;
    if (!newRow)
        return Cell();
    return Cell(d->sheet, col, newRow);
}

Cell CellStorage::firstInRow(int row, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->firstInRow(row, &tmpCol);
    newCol = tmpCol;
    d->valueStorage->firstInRow(row, &tmpCol);
    if (tmpCol)
        newCol = newCol ? qMin(newCol, tmpCol) : tmpCol;
    if (visiting == VisitAll) {
        tmpCol = d->styleStorage->firstColumnIndexInRow(row);
        if (tmpCol)
            newCol = newCol ? qMin(newCol, tmpCol) : tmpCol;
    }
    if (!newCol)
        return Cell();
    return Cell(d->sheet, newCol, row);
}

Cell CellStorage::lastInColumn(int col, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    Q_UNUSED(visiting);
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->lastInColumn(col, &tmpRow);
    newRow = tmpRow;
    d->valueStorage->lastInColumn(col, &tmpRow);
    newRow = qMax(newRow, tmpRow);
    if (!newRow)
        return Cell();
    return Cell(d->sheet, col, newRow);
}

Cell CellStorage::lastInRow(int row, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    Q_UNUSED(visiting);
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->lastInRow(row, &tmpCol);
    newCol = tmpCol;
    d->valueStorage->lastInRow(row, &tmpCol);
    newCol = qMax(newCol, tmpCol);
    if (!newCol)
        return Cell();
    return Cell(d->sheet, newCol, row);
}

Cell CellStorage::nextInColumn(int col, int row, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    Q_UNUSED(visiting);
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->nextInColumn(col, row, &tmpRow);
    newRow = tmpRow;
    d->valueStorage->nextInColumn(col, row, &tmpRow);
    if (tmpRow)
        newRow = newRow ? qMin(newRow, tmpRow) : tmpRow;
    if (!newRow)
        return Cell();
    return Cell(d->sheet, col, newRow);
}

Cell CellStorage::nextInRow(int col, int row, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->nextInRow(col, row, &tmpCol);
    newCol = tmpCol;
    d->valueStorage->nextInRow(col, row, &tmpCol);
    if (tmpCol)
        newCol = newCol ? qMin(newCol, tmpCol) : tmpCol;
    if (visiting == VisitAll) {
        tmpCol = d->styleStorage->nextColumnIndexInRow(col, row);
        if (tmpCol)
            newCol = newCol ? qMin(newCol, tmpCol) : tmpCol;
    }
    if (!newCol)
        return Cell();
    return Cell(d->sheet, newCol, row);
}

Cell CellStorage::prevInColumn(int col, int row, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    Q_UNUSED(visiting);
    int newRow = 0;
    int tmpRow = 0;
    d->formulaStorage->prevInColumn(col, row, &tmpRow);
    newRow = tmpRow;
    d->valueStorage->prevInColumn(col, row, &tmpRow);
    newRow = qMax(newRow, tmpRow);
    if (!newRow)
        return Cell();
    return Cell(d->sheet, col, newRow);
}

Cell CellStorage::prevInRow(int col, int row, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    Q_UNUSED(visiting);
    int newCol = 0;
    int tmpCol = 0;
    d->formulaStorage->prevInRow(col, row, &tmpCol);
    newCol = tmpCol;
    d->valueStorage->prevInRow(col, row, &tmpCol);
    newCol = qMax(newCol, tmpCol);
    if (!newCol)
        return Cell();
    return Cell(d->sheet, newCol, row);
}

int CellStorage::columns(bool includeStyles) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    int max = 0;
    max = qMax(max, d->commentStorage->usedArea().right());
    max = qMax(max, d->conditionsStorage->usedArea().right());
    max = qMax(max, d->fusionStorage->usedArea().right());
    if (includeStyles) max = qMax(max, d->styleStorage->usedArea().right());
    max = qMax(max, d->validityStorage->usedArea().right());
    max = qMax(max, d->formulaStorage->columns());
    max = qMax(max, d->linkStorage->columns());
    max = qMax(max, d->valueStorage->columns());

    // don't include bindings cause the bindingStorage does only listen to all cells in the sheet.
    //max = qMax(max, d->bindingStorage->usedArea().right());

    return max;
}

int CellStorage::rows(bool includeStyles) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    int max = 0;
    max = qMax(max, d->commentStorage->usedArea().bottom());
    max = qMax(max, d->conditionsStorage->usedArea().bottom());
    max = qMax(max, d->fusionStorage->usedArea().bottom());
    if (includeStyles) max = qMax(max, d->styleStorage->usedArea().bottom());
    max = qMax(max, d->validityStorage->usedArea().bottom());
    max = qMax(max, d->formulaStorage->rows());
    max = qMax(max, d->linkStorage->rows());
    max = qMax(max, d->valueStorage->rows());

    // don't include bindings cause the bindingStorage does only listen to all cells in the sheet.
    //max = qMax(max, d->bindingStorage->usedArea().bottom());

    return max;
}

CellStorage CellStorage::subStorage(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&d->bigUglyLock);
#endif
    CellStorage subStorage(d->sheet);
    *subStorage.d->formulaStorage = d->formulaStorage->subStorage(region);
    *subStorage.d->linkStorage = d->linkStorage->subStorage(region);
    *subStorage.d->valueStorage = d->valueStorage->subStorage(region);
    return subStorage;
}

const BindingStorage* CellStorage::bindingStorage() const
{
    return d->bindingStorage;
}

const CommentStorage* CellStorage::commentStorage() const
{
    return d->commentStorage;
}

const ConditionsStorage* CellStorage::conditionsStorage() const
{
    return d->conditionsStorage;
}

const FormulaStorage* CellStorage::formulaStorage() const
{
    return d->formulaStorage;
}

const FusionStorage* CellStorage::fusionStorage() const
{
    return d->fusionStorage;
}

const LinkStorage* CellStorage::linkStorage() const
{
    return d->linkStorage;
}

const StyleStorage* CellStorage::styleStorage() const
{
    return d->styleStorage;
}

const UserInputStorage* CellStorage::userInputStorage() const
{
    return d->userInputStorage;
}

const ValidityStorage* CellStorage::validityStorage() const
{
    return d->validityStorage;
}

const ValueStorage* CellStorage::valueStorage() const
{
    return d->valueStorage;
}

void CellStorage::startUndoRecording()
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // If undo is set, the recording wasn't stopped.
    // Should not happen, hence this assertion.
    Q_ASSERT(e->undoEnabled == false);

    d->bindingStorage->storeUndo(true);
    d->commentStorage->storeUndo(true);
    d->conditionsStorage->storeUndo(true);
    d->databaseStorage->storeUndo(true);
    d->formulaStorage->storeUndo(true);
    d->fusionStorage->storeUndo(true);
    d->linkStorage->storeUndo(true);
    d->matrixStorage->storeUndo(true);
    d->namedAreaStorage->storeUndo(true);
    d->styleStorage->storeUndo(true);
    d->userInputStorage->storeUndo(true);
    d->validityStorage->storeUndo(true);
    d->valueStorage->storeUndo(true);
    d->richTextStorage->storeUndo(true);

    d->undoEnabled = true;
}

void CellStorage::stopUndoRecording(KUndo2Command *parent)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    // If undoData is null, the recording wasn't started.
    // Should not happen, hence this assertion.
    Q_ASSERT(d->undoEnabled != 0);
    // append sub-commands to the parent command
    d->createCommand(parent);
    // TODO ... what to do about this?
    for (int i = 0; i < d->undoData->namedAreas.count(); ++i) {
        emit namedAreaRemoved(d->undoData->namedAreas[i].second);
    }

    d->bindingStorage->resetUndo();
    d->commentStorage->resetUndo();
    d->conditionsStorage->resetUndo();
    d->databaseStorage->resetUndo();
    d->formulaStorage->resetUndo();
    d->fusionStorage->resetUndo();
    d->linkStorage->resetUndo();
    d->matrixStorage->resetUndo();
    d->namedAreaStorage->resetUndo();
    d->styleStorage->resetUndo();
    d->userInputStorage->resetUndo();
    d->validityStorage->resetUndo();
    d->valueStorage->resetUndo();
    d->richTextStorage->resetUndo();

    d->undoEnabled = true;
}

void CellStorage::loadConditions(const QList<QPair<QRegion, Conditions> >& conditions)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->conditionsStorage->load(conditions);
}

void CellStorage::loadStyles(const QList<QPair<QRegion, Style> > &styles)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&d->bigUglyLock);
#endif
    d->styleStorage->load(styles);
}

void CellStorage::invalidateStyleCache()
{
    d->styleStorage->invalidateCache();
}


