/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "BindingManager.h"

#include "engine/calligra_sheets_limits.h"
#include "engine/MapBase.h"
#include "CellStorage.h"
#include "Sheet.h"
#include "Binding.h"
#include "BindingStorage.h"
#include "BindingModel.h"

using namespace Calligra::Sheets;

class BindingManager::Private
{
public:
    const MapBase* map;
};

BindingManager::BindingManager(const MapBase* map)
        : d(new Private)
{
    d->map = map;
}

BindingManager::~BindingManager()
{
    delete d;
}

const QAbstractItemModel* BindingManager::createModel(const QString& regionName)
{
    const Region region = d->map->regionFromName(regionName, nullptr);
    if (!region.isValid() || !region.isContiguous() || !region.firstSheet()) {
        return 0;
    }
    Binding binding(region);
    SheetBase *sheet = region.firstSheet();
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
    fullSheet->fullCellStorage()->setBinding(region, binding);
    return binding.model();
}

bool BindingManager::removeModel(const QAbstractItemModel* model)
{
    QVector< QPair<QRectF, Binding> > bindings;
    const QRect rect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax));
    const QList<SheetBase*> sheets = d->map->sheetList();
    for (int i = 0; i < sheets.count(); ++i) {
        SheetBase* const sheet = sheets[i];
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        bindings = fullSheet->fullCellStorage()->bindingStorage()->intersectingPairs(Region(rect, sheet));
        for (int j = 0; j < bindings.count(); ++j) {
            if (bindings[j].second.model() == model) {
                const Region region(bindings[j].first.toRect(), sheet);
                fullSheet->fullCellStorage()->removeBinding(region, bindings[j].second);
                return true;
            }
        }
    }
    return false;
}

bool BindingManager::isCellRegionValid(const QString& regionName) const
{
    const Region region = d->map->regionFromName(regionName, nullptr);
    return (region.isValid() && region.isContiguous() && region.firstSheet());
}

void BindingManager::regionChanged(const Region& region)
{
    QVector< QPair<QRectF, Binding> > bindings;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        SheetBase *sheet = (*it)->sheet();
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
        const Region changedRegion((*it)->rect(), sheet);
        bindings = fullSheet->fullCellStorage()->bindingStorage()->intersectingPairs(changedRegion);
        for (int j = 0; j < bindings.count(); ++j)
            bindings[j].second.update(changedRegion);
    }
}

void BindingManager::updateAllBindings()
{
    QVector< QPair<QRectF, Binding> > bindings;
    const QRect rect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax));
    const QList<SheetBase*> sheets = d->map->sheetList();
    for (int i = 0; i < sheets.count(); ++i) {
        Sheet *fullSheet = dynamic_cast<Sheet *>(sheets[i]);
        bindings = fullSheet->fullCellStorage()->bindingStorage()->intersectingPairs(Region(rect, sheets[i]));
        for (int j = 0; j < bindings.count(); ++j)
            bindings[j].second.update(Region(bindings[j].first.toRect(), sheets[i]));
    }
}

