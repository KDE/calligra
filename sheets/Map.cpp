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
#include "CellStorage.h"
#include "Damages.h"
#include "DocBase.h"
#include "LoadingInfo.h"
#include "Localization.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "StyleManager.h"
#include "ValueFormatter.h"
#include "engine/Updater.h"

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
    StyleManager* styleManager;
    KoStyleManager* textStyleManager;

    ApplicationSettings* applicationSettings;
    ValueFormatter* formatter;

    // default objects
    ColumnFormat* defaultColumnFormat;
    RowFormat* defaultRowFormat;

    int syntaxVersion;

    KCompletion listCompletion;
};


class Q_DECL_HIDDEN SheetUpdater : public Updater {
public:
    SheetUpdater(KoUpdater *realUpdater) {
        m_updater = realUpdater;
    }

    virtual void setProgress(int percent) override {
        m_updater->setProgress(percent);
    }

private:
    KoUpdater m_updater;
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
    d->styleManager = new StyleManager();
    d->textStyleManager = new KoStyleManager(this);
    d->applicationSettings = new ApplicationSettings();

    d->formatter = new ValueFormatter(d->converter);

    d->defaultColumnFormat = new ColumnFormat();
    d->defaultRowFormat = new RowFormat();

    QFont font(KoGlobal::defaultFont());
    d->defaultRowFormat->setHeight(font.pointSizeF() + 4);
    d->defaultColumnFormat->setWidth((font.pointSizeF() + 4) * 5);

    // default document properties
    d->syntaxVersion = syntaxVersion;

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
    delete d->styleManager;

    delete d->formatter;

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
    // These exist so that we don't pull in a kowidgets dependency into engine
    SheetUpdater *depWrapper = nullptr, recalcWrapper = nullptr;

    if (doc() && doc()->progressUpdater()) {
        dependencyUpdater = doc()->progressUpdater()->startSubtask(1, "Calligra::Sheets::DependencyManager::updateAllDependencies");
        recalcUpdater = doc()->progressUpdater()->startSubtask(1, "Calligra::Sheets::RecalcManager::recalc");
        depWrapper = new SheetUpdater(dependencyUpdater);
        recalcWrapper = new SheetUpdater(recalcUpdater);
    }


    // Initial build of all cell dependencies.
    d->dependencyManager->updateAllDependencies(this, depWrapper);
    // Recalc the whole workbook now, since there may be formulas other spreadsheets support,
    // but Calligra Sheets does not.
    d->recalcManager->recalcMap(recalcWrapper);

    delete depWrapper;
    delete recalcWrapper;

    return true;
}

bool Map::completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext * context)
{
    Q_UNUSED(store);
    Q_UNUSED(manifestWriter);
    Q_UNUSED(context);
    return true;
}

BindingManager* MapBase::bindingManager() const
{
    return d->bindingManager;
}

DatabaseManager* Map::databaseManager() const
{
    return d->databaseManager;
}

StyleManager* Map::styleManager() const
{
    return d->styleManager;
}

KoStyleManager* Map::textStyleManager() const
{
    return d->textStyleManager;
}

ValueFormatter* Map::formatter() const
{
    return d->formatter;
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

    // The base class does most of the work here, we just process bindings and add style invalidation.

    Region bindingChangedRegion;
    bool allValues = false;

    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;

        if (damage->type() == Damage::Cell) {
            CellDamage* cellDamage = static_cast<CellDamage*>(damage);
            SheetBase* const damagedSheet = cellDamage->sheet();
            const Region& region = cellDamage->region();
            const CellDamage::Changes changes = cellDamage->changes();

            if (!allValues) {
                if (changes & CellDamage::Binding)
                    bindingChangedRegion.add(region, damagedSheet);
            }

            // TODO Stefan: Detach the style cache from the CellView cache.
            if (damagedSheet && changes.testFlag(CellDamage::Appearance))
                // Rebuild the style storage cache.
                damagedSheet->fullCellStorage()->invalidateStyleCache(); // FIXME more fine-grained
            continue;
        }

        if (damage->type() == Damage::Workbook) {
            WorkbookDamage* workbookDamage = static_cast<WorkbookDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *damage;
            const WorkbookDamage::Changes changes = workbookDamage->changes();

            if (changes & WorkbookDamage::Value)
                allValues = true;
            continue;
        }
    }

    if (allValues) {
        d->bindingManager->updateAllBindings();
    } else {
        if (!bindingChangedRegion.isEmpty())
            d->bindingManager->regionChanged(bindingChangedRegion);
    }
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
