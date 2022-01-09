/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Map.h"

#include <stdlib.h>
#include <time.h>

#include <QTimer>

#include <kcodecs.h>
#include <kcompletion.h>

#include <KoGlobal.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoUpdater.h>
#include <KoProgressUpdater.h>

#include "ApplicationSettings.h"
#include "BindingManager.h"
#include "CalculationSettings.h"
#include "CellStorage.h"
#include "Damages.h"
#include "DependencyManager.h"
#include "DocBase.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "NamedAreaManager.h"
#include "RecalcManager.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "StyleManager.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "ValueFormatter.h"
#include "ValueParser.h"

// database
#include "database/DatabaseManager.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Map::Private
{
public:
    DocBase* doc;

    QList<Sheet*> lstDeletedSheets;

    // used to give every Sheet a unique default name.
    int tableId;

    // used to determine the loading progress
    int overallRowCount;
    int loadedRowsCounter;

    LoadingInfo* loadingInfo;
    bool readwrite;

    BindingManager* bindingManager;
    DatabaseManager* databaseManager;
    DependencyManager* dependencyManager;
    NamedAreaManager* namedAreaManager;
    RecalcManager* recalcManager;
    StyleManager* styleManager;
    KoStyleManager* textStyleManager;

    ApplicationSettings* applicationSettings;
    CalculationSettings* calculationSettings;
    ValueCalc* calc;
    ValueConverter* converter;
    ValueFormatter* formatter;
    ValueParser* parser;

    // default objects
    ColumnFormat* defaultColumnFormat;
    RowFormat* defaultRowFormat;

    int syntaxVersion;

    KCompletion listCompletion;
};


Map::Map(DocBase* doc, int syntaxVersion)
        :
        MapBase(),
        d(new Private)
{
    setObjectName(QLatin1String("Map")); // necessary for D-Bus
    d->doc = doc;
    d->tableId = 1;
    d->overallRowCount = 0;
    d->loadedRowsCounter = 0;
    d->loadingInfo = 0;
    d->readwrite = true;

    d->bindingManager = new BindingManager(this);
    d->databaseManager = new DatabaseManager();
    d->dependencyManager = new DependencyManager(this);
    d->namedAreaManager = new NamedAreaManager(this);
    d->recalcManager = new RecalcManager(this);
    d->styleManager = new StyleManager();
    d->textStyleManager = new KoStyleManager(this);
    d->applicationSettings = new ApplicationSettings();
    d->calculationSettings = new CalculationSettings();

    d->parser = new ValueParser(d->calculationSettings);
    d->converter = new ValueConverter(d->parser);
    d->calc = new ValueCalc(d->converter);
    d->formatter = new ValueFormatter(d->converter);

    d->defaultColumnFormat = new ColumnFormat();
    d->defaultRowFormat = new RowFormat();

    QFont font(KoGlobal::defaultFont());
    d->defaultRowFormat->setHeight(font.pointSizeF() + 4);
    d->defaultColumnFormat->setWidth((font.pointSizeF() + 4) * 5);

    // default document properties
    d->syntaxVersion = syntaxVersion;

    connect(this, &Map::sheetAdded,
            d->dependencyManager, &DependencyManager::addSheet);
    connect(this, &Map::sheetAdded,
            d->recalcManager, &RecalcManager::addSheet);
    connect(this, &Map::sheetRemoved,
            d->dependencyManager, &DependencyManager::removeSheet);
    connect(this, &Map::sheetRemoved,
            d->recalcManager, &RecalcManager::removeSheet);
    connect(this, &Map::sheetRevived,
            d->dependencyManager, &DependencyManager::addSheet);
    connect(this, &Map::sheetRevived,
            d->recalcManager, &RecalcManager::addSheet);
    connect(d->namedAreaManager, &NamedAreaManager::namedAreaModified,
            d->dependencyManager, &DependencyManager::namedAreaModified);
}

Map::~Map()
{
    // Because some of the shapes might be using a sheet in this map, delete
    // all shapes in each sheet before all sheets are deleted together.
    foreach(Sheet *sheet, d->lstSheets)
        sheet->deleteShapes();
    // we have to explicitly delete the Sheets, not let QObject take care of that
    // as the sheet in its destructor expects the Map to still exist
    qDeleteAll(d->lstSheets);
    d->lstSheets.clear();

    deleteLoadingInfo();

    delete d->bindingManager;
    delete d->databaseManager;
    delete d->dependencyManager;
    delete d->namedAreaManager;
    delete d->recalcManager;
    delete d->styleManager;

    delete d->parser;
    delete d->formatter;
    delete d->converter;
    delete d->calc;
    delete d->calculationSettings;
    delete d->applicationSettings;

    delete d->defaultColumnFormat;
    delete d->defaultRowFormat;

    delete d;
}

DocBase* Map::doc() const
{
    return d->doc;
}

void Map::setReadWrite(bool readwrite)
{
    d->readwrite = readwrite;
}

