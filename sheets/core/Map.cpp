/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Map.h"

#include <KLocalizedString>

#include <KoGlobal.h>
#include <KoProgressUpdater.h>
#include <KoStyleManager.h>
#include <KoUpdater.h>

#include "engine/Damages.h"
#include "engine/DependencyManager.h"
#include "engine/RecalcManager.h"
#include "engine/Region.h"
#include "engine/SheetBase.h"
#include "engine/SheetsDebug.h"
#include "engine/Updater.h"

#include "ApplicationSettings.h"
#include "BindingManager.h"
#include "CellStorage.h"
#include "ColFormatStorage.h"
#include "DocBase.h"
#include "LoadingInfo.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "SheetAccessModel.h"
#include "StyleManager.h"
#include "ValueFormatter.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Map::Private
{
public:
    DocBase *doc;

    // used to give every Sheet a unique default name.
    int tableId;

    // used to determine the loading progress
    int overallRowCount;
    int loadedRowsCounter;

    LoadingInfo *loadingInfo;
    bool readwrite;

    ApplicationSettings *settings;
    BindingManager *bindingManager;
    StyleManager *styleManager;
    KoStyleManager *textStyleManager;

    SheetAccessModel *sheetAccessModel;
    ValueFormatter *formatter;

    // default objects
    ColFormat defaultColumnFormat;
    RowFormat defaultRowFormat;

    int syntaxVersion;
};

class Q_DECL_HIDDEN SheetUpdater : public Updater
{
public:
    SheetUpdater(KoUpdater *realUpdater)
    {
        m_updater = realUpdater;
    }

    virtual void setProgress(int percent) override
    {
        m_updater->setProgress(percent);
    }

private:
    KoUpdater *m_updater;
};

Map::Map(DocBase *doc, int syntaxVersion)
    : MapBase()
    , d(new Private)
{
    setObjectName(QLatin1String("Map")); // necessary for D-Bus
    d->doc = doc;
    d->tableId = 1;
    d->overallRowCount = 0;
    d->loadedRowsCounter = 0;
    d->loadingInfo = 0;
    d->readwrite = true;

    d->bindingManager = new BindingManager(this);
    d->styleManager = new StyleManager();
    d->textStyleManager = new KoStyleManager(this);
    d->settings = new ApplicationSettings;

    d->sheetAccessModel = new SheetAccessModel(this);
    d->formatter = new ValueFormatter(converter());

    QFont font(KoGlobal::defaultFont());
    d->defaultRowFormat.height = font.pointSizeF() + 4;
    d->defaultColumnFormat.width = (font.pointSizeF() + 4) * 5;

    // default document properties
    d->syntaxVersion = syntaxVersion;
}

Map::~Map()
{
    // Because some of the shapes might be using a sheet in this map, delete
    // all shapes in each sheet before all sheets are deleted together.
    for (SheetBase *sheet : sheetList()) {
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        if (fullSheet)
            fullSheet->deleteShapes();
    }

    deleteLoadingInfo();

    delete d->settings;
    delete d->bindingManager;
    delete d->styleManager;

    delete d->sheetAccessModel;
    delete d->formatter;

    delete d;
}

DocBase *Map::doc() const
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
    SheetUpdater *depWrapper = nullptr, *recalcWrapper = nullptr;

    if (doc() && doc()->progressUpdater()) {
        dependencyUpdater = doc()->progressUpdater()->startSubtask(1, "Calligra::Sheets::DependencyManager::updateAllDependencies");
        recalcUpdater = doc()->progressUpdater()->startSubtask(1, "Calligra::Sheets::RecalcManager::recalc");
        depWrapper = new SheetUpdater(dependencyUpdater);
        recalcWrapper = new SheetUpdater(recalcUpdater);
    }

    // Initial build of all cell dependencies.
    dependencyManager()->updateAllDependencies(this, depWrapper);
    // Recalc the whole workbook now, since there may be formulas other spreadsheets support,
    // but Calligra Sheets does not.
    recalcManager()->recalcMap(recalcWrapper);

    delete depWrapper;
    delete recalcWrapper;

    return true;
}

bool Map::completeSaving(KoStore *store, KoXmlWriter *manifestWriter, KoShapeSavingContext *context)
{
    Q_UNUSED(store);
    Q_UNUSED(manifestWriter);
    Q_UNUSED(context);
    return true;
}

ApplicationSettings *Map::applicationSettings() const
{
    return d->settings;
}

BindingManager *Map::bindingManager() const
{
    return d->bindingManager;
}

StyleManager *Map::styleManager() const
{
    return d->styleManager;
}

// TODO - do we actually need this? It's only used in load/save, nowhere else.
KoStyleManager *Map::textStyleManager() const
{
    return d->textStyleManager;
}

ValueFormatter *Map::formatter() const
{
    return d->formatter;
}

SheetAccessModel *Map::sheetAccessModel() const
{
    return d->sheetAccessModel;
}

const ColFormat Map::defaultColumnFormat() const
{
    return d->defaultColumnFormat;
}

const RowFormat Map::defaultRowFormat() const
{
    return d->defaultRowFormat;
}

void Map::setDefaultColumnWidth(double width)
{
    d->defaultColumnFormat.width = width;
}

void Map::setDefaultRowHeight(double height)
{
    d->defaultRowFormat.height = height;
}

// This fully overrides the base routine.
SheetBase *Map::createSheet(const QString &name)
{
    QString sheetName(i18n("Sheet%1", d->tableId++));
    if (!name.isEmpty())
        sheetName = name;
    Sheet *sheet = new Sheet(this, sheetName);
    connect(sheet, &Sheet::statusMessage, this, &Map::statusMessage);
    return sheet;
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

LoadingInfo *Map::loadingInfo() const
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

void Map::handleDamages(const QList<Damage *> &damages)
{
    MapBase::handleDamages(damages);

    // The base class does most of the work here, we just process bindings and add style invalidation.

    Region bindingChangedRegion;
    bool allValues = false;

    QList<Damage *>::ConstIterator end(damages.end());
    for (QList<Damage *>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage *damage = *it;

        if (damage->type() == Damage::Cell) {
            CellDamage *cellDamage = static_cast<CellDamage *>(damage);
            SheetBase *const damagedSheet = cellDamage->sheet();
            const Region &region = cellDamage->region();
            const CellDamage::Changes changes = cellDamage->changes();

            if (!allValues) {
                if (changes & CellDamage::Binding)
                    bindingChangedRegion.add(region, damagedSheet);
            }

            // TODO Stefan: Detach the style cache from the CellView cache.
            if (damagedSheet && changes.testFlag(CellDamage::Appearance)) {
                Sheet *fullSheet = dynamic_cast<Sheet *>(damagedSheet);
                // Rebuild the style storage cache.
                if (fullSheet)
                    fullSheet->fullCellStorage()->invalidateStyleCache(); // FIXME more fine-grained
            }
            continue;
        }

        if (damage->type() == Damage::Workbook) {
            WorkbookDamage *workbookDamage = static_cast<WorkbookDamage *>(damage);
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

KoDocumentResourceManager *Map::resourceManager() const
{
    if (!doc())
        return 0;
    return doc()->resourceManager();
}
