/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "DependencyManager.h"
#include "DependencyManager_p.h"

#include "Cell.h"
#include "CellStorage.h"
#include "Formula.h"
#include "FormulaStorage.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "Region.h"
#include "RTree.h"
#include "Sheet.h"
#include "Value.h"
#include "DocBase.h"
#include "ElapsedTime_p.h"

#include <QHash>
#include <QList>

#include <KoUpdater.h>

using namespace Calligra::Sheets;

// This is currently not called - but it's really convenient to call it from
// gdb or from debug output to check that everything is set up ok.
void DependencyManager::Private::dump() const
{
    QMap<Cell, Region>::ConstIterator mend(providers.end());
    for (QMap<Cell, Region>::ConstIterator mit(providers.begin()); mit != mend; ++mit) {
        Cell cell = mit.key();

        QStringList debugStr;
        Region::ConstIterator rend((*mit).constEnd());
        for (Region::ConstIterator rit((*mit).constBegin()); rit != rend; ++rit) {
            debugStr << (*rit)->name();
        }

        debugSheetsFormula << cell.name() << " consumes values of:" << debugStr.join(",");
    }

    foreach(Sheet* sheet, consumers.keys()) {
        const QList< QPair<QRectF, Cell> > pairs = consumers[sheet]->intersectingPairs(QRect(1, 1, KS_colMax, KS_rowMax)).values();
        QHash<QString, QString> table;
        for (int i = 0; i < pairs.count(); ++i) {
            Region tmpRange(pairs[i].first.toRect(), sheet);
            table.insertMulti(tmpRange.name(), pairs[i].second.name());
        }
        foreach(const QString &uniqueKey, table.uniqueKeys()) {
            QStringList debugStr(table.values(uniqueKey));
            debugSheetsFormula << uniqueKey << " provides values for:" << debugStr.join(",");
        }
    }

    foreach(const Cell &cell, depths.keys()) {
        QString cellName = cell.name();
        while (cellName.count() < 4) cellName.prepend(' ');
        debugSheetsFormula << "depth(" << cellName << " ) =" << depths[cell];
    }
}

DependencyManager::DependencyManager(const Map* map)
        : d(new Private)
{
    d->map = map;
}

DependencyManager::~DependencyManager()
{
    qDeleteAll(d->consumers);
    delete d;
}

void DependencyManager::reset()
{
    d->reset();
}

void DependencyManager::regionChanged(const Region& region)
{
    if (region.isEmpty())
        return;
    debugSheetsFormula << "DependencyManager::regionChanged" << region.name();
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        const Sheet* sheet = (*it)->sheet();

        for (int col = range.left(); col <= range.right(); ++col) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell(sheet, col, row);
                const Formula formula = cell.formula();

                // remove it and all its consumers from the reference depth list
                d->removeDepths(cell);

                // cell without a formula? remove it
                if (formula.expression().isEmpty()) {
                    d->removeDependencies(cell);
                    continue;
                }

                d->generateDependencies(cell, formula);
            }
        }
    }
    {
        ElapsedTime et("Computing reference depths", ElapsedTime::PrintOnlyTime);
        d->generateDepths(region);
    }
//     d->dump();
}

void DependencyManager::namedAreaModified(const QString &name)
{
    d->namedAreaModified(name);
}

void DependencyManager::addSheet(Sheet *sheet)
{
    // Manages also the revival of a deleted sheet.
    Q_UNUSED(sheet);
#if 0 // TODO Stefan: Enable, if dependencies should not be tracked all the time.
    ElapsedTime et("Generating dependencies", ElapsedTime::PrintOnlyTime);

    // Clear orphaned dependencies (i.e. cells formerly containing formulas)
    // FIXME Stefan: Iterate only over the consumers in sheet. Needs adjustment
    //               of the way the providers are stored. Now: only by cell.
    //               Future: QHash<Sheet*, QHash<Cell, Region> >
    const QList<Cell> consumers = d->providers.keys();
    foreach(const Cell& cell, consumers) {
        if (cell.sheet() == sheet) {
            // Those cells may had got providing regions. Clear them first.
            // TODO

            // Clear this cell as provider.
            d->providers.remove(cell);
        }
    }

    Cell cell;
    for (int c = 0; c < sheet->formulaStorage()->count(); ++c) {
        cell = Cell(sheet, sheet->formulaStorage()->col(c), sheet->formulaStorage()->row(c));

        d->generateDependencies(cell, sheet->formulaStorage()->data(c));
        if (!d->depths.contains(cell)) {
            int depth = d->computeDepth(cell);
            d->depths.insert(cell , depth);
        }
    }
#endif
}