bool Map::isReadWrite() const
{
    return d->readwrite;
}

bool Map::completeLoading(KoStore *store)
{
    Q_UNUSED(store);

    QPointer<KoUpdater> dependencyUpdater, recalcUpdater;
    if (doc() && doc()->progressUpdater()) {
        dependencyUpdater = doc()->progressUpdater()->startSubtask(1, "Calligra::Sheets::DependencyManager::updateAllDependencies");
        recalcUpdater = doc()->progressUpdater()->startSubtask(1, "Calligra::Sheets::RecalcManager::recalc");
    }

    // Initial build of all cell dependencies.
    d->dependencyManager->updateAllDependencies(this, dependencyUpdater);
    // Recalc the whole workbook now, since there may be formulas other spreadsheets support,
    // but Calligra Sheets does not.
    d->recalcManager->recalcMap(recalcUpdater);

    return true;
}

bool Map::completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext * context)
{
    Q_UNUSED(store);
    Q_UNUSED(manifestWriter);
    Q_UNUSED(context);
    return true;
}

BindingManager* Map::bindingManager() const
{
    return d->bindingManager;
}

DatabaseManager* Map::databaseManager() const
{
    return d->databaseManager;
}

DependencyManager* Map::dependencyManager() const
{
    return d->dependencyManager;
}

NamedAreaManager* Map::namedAreaManager() const
{
    return d->namedAreaManager;
}

RecalcManager* Map::recalcManager() const
{
    return d->recalcManager;
}

StyleManager* Map::styleManager() const
{
    return d->styleManager;
}

KoStyleManager* Map::textStyleManager() const
{
    return d->textStyleManager;
}

ValueParser* Map::parser() const
{
    return d->parser;
}

ValueFormatter* Map::formatter() const
{
    return d->formatter;
}

ValueConverter* Map::converter() const
{
    return d->converter;
}

ValueCalc* Map::calc() const
{
    return d->calc;
}

const ColumnFormat* Map::defaultColumnFormat() const
{
    return d->defaultColumnFormat;
}

const RowFormat* Map::defaultRowFormat() const
{
    return d->defaultRowFormat;
}

void Map::setDefaultColumnWidth(double width)
{
    d->defaultColumnFormat->setWidth(width);
}

void Map::setDefaultRowHeight(double height)
{
    d->defaultRowFormat->setHeight(height);
}

ApplicationSettings* Map::settings() const
{
    return d->applicationSettings;
}

CalculationSettings* Map::calculationSettings() const
{
    return d->calculationSettings;
}

Sheet* Map::createSheet(const QString& name)
{
    QString sheetName(i18n("Sheet%1", d->tableId++));
    if ( !name.isEmpty() )
        sheetName = name;
    Sheet* sheet = new Sheet(this, sheetName);
    connect(sheet, &Sheet::statusMessage,
            this, &Map::statusMessage);
    return sheet;
}

void Map::addSheet(Sheet *_sheet)
{
    d->lstSheets.append(_sheet);
    emit sheetAdded(_sheet);
}

Sheet *Map::addNewSheet(const QString& name)
{
    Sheet *t = createSheet(name);
    addSheet(t);
    return t;
}

void Map::moveSheet(const QString & _from, const QString & _to, bool _before)
{
    Sheet* sheetfrom = findSheet(_from);
    Sheet* sheetto = findSheet(_to);

    int from = d->lstSheets.indexOf(sheetfrom) ;
    int to = d->lstSheets.indexOf(sheetto) ;
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

bool Map::loadChildren(KoStore * _store)
{
    foreach(Sheet* sheet, d->lstSheets) {
        if (!sheet->loadChildren(_store))
            return false;
    }
    return true;
}

void Map::removeSheet(Sheet* sheet)
{
    d->lstSheets.removeAll(sheet);
    d->lstDeletedSheets.append(sheet);
    d->namedAreaManager->remove(sheet);
    emit sheetRemoved(sheet);
}

void Map::reviveSheet(Sheet* sheet)
{
    d->lstDeletedSheets.removeAll(sheet);
    d->lstSheets.append(sheet);
    emit sheetRevived(sheet);
}

// FIXME cache this for faster operation
QStringList Map::visibleSheets() const
{
    QStringList result;
    foreach(Sheet* sheet, d->lstSheets) {
        if (!sheet->isHidden())
            result.append(sheet->sheetName());
    }
    return result;
}

// FIXME cache this for faster operation
QStringList Map::hiddenSheets() const
{
    QStringList result;
    foreach(Sheet* sheet, d->lstSheets) {
        if (sheet->isHidden())
            result.append(sheet->sheetName());
    }
    return result;
}

void Map::setOverallRowsCounter(int number)
{
    d->overallRowCount = number;
}

int Map::increaseLoadedRowsCounter(int number)
{
    d->loadedRowsCounter += number;
    if (d->overallRowCount) {
        return 100 * d->loadedRowsCounter / d->overallRowCount;
    }
    return -1;
}

bool Map::isLoading() const
{
    // The KoDocument state is necessary to avoid damages while importing a file (through a filter).
    return MapBase::isLoading() || (d->doc && d->doc->isLoading());
}

int Map::syntaxVersion() const
{
    return d->syntaxVersion;
}

void Map::setSyntaxVersion(int version)
{
    d->syntaxVersion = version;
}

LoadingInfo* Map::loadingInfo() const
{
    if (!d->loadingInfo) {
        d->loadingInfo = new LoadingInfo();
    }
    return d->loadingInfo;
}

void Map::deleteLoadingInfo()
{
    delete d->loadingInfo;
    d->loadingInfo = 0;
}

KCompletion& Map::stringCompletion()
{
    return d->listCompletion;
}

void Map::addStringCompletion(const QString &stringCompletion)
{
    if (d->listCompletion.items().contains(stringCompletion) == 0) {
        d->listCompletion.addItem(stringCompletion);
    }
}

void Map::handleDamages(const QList<Damage*>& damages)
{
    MapBase::handleDamages(damages);

    // The base class does most of the work here, we just add style invalidation.

    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            Sheet* damagedSheet = dynamic_cast<Sheet *>(cellDamage->sheet());
            const CellDamage::Changes changes = cellDamage->changes();

            // TODO Stefan: Detach the style cache from the CellView cache.
            if (damagedSheet && changes.testFlag(CellDamage::Appearance))
                // Rebuild the style storage cache.
                damagedSheet->fullCellStorage()->invalidateStyleCache(); // FIXME more fine-grained
        }
    }
}


