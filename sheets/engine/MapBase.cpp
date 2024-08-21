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

#include "DependencyManager.h"
#include "NamedAreaManager.h"
#include "RecalcManager.h"

#include "CalculationSettings.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueParser.h"

#include <KLocalizedString>
#include <QTimer>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN MapBase::Private
{
public:
    bool isLoading;
    int tableId;
    /**
     * List of all sheets in this map.
     */
    QList<SheetBase *> lstSheets, lstDeletedSheets;
    QMap<SheetBase *, int> lstDeletedPos; // original positions of deleted sheets - this only works correctly if we restore in reverse order

    CalculationSettings *calculationSettings;
    ValueCalc *calc;
    ValueConverter *converter;
    ValueParser *parser;

    DependencyManager *dependencyManager;
    NamedAreaManager *namedAreaManager;
    RecalcManager *recalcManager;

    QList<Damage *> damages;
};

MapBase::MapBase()
    : QObject()
    , d(new Private)
{
    d->isLoading = false;

    d->tableId = 1;
    d->dependencyManager = new DependencyManager(this);
    d->namedAreaManager = new NamedAreaManager(this);
    d->recalcManager = new RecalcManager(this);
    d->calculationSettings = new CalculationSettings();

    d->parser = new ValueParser(d->calculationSettings);
    d->converter = new ValueConverter(d->parser);
    d->calc = new ValueCalc(d->converter);

    connect(this, &MapBase::damagesFlushed, this, &MapBase::handleDamages);

    connect(this, &MapBase::sheetAdded, d->dependencyManager, &DependencyManager::addSheet);
    connect(this, &MapBase::sheetAdded, d->recalcManager, &RecalcManager::addSheet);
    connect(this, &MapBase::sheetRemoved, d->dependencyManager, &DependencyManager::removeSheet);
    connect(this, &MapBase::sheetRemoved, d->recalcManager, &RecalcManager::removeSheet);
    connect(this, &MapBase::sheetRevived, d->dependencyManager, &DependencyManager::addSheet);
    connect(this, &MapBase::sheetRevived, d->recalcManager, &RecalcManager::addSheet);

    connect(d->namedAreaManager, &NamedAreaManager::namedAreaModified, d->dependencyManager, &DependencyManager::namedAreaModified);
}

MapBase::~MapBase()
{
    // we have to explicitly delete the Sheets, not let QObject take care of that
    // as the sheet in its destructor expects the Map to still exist
    for (SheetBase *sheet : d->lstSheets)
        delete sheet;
    d->lstSheets.clear();

    delete d->dependencyManager;
    delete d->namedAreaManager;
    delete d->recalcManager;

    delete d->parser;
    delete d->converter;
    delete d->calc;
    delete d->calculationSettings;

    delete d;
}

SheetBase *MapBase::sheet(int index) const
{
    return d->lstSheets.value(index);
}

int MapBase::indexOf(SheetBase *sheet) const
{
    return d->lstSheets.indexOf(sheet);
}

QList<SheetBase *> &MapBase::sheetList() const
{
    return d->lstSheets;
}

int MapBase::count() const
{
    return d->lstSheets.count();
}

SheetBase *MapBase::findSheet(const QString &_name) const
{
    for (SheetBase *sheet : d->lstSheets) {
        if (_name.toLower() == sheet->sheetName().toLower())
            return sheet;
    }
    return nullptr;
}

void MapBase::moveSheet(const QString &_from, const QString &_to, bool _before)
{
    SheetBase *sheetfrom = findSheet(_from);
    SheetBase *sheetto = findSheet(_to);

    int from = d->lstSheets.indexOf(sheetfrom);
    int to = d->lstSheets.indexOf(sheetto);
    if (!_before)
        ++to;

    if (to > (int)d->lstSheets.count()) {
        d->lstSheets.append(sheetfrom);
        d->lstSheets.removeAt(from);
    } else if (from < to) {
        d->lstSheets.insert(to, sheetfrom);
        d->lstSheets.removeAt(from);
    } else {
        d->lstSheets.removeAt(from);
        d->lstSheets.insert(to, sheetfrom);
    }
}

SheetBase *MapBase::nextSheet(SheetBase *currentSheet) const
{
    bool returnNext = false;
    for (SheetBase *sheet : d->lstSheets) {
        if (returnNext)
            return sheet;
        if (sheet == currentSheet)
            returnNext = true;
    }
    // If returnNext is set here, it means that currentSheet was last in the list.
    if (returnNext)
        return currentSheet;
    return nullptr;
}