void DependencyManager::removeSheet(Sheet *sheet)
{
    Q_UNUSED(sheet);
    // TODO Stefan: Implement, if dependencies should not be tracked all the time.
}

void DependencyManager::updateAllDependencies(const Map* map, KoUpdater *updater)
{
    ElapsedTime et("Generating dependencies", ElapsedTime::PrintOnlyTime);

    // clear everything
    d->providers.clear();
    qDeleteAll(d->consumers);
    d->consumers.clear();
    d->namedAreaConsumers.clear();
    d->depths.clear();

    int cellsCount = 9;

    if (updater) {
        updater->setProgress(0);

        foreach(const Sheet* sheet, map->sheetList())
            cellsCount += sheet->formulaStorage()->count();
    }

    Cell cell;
    int cellCurrent = 0;
    foreach(const Sheet* sheet, map->sheetList()) {
        for (int c = 0; c < sheet->formulaStorage()->count(); ++c, ++cellCurrent) {
            cell = Cell(sheet, sheet->formulaStorage()->col(c), sheet->formulaStorage()->row(c));

            d->generateDependencies(cell, sheet->formulaStorage()->data(c));
            if (!sheet->formulaStorage()->data(c).isValid())
                cell.setValue(Value::errorPARSE());

            if (updater)
                updater->setProgress(int(qreal(cellCurrent) / qreal(cellsCount) * 50.));
        }
    }
    cellCurrent = 0;
    foreach(const Sheet* sheet, map->sheetList()) {
        for (int c = 0; c < sheet->formulaStorage()->count(); ++c, ++cellCurrent) {
            cell = Cell(sheet, sheet->formulaStorage()->col(c), sheet->formulaStorage()->row(c));

            if (!d->depths.contains(cell)) {
                int depth = d->computeDepth(cell);
                d->depths.insert(cell , depth);
            }

            if (updater)
                updater->setProgress(50 + int(qreal(cellCurrent) / qreal(cellsCount) * 50.));
        }
    }

    if (updater)
        updater->setProgress(100);
}

QMap<Cell, int> DependencyManager::depths() const
{
    return d->depths;
}

Calligra::Sheets::Region DependencyManager::consumingRegion(const Cell& cell) const
{
    return d->consumingRegion(cell);
}

Calligra::Sheets::Region DependencyManager::reduceToProvidingRegion(const Region& region) const
{
    Region providingRegion;
    QList< QPair<QRectF, Cell> > pairs;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        Sheet* const sheet = (*it)->sheet();
        QHash<Sheet*, RTree<Cell>*>::ConstIterator cit = d->consumers.constFind(sheet);
        if (cit == d->consumers.constEnd())
            continue;

        pairs = cit.value()->intersectingPairs((*it)->rect()).values();
        for (int i = 0; i < pairs.count(); ++i)
            providingRegion.add(pairs[i].first.toRect() & (*it)->rect(), sheet);
    }
    return providingRegion;
}

void DependencyManager::regionMoved(const Region& movedRegion, const Cell& destination)
{
    Region::Point locationOffset(destination.cellPosition() - movedRegion.boundingRect().topLeft());

    Region::ConstIterator end(movedRegion.constEnd());
    for (Region::ConstIterator it(movedRegion.constBegin()); it != end; ++it) {
        Sheet* const sheet = (*it)->sheet();
        locationOffset.setSheet((sheet == destination.sheet()) ? 0 : destination.sheet());

        QHash<Sheet*, RTree<Cell>*>::ConstIterator cit = d->consumers.constFind(sheet);
        if (cit == d->consumers.constEnd())
            continue;

        QList<Cell> dependentLocations = cit.value()->intersects((*it)->rect());
        foreach(const Cell &c, dependentLocations) {
            updateFormula(c, (*it), locationOffset);
        }
    }
}