bool Map::isNamedArea (const QString &name) {
    return namedAreaManager()->contains(name);
}

Region & Map::regionFromName(const QString& expression, Sheet* sheet = 0)
{
    Region res;

    if (string.isEmpty()) return res;

    // FIXME Stefan: Does not respect quoted names!
    QStringList substrings = string.split(';');
    QStringList::ConstIterator end = substrings.constEnd();
    for (QStringList::ConstIterator it = substrings.constBegin(); it != end; ++it) {
        QString sRegion = *it;

        // check for a named area first
        const Region namedAreaRegion = map->namedAreaManager()->namedArea(sRegion);
        if (namedAreaRegion.isValid()) 
            res.add(namedAreaRegion, sheet);
            continue;
        }

        // Single cell or cell range
        int delimiterPos = sRegion.indexOf(':');
        if (delimiterPos > -1) {
            // range
            QString sUL = sRegion.left(delimiterPos);
            QString sLR = sRegion.mid(delimiterPos + 1);

            Sheet* firstSheet = filterSheetName(sUL);
            Sheet* lastSheet = filterSheetName(sLR);
            // TODO: lastSheet is silently ignored if it is different from firstSheet

            // Still has the sheet name separator?
            if (sUL.contains('!') || sLR.contains('!'))
                return res;

            if (!firstSheet)
                firstSheet = fallbackSheet;
            if (!lastSheet)
                lastSheet = fallbackSheet;

            // TODO: shouldn't use Region::Point, the cell name needs to be moved elsewhere
            Region::Point ul(sUL);
            Region::Point lr(sLR);

            if (ul.isValid() && lr.isValid()) {
                QRect range = QRect(ul.pos(), lr.pos());
                res.add(range, firstSheet);
            } else if (ul.isValid()) {
                res.add(ul.pos(), firstSheet);
            } else { // lr.isValid()
                res.add(lr.pos(), firstSheet);
            }
        } else {
            // single cell
            Sheet* sheet = map ? filterSheetName(sRegion) : 0;
            // Still has the sheet name separator?
            if (sRegion.contains('!'))
                return res;
            if (!sheet)
                sheet = fallbackSheet;
            Region::Point pt(sRegion);
            res.add(pt.pos(), sheet);
        }
    }

    return res;
}

// get sheet name from a cell range string
Sheet* Map::filterSheetName(QString& sRegion)
{
    Sheet* sheet = 0;
    int delimiterPos = sRegion.lastIndexOf('!');
    if (delimiterPos < 0)
        delimiterPos = sRegion.lastIndexOf('.');
    if (delimiterPos > -1) {
        QString sheetName = sRegion.left(delimiterPos);
        sheet = findSheet(sheetName);
        // try again without apostrophes
        while(!sheet && sheetName.count() > 2 && sheetName[0] == '\'' && sheetName[sheetName.count()-1] == '\'') {
            sheetName = sheetName.mid(1, sheetName.count() - 2);
            sheet = findSheet(sheetName);
        }
        // remove the sheet name, incl. '!', from the string
        if (sheet)
            sRegion = sRegion.right(sRegion.length() - delimiterPos - 1);
    }
    return sheet;
}


void Map::addCommand(KUndo2Command *command)
{
    emit commandAdded(command);
}

KoDocumentResourceManager* Map::resourceManager() const
{
    if (!doc()) return 0;
    return doc()->resourceManager();
}
