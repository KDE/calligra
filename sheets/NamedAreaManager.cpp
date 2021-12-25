/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2005-2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "NamedAreaManager.h"

// Qt
#include <QHash>

// Sheets
#include "CellStorage.h"
#include "FormulaStorage.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "Region.h"
#include "Sheet.h"
#include "Util.h"

using namespace Calligra::Sheets;

struct NamedArea {
    QString name;
    Sheet* sheet;
    QRect range;
};

class Q_DECL_HIDDEN NamedAreaManager::Private
{
public:
    const Map* map;
    QHash<QString, NamedArea> namedAreas;
};

NamedAreaManager::NamedAreaManager(const Map* map)
        : d(new Private)
{
    d->map = map;
    connect(this, &NamedAreaManager::namedAreaAdded,
            this, &NamedAreaManager::namedAreaModified);
    connect(this, &NamedAreaManager::namedAreaRemoved,
            this, &NamedAreaManager::namedAreaModified);
}

NamedAreaManager::~NamedAreaManager()
{
    delete d;
}

const Map *NamedAreaManager::map() const
{
    return d->map;
}

void NamedAreaManager::insert(const Region& region, const QString& name)
{
    // NOTE Stefan: Only contiguous regions are supported (OpenDocument compatibility).
    Q_ASSERT(!name.isEmpty());
    NamedArea namedArea;
    namedArea.range = region.lastRange();
    namedArea.sheet = region.lastSheet();
    namedArea.name = name;
    namedArea.sheet->cellStorage()->setNamedArea(Region(region.lastRange(), region.lastSheet()), name);
    d->namedAreas[name] = namedArea;
    emit namedAreaAdded(name);
}

void NamedAreaManager::remove(const QString& name)
{
    if (!d->namedAreas.contains(name))
        return;
    NamedArea namedArea = d->namedAreas.value(name);
    namedArea.sheet->cellStorage()->removeNamedArea(Region(namedArea.range, namedArea.sheet), name);
    d->namedAreas.remove(name);
    emit namedAreaRemoved(name);
    const QList<Sheet*> sheets = namedArea.sheet->map()->sheetList();
    foreach(Sheet* sheet, sheets) {
        const QString tmp = '\'' + name + '\'';
        const FormulaStorage* const storage = sheet->formulaStorage();
        for (int c = 0; c < storage->count(); ++c) {
            if (storage->data(c).expression().contains(tmp)) {
                Cell(sheet, storage->col(c), storage->row(c)).makeFormula();
            }
        }
    }
}

void NamedAreaManager::remove(Sheet* sheet)
{
    const QList<NamedArea> namedAreas = d->namedAreas.values();
    for (int i = 0; i < namedAreas.count(); ++i) {
        if (namedAreas[i].sheet == sheet)
            remove(namedAreas[i].name);
    }
}

Calligra::Sheets::Region NamedAreaManager::namedArea(const QString& name) const
{
    if (!d->namedAreas.contains(name))
        return Region();
    const NamedArea namedArea = d->namedAreas.value(name);
    return Region(namedArea.range, namedArea.sheet);
}

Sheet* NamedAreaManager::sheet(const QString& name) const
{
    if (!d->namedAreas.contains(name))
        return 0;
    return d->namedAreas.value(name).sheet;
}

bool NamedAreaManager::contains(const QString& name) const
{
    return d->namedAreas.contains(name);
}

QList<QString> NamedAreaManager::areaNames() const
{
    return d->namedAreas.keys();
}

void NamedAreaManager::regionChanged(const Region& region)
{
    Sheet* sheet;
    QList< QPair<QRectF, QString> > namedAreas;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        sheet = (*it)->sheet();
        namedAreas = sheet->cellStorage()->namedAreas(Region((*it)->rect(), sheet));
        for (int j = 0; j < namedAreas.count(); ++j) {
            Q_ASSERT(d->namedAreas.contains(namedAreas[j].second));
            d->namedAreas[namedAreas[j].second].range = namedAreas[j].first.toRect();
            emit namedAreaModified(namedAreas[j].second);
        }
    }
}

void NamedAreaManager::updateAllNamedAreas()
{
    QList< QPair<QRectF, QString> > namedAreas;
    const QRect rect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax));
    const QList<Sheet*> sheets = d->map->sheetList();
    for (int i = 0; i < sheets.count(); ++i) {
        namedAreas = sheets[i]->cellStorage()->namedAreas(Region(rect, sheets[i]));
        for (int j = 0; j < namedAreas.count(); ++j) {
            Q_ASSERT(d->namedAreas.contains(namedAreas[j].second));
            d->namedAreas[namedAreas[j].second].range = namedAreas[j].first.toRect();
            emit namedAreaModified(namedAreas[j].second);
        }
    }
}