void DependencyManager::updateFormula(const Cell& cell, const Region::Element* oldLocation, const Region::Point& offset)
{
    // Not a formula -> no dependencies
    if (!cell.isFormula())
        return;

    const Formula formula = cell.formula();

    // Broken formula -> meaningless dependencies
    if (!formula.isValid())
        return;

    Tokens tokens = formula.tokens();

    //return empty list if the tokens aren't valid
    if (!tokens.valid())
        return;

    QString expression('=');
    Sheet* sheet = cell.sheet();
    foreach(const Token &token, tokens) {
        //parse each cell/range and put it to our expression
        if (token.type() == Token::Cell || token.type() == Token::Range) {
            // FIXME Stefan: Special handling for named areas
            const Region region(token.text(), sheet->map(), sheet);
            //debugSheetsFormula << region.name();

            // the offset contains a sheet, only if it was an intersheet move.
            if ((oldLocation->sheet() == region.firstSheet()) &&
                    (oldLocation->rect().contains(region.firstRange()))) {
                const Region yetAnotherRegion(region.firstRange().translated(offset.pos()),
                                              offset.sheet() ? offset.sheet() : sheet);
                expression.append(yetAnotherRegion.name(sheet));
            } else {
                expression.append(token.text());
            }
        } else {
            expression.append(token.text());
        }
    }
    Cell(cell).parseUserInput(expression);
}

void DependencyManager::Private::reset()
{
    providers.clear();
    consumers.clear();
}

Calligra::Sheets::Region DependencyManager::Private::consumingRegion(const Cell& cell) const
{
    QHash<Sheet*, RTree<Cell>*>::ConstIterator cit = consumers.constFind(cell.sheet());
    if (cit == consumers.constEnd()) {
        //debugSheetsFormula << "No consumer tree found for the cell's sheet.";
        return Region();
    }

    const QList<Cell> consumers = cit.value()->contains(cell.cellPosition());

    Region region;
    foreach(const Cell& c, consumers) 
        region.add(c.cellPosition(), c.sheet());
    return region;
}

void DependencyManager::Private::namedAreaModified(const QString& name)
{
    // since area names are something like aliases, modifying an area name
    // basically means that all cells referencing this area should be treated
    // as modified - that will retrieve updated area ranges and also update
    // everything as necessary ...
    QHash<QString, QList<Cell> >::ConstIterator it = namedAreaConsumers.constFind(name);
    if (it == namedAreaConsumers.constEnd())
        return;

    Region region;
    const QList<Cell> namedAreaConsumersList = it.value();
    foreach(const Cell &c, namedAreaConsumersList) {
        generateDependencies(c, c.formula());
        region.add(c.cellPosition(), c.sheet());
    }
    generateDepths(region);
}

void DependencyManager::Private::removeDependencies(const Cell& cell)
{
    // look if the cell has any providers
    QMap<Cell, Region>::ConstIterator pit = providers.constFind(cell);
    if (pit == providers.constEnd())
        return;  //it doesn't - nothing more to do

    // first this cell is no longer a provider for all consumers
    Region region = pit.value();
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        QHash<Sheet*, RTree<Cell>*>::ConstIterator cit = consumers.constFind((*it)->sheet());
        if (cit != consumers.constEnd()) {
            cit.value()->remove((*it)->rect(), cell);
        }
    }

    // remove information about named area dependencies
    QHash<QString, QList<Cell> >::Iterator nit(namedAreaConsumers.begin());
    while (nit != namedAreaConsumers.end()) {
        nit.value().removeAll(cell);
        if (nit.value().isEmpty())
            nit = namedAreaConsumers.erase(nit);
        else
            ++nit;
    }

    // clear the circular dependency flags
    removeCircularDependencyFlags(providers.value(cell), Backward);
    removeCircularDependencyFlags(consumingRegion(cell), Forward);

    // finally, remove the providers for this cell
    providers.remove(cell);
}