SheetBase *MapBase::previousSheet(SheetBase *currentSheet) const
{
    SheetBase *prev = nullptr;
    for (SheetBase *sheet : d->lstSheets) {
        if (sheet == currentSheet) {
            if (prev)
                return prev;
            return currentSheet; // this means that currentSheet was first in the list
        }
        prev = sheet;
    }
    return nullptr;
}

SheetBase *MapBase::createSheet(const QString &name)
{
    QString sheetName(i18n("Sheet%1", d->tableId++));
    if (!name.isEmpty())
        sheetName = name;
    SheetBase *sheet = new SheetBase(this, sheetName);
    return sheet;
}

SheetBase *MapBase::addNewSheet(const QString &name)
{
    SheetBase *t = createSheet(name);
    addSheet(t);
    return t;
}

void MapBase::addSheet(SheetBase *_sheet)
{
    d->lstSheets.append(_sheet);
    Q_EMIT sheetAdded(_sheet);
}

void MapBase::removeSheet(SheetBase *sheet)
{
    int pos = d->lstSheets.indexOf(sheet);
    if (pos >= 0)
        d->lstDeletedPos[sheet] = pos;
    d->lstSheets.removeAll(sheet);
    d->lstDeletedSheets.append(sheet);
    namedAreaManager()->remove(sheet);
    Q_EMIT sheetRemoved(sheet);
}

void MapBase::reviveSheet(SheetBase *sheet)
{
    d->lstDeletedSheets.removeAll(sheet);
    if (d->lstDeletedPos.contains(sheet)) {
        int pos = d->lstDeletedPos.take(sheet);
        if ((pos >= 0) && (pos < d->lstSheets.size()))
            d->lstSheets.insert(pos, sheet);
        else
            d->lstSheets.append(sheet);
    } else
        d->lstSheets.append(sheet);
    Q_EMIT sheetRevived(sheet);
}

void MapBase::onSheetHidden(SheetBase *sheet, bool hidden)
{
    if (hidden) {
        addDamage(new SheetDamage(sheet, SheetDamage::Hidden));
        Q_EMIT sheetHidden(sheet);
    } else {
        addDamage(new SheetDamage(sheet, SheetDamage::Shown));
        Q_EMIT sheetShown(sheet);
    }
}

// FIXME cache this for faster operation
QStringList MapBase::visibleSheets() const
{
    QStringList result;
    for (SheetBase *sheet : sheetList()) {
        if (!sheet->isHidden())
            result.append(sheet->sheetName());
    }
    return result;
}

// FIXME cache this for faster operation
QStringList MapBase::hiddenSheets() const
{
    QStringList result;
    for (SheetBase *sheet : sheetList()) {
        if (sheet->isHidden())
            result.append(sheet->sheetName());
    }
    return result;
}

ValueParser *MapBase::parser() const
{
    return d->parser;
}

ValueConverter *MapBase::converter() const
{
    return d->converter;
}

ValueCalc *MapBase::calc() const
{
    return d->calc;
}

DependencyManager *MapBase::dependencyManager() const
{
    return d->dependencyManager;
}

NamedAreaManager *MapBase::namedAreaManager() const
{
    return d->namedAreaManager;
}

RecalcManager *MapBase::recalcManager() const
{
    return d->recalcManager;
}

CalculationSettings *MapBase::calculationSettings() const
{
    return d->calculationSettings;
}

void MapBase::addDamage(Damage *damage)
{
    // Do not create a new Damage, if we are in loading process. Check for it before
    // calling this function. This prevents unnecessary memory allocations (new).
    // see FIXME in Sheet::setSheetName().
    //     Q_ASSERT(!isLoading());
    Q_CHECK_PTR(damage);

#ifndef NDEBUG
    if (damage->type() == Damage::Cell) {
        debugSheetsDamage << "Adding\t" << *static_cast<CellDamage *>(damage);
    } else if (damage->type() == Damage::Sheet) {
        debugSheetsDamage << "Adding\t" << *static_cast<SheetDamage *>(damage);
    } else if (damage->type() == Damage::Selection) {
        debugSheetsDamage << "Adding\t" << *static_cast<SelectionDamage *>(damage);
    } else {
        debugSheetsDamage << "Adding\t" << *damage;
    }
#endif

    d->damages.append(damage);

    if (d->damages.count() == 1) {
        QTimer::singleShot(0, this, &MapBase::flushDamages);
    }
}

void MapBase::flushDamages()
{
    // Copy the damages to process. This allows new damages while processing.
    QList<Damage *> damages = d->damages;
    d->damages.clear();
    Q_EMIT damagesFlushed(damages);
    qDeleteAll(damages);
}

