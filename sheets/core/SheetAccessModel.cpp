/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Ours
#include "SheetAccessModel.h"
#include "Map.h"

#include "engine/calligra_sheets_limits.h"
#include "engine/Damages.h"
#include "engine/Region.h"
#include "engine/SheetBase.h"

#include "BindingManager.h"

// Qt
#include <QAbstractItemModel>
#include <QPointer>
#include <QVariant>

Q_DECLARE_METATYPE(QPointer<QAbstractItemModel>)

namespace Calligra
{
namespace Sheets
{

class SheetAccessModel::Private
{
public:
    Map *map;
    /// Stores in what column each Sheet is. We need this because
    /// a Sheet is removed from its Map before the sheetRemoved() signal
    /// is emitted, thus we can't ask the Map what index it had.
    QMap<SheetBase*, int> cols;
};

SheetAccessModel::SheetAccessModel(Map *map)
        : d(new Private)
{
    d->map = map;

    connect(map, &Map::sheetAdded,
            this, &SheetAccessModel::slotSheetAdded);
    // FIXME: Check if we can simply connect sheetRevived() to slotSheetAdded()
    connect(map, &Map::sheetRevived,
            this, &SheetAccessModel::slotSheetAdded);
    connect(map, &Map::sheetRemoved,
            this, &SheetAccessModel::slotSheetRemoved);
    connect(map, &Map::damagesFlushed,
            this, &SheetAccessModel::handleDamages);

    setRowCount(1);
    setColumnCount(0);
}

SheetAccessModel::~SheetAccessModel()
{
    delete d;
}

void SheetAccessModel::slotSheetAdded(SheetBase *sheet)
{
    Q_ASSERT(!d->cols.contains(sheet));

    QStandardItem *item = new QStandardItem;
    QList<QStandardItem*> col;
    col.append(item);

    // This region contains the entire sheet
    const Region region(1, 1, KS_colMax, KS_rowMax, sheet);
    const QPointer<QAbstractItemModel> model = const_cast<QAbstractItemModel*>( d->map->bindingManager()->createModel( region.name() ) );

    item->setData( QVariant::fromValue( model ), Qt::DisplayRole );

    const int sheetIndex = d->map->indexOf( sheet );
    d->cols.insert(sheet, sheetIndex);

    insertColumn( sheetIndex, col );
    setHeaderData( sheetIndex, Qt::Horizontal, sheet->sheetName() );
}

void SheetAccessModel::slotSheetRemoved(SheetBase *sheet)
{
    Q_ASSERT(d->cols.contains(sheet));
    removeColumn(d->cols[sheet]);
    d->cols.remove(sheet);
}

void SheetAccessModel::handleDamages(const QList<Damage*>& damages)
{
    QList<Damage*>::ConstIterator end(damages.end());
    for (QList<Damage*>::ConstIterator it = damages.begin(); it != end; ++it) {
        Damage* damage = *it;
        if (!damage) {
            continue;
        }

        if (damage->type() == Damage::Sheet) {
            SheetDamage* sheetDamage = static_cast<SheetDamage*>(damage);
            debugSheetsDamage << "Processing\t" << *sheetDamage;

            if (sheetDamage->changes() & SheetDamage::Name) {
                SheetBase *sheet = sheetDamage->sheet();
                // We should never receive signals from sheets that are not in our model
                Q_ASSERT(d->cols.contains(sheet));
                const int sheetIndex = d->cols[sheet];
                setHeaderData(sheetIndex, Qt::Horizontal, sheet->sheetName());
            }
            continue;
        }
    }
}

} // namespace Sheets
} // namespace Calligra
