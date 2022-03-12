/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetBase.h"
#include "CellBase.h"
#include "MapBase.h"
#include "CellBaseStorage.h"
#include "Damages.h"
#include "FormulaStorage.h"
#include "DependencyManager.h"
#include "NamedAreaManager.h"
#include "RecalcManager.h"

#include <KLocalizedString>


using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SheetBase::Private
{
public:
    Private(SheetBase *sheet);
    ~Private() {}

    MapBase *workbook;
    QString name;

    bool autoCalc;
    bool firstLetterUpper;

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

    d->autoCalc = true;
    d->firstLetterUpper = false;
}

SheetBase::SheetBase(const SheetBase &other)
        : d(new Private(this))
{
    d->workbook = other.d->workbook;

    d->autoCalc = other.d->autoCalc;
    d->firstLetterUpper = other.d->firstLetterUpper;

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

bool SheetBase::rowIsHidden(int /*row*/) const
{
    return false;
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

bool SheetBase::isAutoCalculationEnabled() const
{
    return d->autoCalc;
}

void SheetBase::setAutoCalculationEnabled(bool enable)
{
    //Avoid possible recalculation of dependencies if the auto calc setting hasn't changed
    if (d->autoCalc == enable)
        return;

    d->autoCalc = enable;

    //If enabling automatic calculation, make sure that the dependencies are up-to-date
    if (enable) {
        map()->dependencyManager()->addSheet(this);
        map()->recalcManager()->recalcSheet(this);
    } else {
        map()->dependencyManager()->removeSheet(this);
    }
}

bool SheetBase::getFirstLetterUpper() const
{
    return d->firstLetterUpper;
}

void SheetBase::setFirstLetterUpper(bool _firstUpper)
{
    d->firstLetterUpper = _firstUpper;
}


void SheetBase::showStatusMessage(const QString & /*message*/, int /*timeout*/) const {
    // TODO
#warning Implement this.
}

bool SheetBase::onValidationFailed(Validity::Action action, const CellBase *cell, const QString &/*message*/, const QString &/*title*/) const
{
    QString msg = QString("Validation for cell ") + cell->fullName() + " failed.";
    showStatusMessage(msg);
    if (action == Validity::Information) return true;
    return false;
}


QString SheetBase::changeNameCellRefHelper(const QPoint& pos, bool fullRowOrColumn, ChangeRef ref,
                                       int nbCol, const QPoint& point, bool isColumnFixed,
                                       bool isRowFixed)
{
    QString newPoint;
    int col = point.x();
    int row = point.y();
    // update column
    if (isColumnFixed)
        newPoint.append('$');
    if (ref == ColumnInsert &&
            col + nbCol <= KS_colMax &&
            col >= pos.x() &&    // Column after the new one : +1
            (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += CellBase::columnName(col + nbCol);
    } else if (ref == ColumnRemove &&
               col > pos.x() &&    // Column after the deleted one : -1
               (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += CellBase::columnName(col - nbCol);
    } else
        newPoint += CellBase::columnName(col);

    // Update row
    if (isRowFixed)
        newPoint.append('$');
    if (ref == RowInsert &&
            row + nbCol <= KS_rowMax &&
            row >= pos.y() &&   // Row after the new one : +1
            (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row + nbCol);
    } else if (ref == RowRemove &&
               row > pos.y() &&   // Row after the deleted one : -1
               (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row - nbCol);
    } else
        newPoint += QString::number(row);

    if (((ref == ColumnRemove
            && (col >= pos.x() && col < pos.x() + nbCol) // Column is the deleted one : error
            && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowRemove
             && (row >= pos.y() && row < pos.y() + nbCol) // Row is the deleted one : error
             && (fullRowOrColumn || col == pos.x())) ||
            (ref == ColumnInsert
             && col + nbCol > KS_colMax
             && col >= pos.x()     // Column after the new one : +1
             && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowInsert
             && row + nbCol > KS_rowMax
             && row >= pos.y() // Row after the new one : +1
             && (fullRowOrColumn || col == pos.x())))) {
        newPoint = '#' + i18n("Dependency") + '!';
    }
    return newPoint;
}

QString SheetBase::changeNameCellRefHelper(const QPoint& pos, const QRect& rect, bool fullRowOrColumn, ChangeRef ref,
                                       int nbCol, const QPoint& point, bool isColumnFixed,
                                       bool isRowFixed)
{
    const bool isFirstColumn = pos.x() == rect.left();
    const bool isLastColumn = pos.x() == rect.right();
    const bool isFirstRow = pos.y() == rect.top();
    const bool isLastRow = pos.y() == rect.bottom();

    QString newPoint;
    int col = point.x();
    int row = point.y();
    // update column
    if (isColumnFixed)
        newPoint.append('$');
    if (ref == ColumnInsert &&
            col + nbCol <= KS_colMax &&
            col >= pos.x() &&    // Column after the new one : +1
            (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += CellBase::columnName(col + nbCol);
    } else if (ref == ColumnRemove &&
               (col > pos.x() ||
                (col == pos.x() && isLastColumn)) &&    // Column after the deleted one : -1
               (fullRowOrColumn || row == pos.y())) {  // All rows or just one
        newPoint += CellBase::columnName(col - nbCol);
    } else
        newPoint += CellBase::columnName(col);

    // Update row
    if (isRowFixed)
        newPoint.append('$');
    if (ref == RowInsert &&
            row + nbCol <= KS_rowMax &&
            row >= pos.y() &&   // Row after the new one : +1
            (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row + nbCol);
    } else if (ref == RowRemove &&
               (row > pos.y() ||
                (row == pos.y() && isLastRow)) &&   // Row after the deleted one : -1
               (fullRowOrColumn || col == pos.x())) {  // All columns or just one
        newPoint += QString::number(row - nbCol);
    } else
        newPoint += QString::number(row);

    if (((ref == ColumnRemove
            && col == pos.x() // Column is the deleted one : error
            && (fullRowOrColumn || row == pos.y())
            && (isFirstColumn && isLastColumn)) ||
            (ref == RowRemove
             && row == pos.y() // Row is the deleted one : error
             && (fullRowOrColumn || col == pos.x())
             && (isFirstRow && isLastRow)) ||
            (ref == ColumnInsert
             && col + nbCol > KS_colMax
             && col >= pos.x()     // Column after the new one : +1
             && (fullRowOrColumn || row == pos.y())) ||
            (ref == RowInsert
             && row + nbCol > KS_rowMax
             && row >= pos.y() // Row after the new one : +1
             && (fullRowOrColumn || col == pos.x())))) {
        newPoint = '#' + i18n("Dependency") + '!';
    }
    return newPoint;
}

void SheetBase::changeNameCellRef(const QPoint& pos, bool fullRowOrColumn, ChangeRef ref,
                              const QString& tabname, int nbCol)
{
    for (int c = 0; c < formulaStorage()->count(); ++c) {
        QString newText('=');
        const Tokens tokens = formulaStorage()->data(c).tokens();
        for (int t = 0; t < tokens.count(); ++t) {
            const Token token = tokens[t];
            switch (token.type()) {
            case Token::Cell:
            case Token::Range: {
                if (map()->namedAreaManager()->contains(token.text())) {
                    newText.append(token.text()); // simply keep the area name
                    break;
                }
                const Region region = map()->regionFromName(token.text(), this);
                if (!region.isValid() || !region.isContiguous()) {
                    newText.append(token.text());
                    break;
                }
                if (!region.firstSheet() && tabname != sheetName()) {
                    // nothing to do here
                    newText.append(token.text());
                    break;
                }
                // actually only one element in here, but we need extended access to the element
                Region::ConstIterator end(region.constEnd());
                for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
                    Region::Element* element = (*it);
                    if (element->type() == Region::Element::Point) {
                        if (element->sheet())
                            newText.append(element->sheet()->sheetName() + '!');
                        QString newPoint = changeNameCellRefHelper(pos, fullRowOrColumn, ref,
                                           nbCol,
                                           element->rect().topLeft(),
                                           element->isColumnFixed(),
                                           element->isRowFixed());
                        newText.append(newPoint);
                    } else { // (element->type() == Region::Element::Range)
                        if (element->sheet())
                            newText.append(element->sheet()->sheetName() + '!');
                        QString newPoint;
                        newPoint = changeNameCellRefHelper(pos, element->rect(), fullRowOrColumn, ref,
                                                           nbCol, element->rect().topLeft(),
                                                           element->isColumnFixed(),
                                                           element->isRowFixed());
                        newText.append(newPoint + ':');
                        newPoint = changeNameCellRefHelper(pos, element->rect(), fullRowOrColumn, ref,
                                                           nbCol, element->rect().bottomRight(),
                                                           element->isColumnFixed(),
                                                           element->isRowFixed());
                        newText.append(newPoint);
                    }
                }
                break;
            }
            default: {
                newText.append(token.text());
                break;
            }
            }
        }

        CellBase cell(this, formulaStorage()->col(c), formulaStorage()->row(c));
        Formula formula(this, cell);
        formula.setExpression(newText);
        cell.setFormula(formula);
    }
}

void SheetBase::changeNameCellRefs(const QRect& rect, bool fullRowOrColumn, ChangeRef ref)
{
    int number = 0, first = 0, last = 0;
    bool isColumn = ((ref == ColumnInsert) || (ref == ColumnRemove));
    if (isColumn) {
        first = rect.top();
        last = rect.bottom();
        number = rect.right() - rect.left() + 1;
    }
    else {
        first = rect.left();
        last = rect.right();
        number = rect.bottom() - rect.top() + 1;
    }

    for (SheetBase* sheet : map()->sheetList()) {
        for (int i = first; i <= last; ++i) {
            QPoint p = isColumn ? QPoint(rect.left(), i) : QPoint(i, rect.top());
            sheet->changeNameCellRef(p, fullRowOrColumn, ref, sheetName(), number);
        }
    }

}



