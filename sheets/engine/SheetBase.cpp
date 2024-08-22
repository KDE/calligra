/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetBase.h"
#include "CellBase.h"
#include "CellBaseStorage.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "FormulaStorage.h"
#include "MapBase.h"
#include "NamedAreaManager.h"
#include "RecalcManager.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN SheetBase::Private
{
public:
    Private(SheetBase *sheet);
    ~Private() = default;

    MapBase *workbook;
    QString name;

    // true if sheet is hidden
    bool hide;

    bool autoCalc;
    bool firstLetterUpper;

    SheetBase *m_sheet;
    CellBaseStorage *cellStorage;
};

SheetBase::Private::Private(SheetBase *sheet)
    : m_sheet(sheet)
{
    cellStorage = nullptr;
}

SheetBase::SheetBase(MapBase *map, const QString &sheetName)
    : d(new Private(this))
{
    d->workbook = map;
    d->name = sheetName;
    d->cellStorage = new CellBaseStorage(this);

    d->hide = false;
    d->autoCalc = true;
    d->firstLetterUpper = false;
}

SheetBase::SheetBase(const SheetBase &other)
    : d(new Private(this))
{
    d->workbook = other.d->workbook;
    d->cellStorage = new CellBaseStorage(*other.d->cellStorage, this);

    d->hide = other.d->hide;
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

CellBaseStorage *SheetBase::cellStorage() const
{
    return d->cellStorage;
}

const FormulaStorage *SheetBase::formulaStorage() const
{
    return d->cellStorage->formulaStorage();
}

const ValidityStorage *SheetBase::validityStorage() const
{
    return d->cellStorage->validityStorage();
}

const ValueStorage *SheetBase::valueStorage() const
{
    return d->cellStorage->valueStorage();
}

MapBase *SheetBase::map() const
{
    return d->workbook;
}

QString SheetBase::sheetName() const
{
    return d->name;
}

bool SheetBase::setSheetName(const QString &name)
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

bool SheetBase::isHidden() const
{
    return d->hide;
}

void SheetBase::setHidden(bool hidden)
{
    d->hide = hidden;
    map()->onSheetHidden(this, hidden);
}

void SheetBase::hideSheet(bool _hide)
{
    setHidden(_hide);
}

void SheetBase::changeCellTabName(QString const &old_name, QString const &new_name)
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
    // Avoid possible recalculation of dependencies if the auto calc setting hasn't changed
    if (d->autoCalc == enable)
        return;

    d->autoCalc = enable;

    // If enabling automatic calculation, make sure that the dependencies are up-to-date
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

void SheetBase::showStatusMessage(const QString & /*message*/, int /*timeout*/) const
{
    // TODO
#ifndef Q_CC_MSVC
#warning Implement this.
#endif
}

bool SheetBase::onValidationFailed(Validity::Action action, const CellBase *cell, const QString & /*message*/, const QString & /*title*/) const
{
    QString msg = QString("Validation for cell ") + cell->fullName() + " failed.";
    showStatusMessage(msg);
    if (action == Validity::Information)
        return true;
    return false;
}

// Adjusts the coordinate 'pos' if the 'rect' area is being adjusted using operation 'ref'
// This assumes that the coordinates are all on the same sheet.
QPoint SheetBase::changeNameCellRefHelper(const QPoint &pos, const QRect &rect, ChangeRef ref, bool *changed, bool *valid, bool isStart)
{
    *changed = false;
    *valid = true;
    int col = pos.x();
    int row = pos.y();
    // Not affected if we're to the left/up of the modified area
    if (col < rect.left())
        return pos;
    if (row < rect.top())
        return pos;

    if (ref == ColumnInsert) {
        // The X-coordinate is shifting to the right.
        *changed = true;
        col += rect.width();
        if (col > KS_colMax)
            *valid = false;
    }
    if (ref == RowInsert) {
        // The Y-coordinate is shifting to the right.
        *changed = true;
        row += rect.height();
        if (row > KS_rowMax)
            *valid = false;
    }
    if (ref == ColumnRemove) {
        *changed = true;
        if (col <= rect.right()) {
            *valid = false; // inside the removed zone
            col = rect.left();
            if (!isStart)
                col--;
        } else
            col -= rect.width();
    }
    if (ref == RowRemove) {
        *changed = true;
        if (row <= rect.bottom()) {
            *valid = false; // inside the removed zone
            row = rect.top();
            if (!isStart)
                row--;
        } else
            row -= rect.height();
    }

    return QPoint(col, row);
}

void SheetBase::changeNameCellRef(const QRect &rect, ChangeRef ref, SheetBase *changedSheet)
{
    const FormulaStorage *formulas = formulaStorage();

    for (int c = 0; c < formulas->count(); ++c) {
        // For each formula, find out if it's affected.
        bool changed = false;
        QString newText('=');
        const Tokens tokens = formulas->data(c).tokens();
        for (int t = 0; t < tokens.count(); ++t) {
            const Token token = tokens[t];
            QString txt = token.text();
            if ((token.type() != Token::Cell) && (token.type() != Token::Range)) {
                newText.append(txt);
                continue;
            }
            if (map()->namedAreaManager()->contains(txt)) {
                newText.append(txt); // keep the area name
                continue;
            }
            const Region region = map()->regionFromName(token.text(), this);
            if (!region.isValid()) {
                newText.append(txt);
                continue;
            }
            // Check if the region is affected.
            bool affected = false;
            bool valid = true;
            Region newRegion;
            Region::ConstIterator end(region.constEnd());
            for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
                Region::Element *element = (*it);
                SheetBase *tgsheet = element->sheet();
                if (tgsheet != changedSheet)
                    continue; // not the correct sheet

                bool aff = false;
                bool valid1 = true;
                QRect r = element->rect();
                QPoint topleft = SheetBase::changeNameCellRefHelper(r.topLeft(), rect, ref, &aff, &valid1, true);
                if (aff)
                    affected = true;

                bool valid2 = true;
                QPoint bottomright = SheetBase::changeNameCellRefHelper(r.bottomRight(), rect, ref, &aff, &valid2, false);
                if (aff)
                    affected = true;

                valid = true;
                if ((!valid1) && (!valid2))
                    valid = false;
                // If one point is valid and the other one is not, we just use the calculated coordinates anyway.

                if (!valid) {
                    affected = true;
                    break;
                }

                if (topleft == bottomright)
                    newRegion.add(topleft, tgsheet, element->isColumnFixed(), element->isRowFixed());
                else
                    newRegion.add(QRect(topleft, bottomright),
                                  tgsheet,
                                  element->isTopFixed(),
                                  element->isLeftFixed(),
                                  element->isBottomFixed(),
                                  element->isRightFixed());
            }
            if (!affected) {
                newText.append(txt);
                continue;
            }
            changed = true;
            if (!valid) {
                newText.append('#' + i18n("Dependency") + '!');
                continue;
            }
            newText.append(newRegion.name(this));
        }

        if (!changed)
            continue; // no change - nothing to do

        CellBase cell(this, formulas->col(c), formulas->row(c));
        Formula formula(this, cell);
        formula.setExpression(newText);
        cell.setFormula(formula);
    }

    const Region region(1, 1, KS_colMax, KS_rowMax, this);
    map()->addDamage(new CellDamage(this, region, CellDamage::Appearance));
}

// 'rect' is the area being added/removed
void SheetBase::changeNameCellRefs(const QRect &rect, ChangeRef ref)
{
    for (SheetBase *sheet : map()->sheetList())
        sheet->changeNameCellRef(rect, ref, this);
}