void DependencyManager::Private::removeDepths(const Cell& cell)
{
    QMap<Cell, int>::Iterator dit = depths.find(cell);
    if (dit == depths.end())
        return;
    QHash<Sheet*, RTree<Cell>*>::ConstIterator cit = consumers.constFind(cell.sheet());
    if (cit == consumers.constEnd())
        return;
    depths.erase(dit);
    const QList<Cell> consumers = cit.value()->contains(cell.cellPosition());
    foreach(const Cell &c, consumers)
        removeDepths(c);
}

void DependencyManager::Private::generateDependencies(const Cell& cell, const Formula& formula)
{
    //get rid of old dependencies first
    removeDependencies(cell);

    //new dependencies only need to be generated if the cell contains a formula
//     if (cell.isNull())
//         return;
//     if (!cell.isFormula())
//         return;

    //now we need to generate the providing region
    computeDependencies(cell, formula);
}

void DependencyManager::Private::generateDepths(const Region& region)
{
    QSet<Cell> computedDepths;

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        const Sheet* sheet = (*it)->sheet();
        const CellStorage *cells = sheet->cellStorage();

        int bottom = range.bottom();
        if (bottom > cells->rows()) bottom = cells->rows();
        int right = range.right();
        if (right > cells->columns()) right = cells->columns();

        for (int row = range.top(); row <= bottom; ++row) {
            for (int col = range.left(); col <= right; ++col) {
                Cell cell(sheet, col, row);
                generateDepths(cell, computedDepths);
            }
        }
    }
}

void DependencyManager::Private::generateDepths(Cell cell, QSet<Cell>& computedDepths)
{
    static QSet<Cell> processedCells;

    //prevent infinite recursion (circular dependencies)
    if (processedCells.contains(cell) || cell.value() == Value::errorCIRCLE()) {
        debugSheetsFormula << "Circular dependency at" << cell.fullName();
        cell.setValue(Value::errorCIRCLE());
        depths.insert(cell, 0);
        return;
    }
    if (computedDepths.contains(cell)) {
        return;
    }

    // set the compute reference depth flag
    processedCells.insert(cell);

    int depth = computeDepth(cell);
    depths.insert(cell, depth);

    computedDepths.insert(cell);

    // Recursion. We need the whole dependency tree of the changed region.
    // An infinite loop is prevented by the check above.
    QHash<Sheet*, RTree<Cell>*>::ConstIterator cit = consumers.constFind(cell.sheet());
    if (cit == consumers.constEnd()) {
        processedCells.remove(cell);
        return;
    }
    const QList<Cell> consumers = cit.value()->contains(cell.cellPosition());
    foreach (const Cell &c, consumers) {
        generateDepths(c, computedDepths);
    }

    // clear the compute reference depth flag
    processedCells.remove(cell);
}

int DependencyManager::Private::computeDepth(Cell cell) const
{
    // a set of cell, which depth is currently calculated
    static QSet<Cell> processedCells;

    //prevent infinite recursion (circular dependencies)
    if (processedCells.contains(cell) || cell.value() == Value::errorCIRCLE()) {
        debugSheetsFormula << "Circular dependency at" << cell.fullName();
        cell.setValue(Value::errorCIRCLE());
        return 0;
    }

    // set the compute reference depth flag
    processedCells.insert(cell);

    int depth = 0;

    const Region region = providers.value(cell);

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        Sheet* sheet = (*it)->sheet();
        const int right = range.right();
        const int bottom = range.bottom();
        for (int col = range.left(); col <= right; ++col) {
            for (int row = range.top(); row <= bottom; ++row) {
                Cell referencedCell = Cell(sheet, col, row);
                if (!providers.contains(referencedCell)) {
                    // no further references
                    // depth is one at least
                    depth = qMax(depth, 1);
                    continue;
                }

                QMap<Cell, int>::ConstIterator it = depths.constFind(referencedCell);
                if (it != depths.constEnd()) {
                    // the referenced cell depth was already computed
                    depth = qMax(it.value() + 1, depth);
                    continue;
                }

                // compute the depth of the referenced cell, add one and
                // take it as new depth, if it's greater than the current one
                depth = qMax(computeDepth(referencedCell) + 1, depth);
            }
        }
    }

    //clear the computing reference depth flag
    processedCells.remove(cell);

    return depth;
}