void MapBase::handleDamages(const QList<Damage *> &damages)
{
    Region formulaChangedRegion;
    Region namedAreaChangedRegion;
    Region valueChangedRegion;
    WorkbookDamage::Changes workbookChanges = WorkbookDamage::None;
    bool allValues = false, allFormulas = false; // These are for improved code readability

    QList<Damage *>::ConstIterator end(damages.end());
    for (QList<Damage *>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage *damage = *it;

        if (damage->type() == Damage::Cell) {
            CellDamage *cellDamage = static_cast<CellDamage *>(damage);
            debugSheetsDamage << "Processing\t" << *cellDamage;
            SheetBase *const damagedSheet = cellDamage->sheet();
            const Region &region = cellDamage->region();
            const CellDamage::Changes changes = cellDamage->changes();

            if (!allValues) {
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
            SheetDamage *sheetDamage = static_cast<SheetDamage *>(damage);
            debugSheetsDamage << "Processing\t" << *sheetDamage;
            //             SheetBase* damagedSheet = sheetDamage->sheet();

            if (sheetDamage->changes() & SheetDamage::PropertiesChanged) { }
            continue;
        }

        if (damage->type() == Damage::Workbook) {
            WorkbookDamage *workbookDamage = static_cast<WorkbookDamage *>(damage);
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
    } else {
        if (!valueChangedRegion.isEmpty())
            d->recalcManager->regionChanged(valueChangedRegion);
    }
}

bool MapBase::isLoading() const
{
    return d->isLoading;
}

void MapBase::setLoading(bool l)
{
    d->isLoading = l;
}

Region MapBase::regionFromName(const QString &expression, SheetBase *sheet) const
{
    Region res;

    if (expression.isEmpty())
        return res;

    // FIXME Stefan: Does not respect quoted names!
    QStringList substrings = expression.split(';');
    for (QString sRegion : substrings) {
        // check for a named area first
        const Region namedAreaRegion = namedAreaManager()->namedArea(sRegion);
        if (namedAreaRegion.isValid()) {
            res.add(namedAreaRegion, sheet);
            continue;
        }

        // Single cell or cell range
        int delimiterPos = sRegion.indexOf(':');
        if (delimiterPos > -1) {
            // range
            QString sUL = sRegion.left(delimiterPos);
            QString sLR = sRegion.mid(delimiterPos + 1);

            SheetBase *firstSheet = filterSheetName(sUL);
            SheetBase *lastSheet = filterSheetName(sLR);
            // TODO: lastSheet is silently ignored if it is different from firstSheet

            // Still has the sheet name separator?
            if (sUL.contains('!') || sLR.contains('!'))
                return res;

            if (!firstSheet)
                firstSheet = sheet;
            if (!lastSheet)
                lastSheet = sheet;

            // TODO: shouldn't use Region::Point, the cell name needs to be moved elsewhere
            Region::Point ul(sUL);
            Region::Point lr(sLR);

            if (ul.isValid() && lr.isValid()) {
                QRect range = QRect(ul.pos(), lr.pos());
                res.add(range, firstSheet, ul.isTopFixed(), ul.isLeftFixed(), lr.isBottomFixed(), lr.isRightFixed());
            } else if (ul.isValid()) {
                res.add(ul.pos(), firstSheet, ul.isTopFixed(), ul.isLeftFixed());
            } else { // lr.isValid()
                res.add(lr.pos(), firstSheet, lr.isTopFixed(), lr.isLeftFixed());
            }
        } else {
            // single cell
            SheetBase *targetSheet = filterSheetName(sRegion);
            // Still has the sheet name separator?
            if (sRegion.contains('!'))
                return res;
            if (!targetSheet)
                targetSheet = sheet;
            Region::Point pt(sRegion);
            res.add(pt.pos(), targetSheet, pt.isColumnFixed(), pt.isRowFixed());
        }
    }

    return res;
}

// get sheet name from a cell range string
SheetBase *MapBase::filterSheetName(QString &sRegion) const
{
    SheetBase *sheet = nullptr;
    int delimiterPos = sRegion.lastIndexOf('!');
    if (delimiterPos < 0)
        delimiterPos = sRegion.lastIndexOf('.');
    if (delimiterPos > -1) {
        QString sheetName = sRegion.left(delimiterPos);
        sheet = findSheet(sheetName);
        // try again without apostrophes
        while (!sheet && sheetName.count() > 2 && sheetName[0] == '\'' && sheetName[sheetName.count() - 1] == '\'') {
            sheetName = sheetName.mid(1, sheetName.count() - 2);
            sheet = findSheet(sheetName);
        }
        // remove the sheet name, incl. '!', from the string
        if (sheet)
            sRegion = sRegion.right(sRegion.length() - delimiterPos - 1);
    }
    return sheet;
}

bool MapBase::isNamedArea(const QString &name)
{
    return namedAreaManager()->contains(name);
}
