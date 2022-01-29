/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CellStorage.h"

// Qt
#ifdef CALLIGRA_SHEETS_MT
// #include <QReadLocker>
// #include <QWriteLocker>
#endif

// Sheets - storages
// #include "BindingStorage.h"
// #include "ConditionsStorage.h"
// #include "RectStorage.h"
// #include "StyleStorage.h"
// #include "ValidityStorage.h"

// Sheets - rest
// #include "Damages.h"
// #include "Map.h"
// #include "ModelSupport.h"
// #include "Sheet.h"

// commands
// #include "commands/PointStorageUndoCommand.h"
// #include "commands/RectStorageUndoCommand.h"
// #include "commands/StyleStorageUndoCommand.h"

// database
// #include "database/DatabaseStorage.h"
// #include "database/DatabaseManager.h"

Q_DECLARE_METATYPE(QSharedPointer<QTextDocument>)

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN CellStorage::Private
{
public:
    Private(Sheet* sheet)
            : sheet(sheet)
            , bindingStorage(new BindingStorage(sheet->map()))
            , conditionsStorage(new ConditionsStorage(sheet->map()))
            , databaseStorage(new DatabaseStorage(sheet->map()))
            , fusionStorage(new FusionStorage(sheet->map()))
            , linkStorage(new LinkStorage())
            , styleStorage(new StyleStorage(sheet->map()))
            , richTextStorage(new RichTextStorage())
    {
    }

    Private(const Private& other, Sheet* sheet)
            : sheet(sheet)
            , bindingStorage(new BindingStorage(*other.bindingStorage))
            , conditionsStorage(new ConditionsStorage(*other.conditionsStorage))
            , databaseStorage(new DatabaseStorage(*other.databaseStorage))
            , fusionStorage(new FusionStorage(*other.fusionStorage))
            , linkStorage(new LinkStorage(*other.linkStorage))
            , styleStorage(new StyleStorage(*other.styleStorage))
            , richTextStorage(new RichTextStorage(*other.richTextStorage))
    {
    }

    ~Private() {
        delete bindingStorage;
        delete conditionsStorage;
        delete databaseStorage;
        delete fusionStorage;
        delete linkStorage;
        delete styleStorage;
        delete richTextStorage;
    }

    void createCommand(KUndo2Command *parent) const;

    Sheet*                  sheet;
    BindingStorage*         bindingStorage;
    ConditionsStorage*      conditionsStorage;
    DatabaseStorage*        databaseStorage;
    FusionStorage*          fusionStorage;
    LinkStorage*            linkStorage;
    StyleStorage*           styleStorage;
    RichTextStorage*        richTextStorage;
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
        , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
        , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillExtraStorages();
}

CellStorage::CellStorage(const CellStorage& other)
        : QObject(other.d->sheet)
        , CellBaseStorage(other),
        , d(new Private(*other.d, other.d->sheet))
        , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
        , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillExtraStorages();
}

CellStorage::CellStorage(const CellStorage& other, Sheet* sheet)
        : QObject(sheet)
        , CellBaseStorage(other, sheet),
        , d(new Private(*other.d, sheet))
        , undoEnabled(false)
#ifdef CALLIGRA_SHEETS_MT
        , bigUglyLock(QReadWriteLock::Recursive)
#endif
{
    fillExtraStorages();
}

CellStorage::~CellStorage()
{
    storages.clear();
    delete d;
}

void CellStorage::fillExtraStorages() {
    storages.push_back (d->bindingStorage);
    storages.push_back (d->conditionsStorage);
    storages.push_back (d->databaseStorage);
    storages.push_back (d->fusionStorage);
    storages.push_back (d->linkStorage);
    storages.push_back (d->styleStorage);
    storages.push_back (d->richTextStorage);
}

Sheet* CellStorage::fullSheet() const
{
    return d->sheet;
}

void CellStorage::take(int col, int row)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
#endif
    return d->bindingStorage->contains(QPoint(column, row));
}

void CellStorage::setBinding(const Region& region, const Binding& binding)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->bindingStorage->insert(region, binding);
}

void CellStorage::removeBinding(const Region& region, const Binding& binding)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->bindingStorage->remove(region, binding);
}

Conditions CellStorage::conditions(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->conditionsStorage->contains(QPoint(column, row));
}

void CellStorage::setConditions(const Region& region, Conditions conditions)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->conditionsStorage->insert(region, conditions);
}

Database CellStorage::database(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
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

QVector< QPair<QRectF, Database> > CellStorage::databases(const Region& region) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->databaseStorage->intersectingPairs(region);
}

void CellStorage::setDatabase(const Region& region, const Database& database)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->databaseStorage->insert(region, database);
}

QString CellStorage::link(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->linkStorage->lookup(column, row);
}

void CellStorage::setLink(int column, int row, const QString& link)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    if (link.isEmpty())
        d->linkStorage->take(column, row);
    else
        d->linkStorage->insert(column, row, link);
}


void CellStorage::emitInsertNamedArea(const Region &region, const QString &namedArea)
{
    emit insertNamedArea(region, namedArea);
}

Style CellStorage::style(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->styleStorage->contains(QPoint(column, row));
}

Style CellStorage::style(const QRect& rect) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->styleStorage->contains(rect);
}

void CellStorage::setStyle(const Region& region, const Style& style)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->styleStorage->insert(region, style);
}

void CellStorage::insertSubStyle(const QRect &rect, const SharedSubStyle &subStyle)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->styleStorage->insert(rect, subStyle);
}

QSharedPointer<QTextDocument> CellStorage::richText(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
#endif
    return d->richTextStorage->lookup(column, row);
}

void CellStorage::setRichText(int column, int row, QSharedPointer<QTextDocument> text)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    if (text.isNull())
        d->richTextStorage->take(column, row);
    else
        d->richTextStorage->insert(column, row, text);
}

bool CellStorage::doesMergeCells(int column, int row) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QWriteLocker(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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

Cell CellStorage::firstInColumn(int col, Visiting visiting) const
{
#ifdef CALLIGRA_SHEETS_MT
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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
    QReadLocker rl(&bigUglyLock);
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

const ConditionsStorage* CellStorage::conditionsStorage() const
{
    return d->conditionsStorage;
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

void CellStorage::startUndoRecording()
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    // If undo is set, the recording wasn't stopped.
    // Should not happen, hence this assertion.
    Q_ASSERT(undoEnabled == false);

    for (BaseStorage *storage : storages)
        storage->storeUndo(true);

    undoEnabled = true;
}

void CellStorage::stopUndoRecording(KUndo2Command *parent)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    // If undoData is null, the recording wasn't started.
    // Should not happen, hence this assertion.
    Q_ASSERT(undoEnabled != 0);
    // append sub-commands to the parent command
    d->createCommand(parent);
    // TODO ... what to do about this?
    for (int i = 0; i < d->undoData->namedAreas.count(); ++i) {
        emit namedAreaRemoved(d->undoData->namedAreas[i].second);
    }

    for (BaseStorage *storage : storages)
        storage->resetUndo();

    undoEnabled = true;
}

void CellStorage::loadConditions(const QList<QPair<QRegion, Conditions> >& conditions)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->conditionsStorage->load(conditions);
}

void CellStorage::loadStyles(const QList<QPair<QRegion, Style> > &styles)
{
#ifdef CALLIGRA_SHEETS_MT
    QWriteLocker(&bigUglyLock);
#endif
    d->styleStorage->load(styles);
}

void CellStorage::invalidateStyleCache()
{
    d->styleStorage->invalidateCache();
}