void DependencyManager::Private::computeDependencies(const Cell& cell, const Formula& formula)
{
    // Broken formula -> meaningless dependencies
    if (!formula.isValid())
        return;

    const Tokens tokens = formula.tokens();

    //return empty list if the tokens aren't valid
    if (!tokens.valid())
        return;

    Sheet* sheet = cell.sheet();
    int inAreasCall = 0;
    Region providingRegion;
    for (int i = 0; i < tokens.count(); i++) {
        const Token &token = tokens[i];

        if (inAreasCall) {
            if (token.isOperator() && token.asOperator() == Token::LeftPar)
                inAreasCall++;
            else if (token.isOperator() && token.asOperator() == Token::RightPar)
                inAreasCall--;
        } else {
            if (i > 0 && token.isOperator() && token.asOperator() == Token::LeftPar && tokens[i-1].isIdentifier() && QString::compare(tokens[i-1].text(), "AREAS", Qt::CaseInsensitive) == 0)
                inAreasCall = 1;

            //parse each cell/range and put it to our Region
            if (token.type() == Token::Cell || token.type() == Token::Range) {
                // check for named area
                const bool isNamedArea = sheet->map()->namedAreaManager()->contains(token.text());
                if (isNamedArea) {
                    // add cell as consumer of the named area
                    namedAreaConsumers[token.text()].append(cell);
                }

                // check if valid cell/range
                Region region(token.text(), sheet->map(), sheet);
                if (region.isValid()) {
                    if (isNamedArea) {
                        if ((i > 0 && tokens[i-1].isOperator()) || (i < tokens.count()-1 && tokens[i+1].isOperator())) {
                            // TODO: this check is not quite correct, to really properly determine if the entire range is referenced
                            // or just a single cell we would need to actually have the compile formula, not just the tokenized one
                            // basically this is the same logic as Formula::Private::valueOrElement

                            Region realRegion = region.intersectedWithRow(cell.row());
                            if (!realRegion.isEmpty()) {
                                region = realRegion;
                            }
                        }
                    }
                    // add it to the providers
                    providingRegion.add(region);

                    Sheet* sheet = region.firstSheet();

                    // create consumer tree, if not existing yet
                    QHash<Sheet*, RTree<Cell>*>::iterator it = consumers.find(sheet);
                    if (it == consumers.end()) {
                        it = consumers.insert(sheet, new RTree<Cell>());
                    }
                    // add cell as consumer of the range
                    it.value()->insert(region.firstRange(), cell);
                }
            }
        }
    }

    // store the providing region
    // NOTE Stefan: Also store cells without dependencies to avoid an
    //              iteration over all cells in a map/sheet on recalculation.
    // empty region will be created automatically, if necessary
    providers[cell].add(providingRegion);
}

void DependencyManager::Private::removeCircularDependencyFlags(const Region& region, Direction direction)
{
    // a set of cells, which circular dependency flag is currently removed
    static QSet<Cell> processedCells;

    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        const Sheet* sheet = (*it)->sheet();
        for (int col = range.left(); col <= range.right(); ++col) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell(sheet, col, row);

                if (processedCells.contains(cell))
                    continue;
                processedCells.insert(cell);

                if (cell.value() == Value::errorCIRCLE())
                    cell.setValue(Value::empty());

                if (direction == Backward)
                    removeCircularDependencyFlags(providers.value(cell), Backward);
                else // Forward
                    removeCircularDependencyFlags(consumingRegion(cell), Forward);

                processedCells.remove(cell);
            }
        }
    }
}
